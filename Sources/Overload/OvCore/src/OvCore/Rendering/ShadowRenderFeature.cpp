/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tracy/Tracy.hpp>

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/ShadowRenderFeature.h>
#include <OvDebug/Logger.h>
#include <OvRendering/Features/LightingRenderFeature.h>

constexpr uint8_t kMaxShadowMaps = 1;

OvCore::Rendering::ShadowRenderFeature::ShadowRenderFeature(
	OvRendering::Core::CompositeRenderer& p_renderer,
	OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
) :
	ARenderFeature(p_renderer, p_executionPolicy)
{
}

void OvCore::Rendering::ShadowRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	ZoneScoped;

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
						OVASSERT(light.IsSetupForShadowRendering(), "This light isn't setup for shadow rendering");

						const auto shadowTex = light.shadowBuffer->GetAttachment<OvRendering::HAL::Texture>(
							OvRendering::Settings::EFramebufferAttachment::DEPTH
						);

						if (!material.TrySetProperty("_ShadowMap", &shadowTex.value(), true))
						{
							OVLOG_WARNING("ShadowRenderFeature: Material does not have a _ShadowMap property");
						}

						if (!material.TrySetProperty("_LightSpaceMatrix", light.lightSpaceMatrix.value(), true))
						{
							OVLOG_WARNING("ShadowRenderFeature: Material does not have a _LightSpaceMatrix property");
						}

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

