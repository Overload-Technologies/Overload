/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/ShadowRenderFeature.h>
#include <OvRendering/Features/LightingRenderFeature.h>

constexpr uint8_t kMaxShadowMaps = 1;

OvCore::Rendering::ShadowRenderFeature::ShadowRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer) :
	ARenderFeature(p_renderer)
{
}

void OvCore::Rendering::ShadowRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	auto& material = p_drawable.material.value();

	if (material.IsShadowReceiver())
	{
		OVASSERT(m_renderer.HasDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>(), "Cannot find LightingDescriptor attached to this renderer");

		auto& lightDescriptor = m_renderer.GetDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>();

		uint8_t lightIndex = 0;

		for (auto lightReference : lightDescriptor.lights)
		{
			const auto& light = lightReference.get();

			if (light.castShadows)
			{
				if (lightIndex < kMaxShadowMaps)
				{
					if (light.type == OvRendering::Settings::ELightType::DIRECTIONAL)
					{
						const auto shadowTex = light.GetShadowBuffer().GetAttachment<OvRendering::HAL::Texture>(OvRendering::Settings::EFramebufferAttachment::DEPTH);
						material.Set("_ShadowMap", shadowTex, true); // Single use material property
						material.Set("_LightSpaceMatrix", light.GetLightSpaceMatrix());
						++lightIndex;
					}
				}
				else
				{
					OVLOG_WARNING("ShadowRenderFeature does not support more than one shadow casting directional light at the moment");
				}
			}
		}
	}
}

void OvCore::Rendering::ShadowRenderFeature::OnAfterDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{

}

