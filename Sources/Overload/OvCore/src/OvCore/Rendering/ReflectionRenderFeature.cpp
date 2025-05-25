/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>

#include <OvDebug/Logger.h>

#include <OvRendering/Features/LightingRenderFeature.h>

OvCore::Rendering::ReflectionRenderFeature::ReflectionRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer) :
	ARenderFeature(p_renderer)
{
}

void OvCore::Rendering::ReflectionRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	auto& material = p_drawable.material.value();

	// TODO: Check if material is set to receive reflections
	if (!material.HasProperty("_ReflectionProbe"))
	{
		return;
	}

	OVASSERT(m_renderer.HasDescriptor<ReflectionRenderFeature::ReflectionDescriptor>(), "Cannot find ReflectionDescriptor attached to this renderer");

	const auto& reflectionDescriptor = m_renderer.GetDescriptor<ReflectionRenderFeature::ReflectionDescriptor>();

	// Find the probe that is best suited for this drawable.
	auto targetProbe = [&]() -> OvTools::Utils::OptRef<ECS::Components::CReflectionProbe> {
		for (auto& probeReference : reflectionDescriptor.reflectionProbes)
		{
			return probeReference.get();
		}
		return std::nullopt;
	}();

	if (targetProbe.has_value())
	{
		material.TrySetProperty("_ReflectionProbe", targetProbe->GetCubemap().get(), true);
	}
	else
	{
		// This is necessary to "stop" the material from using the previous probe.
		material.TrySetProperty("_ReflectionProbe", static_cast<OvRendering::HAL::Texture*>(nullptr), true);
	}
}

void OvCore::Rendering::ReflectionRenderFeature::OnAfterDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{

}

