/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/Rendering/PostProcess/TonemappingEffect.h"
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

OvCore::Rendering::PostProcess::TonemappingEffect::TonemappingEffect(OvRendering::Core::CompositeRenderer& p_renderer) : AEffect(p_renderer)
{
	m_tonemappingMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\Tonemapping.ovfx"]);
}

void OvCore::Rendering::PostProcess::TonemappingEffect::Draw(
	OvRendering::Data::PipelineState p_pso,
	OvRendering::HAL::Framebuffer& p_src,
	OvRendering::HAL::Framebuffer& p_dst,
	const EffectSettings& p_settings
)
{
	const auto& tonemappingSettings = static_cast<const TonemappingSettings&>(p_settings);

	// Tonemapping
	m_tonemappingMaterial.Set("_Exposure", tonemappingSettings.exposure, true);
	m_tonemappingMaterial.Set("_Mode", static_cast<int>(tonemappingSettings.mode), true);
	m_tonemappingMaterial.Set("_GammaCorrection", static_cast<int>(tonemappingSettings.gammaCorrection), true);
	m_renderer.Blit(p_pso, p_src, p_dst, m_tonemappingMaterial);
}
