/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Rendering/PostProcess/BloomEffect.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/Utils/Profiling.h>

#include <baregl/data/TextureDesc.h>

// Implementation reference: https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

// Additional private constants for the bloom effect
namespace OvCore::Rendering::PostProcess::BloomConstants
{
	constexpr float kFilterRadius = 0.005f;
}

namespace
{
	const auto kBloomTextureDesc = baregl::data::TextureDesc{
		.width = 1, // Unknown size at this point
		.height = 1,
		.minFilter = baregl::types::ETextureFilteringMode::LINEAR,
		.magFilter = baregl::types::ETextureFilteringMode::LINEAR,
		.horizontalWrap = baregl::types::ETextureWrapMode::CLAMP_TO_EDGE,
		.verticalWrap = baregl::types::ETextureWrapMode::CLAMP_TO_EDGE,
		.internalFormat = baregl::types::EInternalFormat::RGBA32F,
		.useMipMaps = false,
		.mutableDesc = baregl::data::MutableTextureDesc{
			.format = baregl::types::EFormat::RGBA,
			.type = baregl::types::EPixelDataType::FLOAT
		}
	};

	struct BloomPass
	{
		uint16_t width;
		uint16_t height;
		baregl::Framebuffer& target;
	};
}

OvCore::Rendering::PostProcess::BloomEffect::BloomEffect(OvRendering::Core::CompositeRenderer& p_renderer) :
	AEffect(p_renderer),
	m_bloomSamplingBuffers{
		baregl::Framebuffer{"BloomSamplingBuffer0"},
		baregl::Framebuffer{"BloomSamplingBuffer1"},
		baregl::Framebuffer{"BloomSamplingBuffer2"},
		baregl::Framebuffer{"BloomSamplingBuffer3"},
		baregl::Framebuffer{"BloomSamplingBuffer4"},
		baregl::Framebuffer{"BloomSamplingBuffer5"},
		baregl::Framebuffer{"BloomSamplingBuffer6"},
		baregl::Framebuffer{"BloomSamplingBuffer7"},
		baregl::Framebuffer{"BloomSamplingBuffer8"},
		baregl::Framebuffer{"BloomSamplingBuffer9"}
	},
	m_bloomOutputBuffer{ "BloomOutputBuffer" }
{
	// Prepare sampling buffers (for downsampling and upsampling).
	// We don't use a ping-pong buffer here, since we need to keep all the intermediate results.
	for (auto& buffer : m_bloomSamplingBuffers)
	{
		FramebufferUtil::SetupFramebuffer(buffer, kBloomTextureDesc, false, false);
	}

	// Prepare bloom output buffer.
	FramebufferUtil::SetupFramebuffer(m_bloomOutputBuffer, kBloomTextureDesc, false, false);

	auto& shaderManager = OVSERVICE(OvCore::ResourceManagement::ShaderManager);

	m_downsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomDownsampling.ovfx"]);
	m_upsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomUpsampling.ovfx"]);
	m_bloomMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\Bloom.ovfx"]);
	m_blitMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\Blit.ovfx"]);

	// Since we want to use blending during the upsampling pass, we need to set up the material
	// manually. The EBlitFlag::USE_MATERIAL_STATE_MASK will be used to enforce these settings.
	m_upsamplingMaterial.SetDepthWriting(false);
	m_upsamplingMaterial.SetColorWriting(true);
	m_upsamplingMaterial.SetBlendable(true);
	m_upsamplingMaterial.SetFrontfaceCulling(false);
	m_upsamplingMaterial.SetBackfaceCulling(false);
	m_upsamplingMaterial.SetDepthTest(false);
}

bool OvCore::Rendering::PostProcess::BloomEffect::IsApplicable(const EffectSettings& p_settings) const
{
	auto& bloomSettings = static_cast<const BloomSettings&>(p_settings);

	return
		AEffect::IsApplicable(p_settings) &&
		bloomSettings.intensity > 0.0f;
}

void OvCore::Rendering::PostProcess::BloomEffect::Draw(
	OvRendering::Data::PipelineState p_pso,
	baregl::Framebuffer& p_src,
	baregl::Framebuffer& p_dst,
	const EffectSettings& p_settings
)
{
	ZoneScoped;
	TracyGpuZone("BloomEffect");

	using enum OvRendering::Settings::EBlitFlags;

	const auto [refX, refY] = p_src.GetSize();

	OVASSERT(refX > 0 && refY > 0, "Invalid source framebuffer size");

	const auto& bloomSettings = static_cast<const BloomSettings&>(p_settings);
	const auto preferredPassCount = static_cast<uint32_t>(bloomSettings.passes);

	// We want to make sure we don't downsample too much (shouldn't go past 1x1).
	const auto maxPassCountForCurrentResolution = std::max(
		static_cast<uint32_t>(std::log2(refX)),
		static_cast<uint32_t>(std::log2(refY))
	);

	const auto maxPassCount = std::clamp(
		preferredPassCount,
		BloomConstants::kMinPassCount,
		std::min(maxPassCountForCurrentResolution, BloomConstants::kMaxPassCount)
	);

	const auto passCount = std::min(preferredPassCount, maxPassCount);

	std::vector<BloomPass> bloomMips;
	bloomMips.reserve(passCount);

	for (uint32_t i = 0; i < passCount; ++i)
	{
		// (value >> i) is equivalent to (value / 2^i),
		// so we essentially get half the resolution each time.
		const uint16_t width = refX >> (i + 1);
		const uint16_t height = refY >> (i + 1);
		auto& target = m_bloomSamplingBuffers[i];
		target.Resize(width, height);
		bloomMips.emplace_back(width, height, target);
	}

	// First we want to copy the input image to another buffer to avoid modifying the original image.
	// This could also be made into a filtering pass, so we can exclude low luminance pixels.
	m_renderer.Blit(p_pso, p_src, m_bloomOutputBuffer, m_blitMaterial);

	auto downsamplingPass = [&](
		baregl::Framebuffer& p_src,
		baregl::Framebuffer& p_dst,
		const OvMaths::FVector2& p_srcRes 
	) {
		m_downsamplingMaterial.SetProperty("_InputResolution", p_srcRes, true);
		m_renderer.Blit(p_pso, p_src, p_dst, m_downsamplingMaterial, DEFAULT & ~RESIZE_DST_TO_MATCH_SRC);
	};

	// First downsample pass, using karis average filter
	m_downsamplingMaterial.SetFeatures({ "KARIS_AVERAGE" });
	downsamplingPass(m_bloomOutputBuffer, bloomMips[0].target, {
		static_cast<float>(refX),
		static_cast<float>(refY)
	});

	// Subsequent downsample passes don't need the karis average filter
	m_downsamplingMaterial.SetFeatures({});

	// Downsample the remaining passes (passCount - 1)
	for (uint32_t i = 1; i < passCount; ++i)
	{
		auto& src = bloomMips[i - 1];
		auto& dst = bloomMips[i];

		downsamplingPass(src.target, dst.target, {
			static_cast<float>(src.width),
			static_cast<float>(src.height)
		});
	}

	// Custom PSO for the upsampling pass, allowing us to use additive blending (accumulation)
	auto upsamplingPSO = p_pso;
	upsamplingPSO.blendingSrcFactor = baregl::types::EBlendingFactor::ONE;
	upsamplingPSO.blendingDestFactor = baregl::types::EBlendingFactor::ONE;
	upsamplingPSO.blendingEquation = baregl::types::EBlendingEquation::FUNC_ADD;

	m_upsamplingMaterial.SetProperty("_FilterRadius", BloomConstants::kFilterRadius, true);

	auto upsamplingPass = [&](
		baregl::Framebuffer& p_src,
		baregl::Framebuffer& p_dst
	) {
		m_renderer.Blit(upsamplingPSO, p_src, p_dst, m_upsamplingMaterial, (DEFAULT & ~RESIZE_DST_TO_MATCH_SRC) | USE_MATERIAL_STATE_MASK);
	};

	// Blur and upsample back to the original resolution
	for (int32_t i = passCount - 1; i > 0; --i)
	{
		upsamplingPass(bloomMips[i].target, bloomMips[i - 1].target);
	}

	upsamplingPass(bloomMips[0].target, m_bloomOutputBuffer);

	// Final pass, interpolate bloom result with the input image
	const auto bloomTex = m_bloomOutputBuffer.GetAttachment<baregl::Texture>(baregl::types::EFramebufferAttachment::COLOR);
	m_bloomMaterial.SetProperty("_BloomTexture", &bloomTex.value().get());
	m_bloomMaterial.SetProperty("_BloomStrength", std::min(bloomSettings.intensity * 0.04f, 1.0f));
	m_renderer.Blit(p_pso, p_src, p_dst, m_bloomMaterial);
}
