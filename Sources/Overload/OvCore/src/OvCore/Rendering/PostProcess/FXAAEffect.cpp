/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Rendering/PostProcess/FXAAEffect.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

OvCore::Rendering::PostProcess::FXAAEffect::FXAAEffect(OvRendering::Core::CompositeRenderer& p_renderer) : AEffect(p_renderer)
{
	m_material.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\FXAA.ovfx"]);
}

void OvCore::Rendering::PostProcess::FXAAEffect::Draw(
	OvRendering::Data::PipelineState p_pso,
	OvRendering::HAL::Framebuffer& p_src,
	OvRendering::HAL::Framebuffer& p_dst,
	const EffectSettings& p_settings
)
{
	m_renderer.Blit(p_pso, p_src, p_dst, m_material);
}
