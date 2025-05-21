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
	m_bloomPingPong{
		OvRendering::HAL::Framebuffer{"BloomPingPong0"},
		OvRendering::HAL::Framebuffer{"BloomPingPong1"}
	}
{
	for (auto& buffer : m_bloomPingPong)
	{
		FramebufferUtil::SetupFramebuffer(
			buffer, 1, 1, false, false, false
		);
	}

	auto& shaderManager = OVSERVICE(OvCore::ResourceManagement::ShaderManager);

	m_downsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomDownsampling.ovfx"]);
	m_upsamplingMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\BloomUpsampling.ovfx"]);
	m_bloomMaterial.SetShader(shaderManager[":Shaders\\PostProcess\\Bloom.ovfx"]);
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

	const auto& bloomSettings = static_cast<const BloomSettings&>(p_settings);

	const auto passCount = static_cast<uint32_t>(bloomSettings.passes);

	// Build the resolution chain.
	const auto [refX, refY] = p_src.GetSize();
	std::vector<std::pair<uint16_t, uint16_t>> resolutions{ p_src.GetSize() };
	resolutions.reserve(resolutions.size() + passCount);
	for (uint32_t i = 0; i < passCount; ++i)
	{
		const uint32_t factor = 2ULL << i;
		resolutions.emplace_back(refX / factor, refY / factor);
	}

	uint32_t pingPongIndex = 0;

	p_pso.blending = true;
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	OVASSERT(resolutions.size() == passCount + 1, "Resolution list should match the pass count size + 1");

	// Downsample the input n times.
	for (uint32_t i = 0; i < passCount; ++i)
	{
		const bool isFirstPass = i == 0;
		auto& src = isFirstPass ? p_src : m_bloomPingPong[pingPongIndex % 2];
		auto& dst = m_bloomPingPong[(pingPongIndex + 1) % 2];

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
		
		m_renderer.Blit(
			p_pso,
			src,
			dst,
			m_downsamplingMaterial,
			OvRendering::Settings::EBlitFlags::DEFAULT & ~OvRendering::Settings::EBlitFlags::RESIZE_DST_TO_MATCH_SRC
		);

		++pingPongIndex;
	}

	p_pso.blending = false;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Blur and upsample back to the original resolution
	for (uint32_t i = 0; i < passCount; ++i)
	{
		auto& src = m_bloomPingPong[pingPongIndex % 2];
		auto& dst = m_bloomPingPong[(pingPongIndex + 1) % 2];

		m_upsamplingMaterial.SetProperty("_FilterRadius", bloomSettings.radius, true);

		// Destination size (upsampled)
		const uint32_t resolutionIndex = passCount - 1 - i;
		dst.Resize(
			resolutions[resolutionIndex].first,
			resolutions[resolutionIndex].second
		);

		m_renderer.Blit(
			p_pso,
			src,
			dst,
			m_upsamplingMaterial,
			OvRendering::Settings::EBlitFlags::DEFAULT & ~OvRendering::Settings::EBlitFlags::RESIZE_DST_TO_MATCH_SRC
		);

		++pingPongIndex;
	}

	// Final pass, interpolate bloom result with the original frame
	const auto bloomTex = m_bloomPingPong[pingPongIndex % 2].GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::COLOR);
	m_bloomMaterial.SetProperty("_BloomTexture", &bloomTex.value());
	m_bloomMaterial.SetProperty("_BloomStrength", bloomSettings.intensity * 0.04f);
	m_renderer.Blit(p_pso, p_src, p_dst, m_bloomMaterial);
}
