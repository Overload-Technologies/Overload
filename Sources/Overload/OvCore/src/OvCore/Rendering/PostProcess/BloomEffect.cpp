/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Rendering/PostProcess/BloomEffect.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/HAL/Profiling.h>

// Implementation reference: https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

OvCore::Rendering::PostProcess::BloomEffect::BloomEffect(OvRendering::Core::CompositeRenderer& p_renderer) :
	AEffect(p_renderer),
	m_bloomPingPong{ "Bloom" }
{
	for (auto& buffer : m_bloomPingPong.GetFramebuffers())
	{
		FramebufferUtil::SetupFramebuffer(
			buffer, 1, 1, false, false, false
		);
	}

	auto& shaderManager = OVSERVICE(OvCore::ResourceManagement::ShaderManager);

	m_downsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomDownsampling.ovfx"]);
	m_upsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomUpsampling.ovfx"]);
	m_bloomMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\Bloom.ovfx"]);

	// Since we want to use blending during the downsampling pass, we need to set up the material
	// manually. The EBlitFlag::USE_MATERIAL_STATE_MASK will be used to enforce these settings.
	m_downsamplingMaterial.SetDepthWriting(false);
	m_downsamplingMaterial.SetColorWriting(true);
	m_downsamplingMaterial.SetBlendable(true);
	m_downsamplingMaterial.SetFrontfaceCulling(false);
	m_downsamplingMaterial.SetBackfaceCulling(false);
	m_downsamplingMaterial.SetDepthTest(false);
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
	OvRendering::HAL::Framebuffer& p_src,
	OvRendering::HAL::Framebuffer& p_dst,
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
	const auto maxPassCount = static_cast<uint32_t>(std::log2(std::min(refX, refY)));
	const auto passCount = std::min(preferredPassCount, maxPassCount);

	// Build the resolution chain.
	std::vector<std::pair<uint16_t, uint16_t>> resolutions;
	for (uint32_t i = 0; i < passCount + 1; ++i)
	{
		resolutions.emplace_back(refX >> i, refY >> i); // >> i is equivalent to dividing by 2^i
	}

	// Clear the ping-pong buffers, since we use additive blending
	for (auto& buffer : m_bloomPingPong.GetFramebuffers())
	{
		buffer.Bind();
		m_renderer.Clear(true, true, true);
		buffer.Unbind();
	}

	// Custom PSO for the downsampling pass, allowing us to use additive blending (accumulation)
	auto downsamplingPSO = p_pso;
	downsamplingPSO.blendingSrcFactor = OvRendering::Settings::EBlendingFactor::ONE;
	downsamplingPSO.blendingDestFactor = OvRendering::Settings::EBlendingFactor::ONE;
	downsamplingPSO.blendingEquation = OvRendering::Settings::EBlendingEquation::FUNC_ADD;

	m_downsamplingMaterial.SetBlendable(bloomSettings.intensity > 2.0f);

	// Downsample the input n times.
	for (uint32_t i = 0; i < passCount; ++i)
	{
		const bool isFirstPass = i == 0;
		auto& src = isFirstPass ? p_src : m_bloomPingPong[0];
		auto& dst = m_bloomPingPong[1];

		m_downsamplingMaterial.SetFeatures(
			isFirstPass ?
			OvRendering::Data::FeatureSet{ "KARIS_AVERAGE" } :
			OvRendering::Data::FeatureSet{}
		);

		// Provide source size to the shader
		m_downsamplingMaterial.SetProperty("_InputResolution", OvMaths::FVector2{
			static_cast<float>(resolutions[i].first),
			static_cast<float>(resolutions[i].second)
		}, true);

		// Destination size (downsampled)
		dst.Resize(
			resolutions[i + 1].first,
			resolutions[i + 1].second
		);
		
		m_renderer.Blit(downsamplingPSO, src, dst, m_downsamplingMaterial, (DEFAULT & ~RESIZE_DST_TO_MATCH_SRC) | USE_MATERIAL_STATE_MASK);

		++m_bloomPingPong;
	}

	// Blur and upsample back to the original resolution
	for (uint32_t i = 0; i < passCount; ++i)
	{
		auto& src = m_bloomPingPong[0];
		auto& dst = m_bloomPingPong[1];

		m_upsamplingMaterial.SetProperty("_FilterRadius", bloomSettings.radius, true);

		// Destination size (upsampled)
		const uint32_t resolutionIndex = passCount - 1 - i;

		dst.Resize(
			resolutions[resolutionIndex].first,
			resolutions[resolutionIndex].second
		);

		m_renderer.Blit(p_pso, src, dst, m_upsamplingMaterial, DEFAULT & ~RESIZE_DST_TO_MATCH_SRC);

		++m_bloomPingPong;
	}

	// Final pass, interpolate bloom result with the input image
	const auto bloomTex = m_bloomPingPong[0].GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::COLOR);
	m_bloomMaterial.SetProperty("_BloomTexture", &bloomTex.value());
	m_bloomMaterial.SetProperty("_BloomStrength", std::min(bloomSettings.intensity * 0.04f, 1.0f));
	m_renderer.Blit(p_pso, p_src, p_dst, m_bloomMaterial);
}
