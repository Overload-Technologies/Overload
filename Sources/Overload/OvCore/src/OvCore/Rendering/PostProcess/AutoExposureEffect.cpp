/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/PostProcess/AutoExposureEffect.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/HAL/Profiling.h>

constexpr uint32_t kLuminanceBufferResolution = 1024;
constexpr uint32_t kExposureBufferResolution = 1;

OvCore::Rendering::PostProcess::AutoExposureEffect::AutoExposureEffect(
	OvRendering::Core::CompositeRenderer& p_renderer
) :	AEffect(p_renderer),
	m_luminanceBuffer{ "Luminance" },
	m_exposurePingPongBuffer{ "Exposure" }
{
	for (auto& buffer : m_exposurePingPongBuffer.GetFramebuffers())
	{
		FramebufferUtil::SetupFramebuffer(
			buffer,
			kExposureBufferResolution,
			kExposureBufferResolution,
			false, false, false
		);
	}

	FramebufferUtil::SetupFramebuffer(
		m_luminanceBuffer,
		kLuminanceBufferResolution,
		kLuminanceBufferResolution,
		false, false,
		true // <-- use mipmaps
	);

	m_luminanceMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\Luminance.ovfx"]);
	m_exposureMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\AutoExposure.ovfx"]);
	m_compensationMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\ApplyExposure.ovfx"]);
}

void OvCore::Rendering::PostProcess::AutoExposureEffect::Draw(
	OvRendering::Data::PipelineState p_pso,
	OvRendering::HAL::Framebuffer& p_src,
	OvRendering::HAL::Framebuffer& p_dst,
	const EffectSettings& p_settings
)
{
	ZoneScoped;
	TracyGpuZone("AutoExposureEffect");

	const auto& autoExposureSettings = static_cast<const AutoExposureSettings&>(p_settings);

	// Luminance calculation
	m_luminanceBuffer.Resize(kLuminanceBufferResolution, kLuminanceBufferResolution);
	m_luminanceMaterial.SetProperty("_CenterWeightBias", autoExposureSettings.centerWeightBias, true);
	m_renderer.Blit(p_pso, p_src, m_luminanceBuffer, m_luminanceMaterial,
		OvRendering::Settings::EBlitFlags::DEFAULT & ~OvRendering::Settings::EBlitFlags::RESIZE_DST_TO_MATCH_SRC);
	const auto luminanceTex = m_luminanceBuffer.GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::COLOR);
	luminanceTex->GenerateMipmaps();

	float elapsedTime = 1.0f;
	auto currentTime = std::chrono::high_resolution_clock::now();
	if (m_previousTime.has_value())
	{
		elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(
			currentTime - m_previousTime.value()
		).count();
	}
	m_previousTime = currentTime;

	auto& previousExposure = m_exposurePingPongBuffer[0];
	auto& currentExposure = m_exposurePingPongBuffer[1];
	++m_exposurePingPongBuffer;

	// Exposure adaptation
	m_exposureMaterial.SetProperty("_LuminanceTexture", &luminanceTex.value(), true);
	m_exposureMaterial.SetProperty("_MinLuminanceEV", autoExposureSettings.minLuminanceEV, true);
	m_exposureMaterial.SetProperty("_MaxLuminanceEV", autoExposureSettings.maxLuminanceEV, true);
	m_exposureMaterial.SetProperty("_ExposureCompensationEV", autoExposureSettings.exposureCompensationEV, true);
	m_exposureMaterial.SetProperty("_ElapsedTime", elapsedTime, true);
	m_exposureMaterial.SetProperty("_Progressive", static_cast<int>(autoExposureSettings.progressive && !m_firstFrame), true);
	m_exposureMaterial.SetProperty("_SpeedUp", autoExposureSettings.speedUp, true);
	m_exposureMaterial.SetProperty("_SpeedDown", autoExposureSettings.speedDown, true);
	m_renderer.Blit(p_pso, previousExposure, currentExposure, m_exposureMaterial);

	// Apply the exposure to the final image
	const auto exposureTex = currentExposure.GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::COLOR);
	m_compensationMaterial.SetProperty("_ExposureTexture", &exposureTex.value(), true);
	m_renderer.Blit(p_pso, p_src, p_dst, m_compensationMaterial);

	m_firstFrame = false;
}
