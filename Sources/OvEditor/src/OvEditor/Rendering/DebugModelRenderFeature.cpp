/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Rendering/DebugModelRenderFeature.h"
#include "OvCore/Rendering/EngineDrawableDescriptor.h"
#include "OvRendering/Core/CompositeRenderer.h"

OvEditor::Rendering::DebugModelRenderFeature::DebugModelRenderFeature(
	OvRendering::Core::CompositeRenderer& p_renderer,
	OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
) :
	ARenderFeature(p_renderer, p_executionPolicy)
{
}

void OvEditor::Rendering::DebugModelRenderFeature::DrawModelWithSingleMaterial(
	OvRendering::Data::PipelineState p_pso,
	OvRendering::Resources::Model& p_model,
	OvRendering::Data::Material& p_material,
	const OvMaths::FMatrix4& p_modelMatrix,
	std::optional<OvMaths::FMatrix4> p_viewMatrixOverride,
	std::optional<OvMaths::FMatrix4> p_projectionMatrixOverride
)
{
	auto stateMask = p_material.GenerateStateMask();

	auto engineDrawableDescriptor = OvCore::Rendering::EngineDrawableDescriptor{
		p_modelMatrix,
		OvMaths::FMatrix4::Identity,
		p_viewMatrixOverride,
		p_projectionMatrixOverride
	};

	for (auto mesh : p_model.GetMeshes())
	{
		OvRendering::Entities::Drawable element;
		element.mesh = *mesh;
		element.material = p_material;
		element.stateMask = stateMask;
		element.AddDescriptor(engineDrawableDescriptor);

		m_renderer.DrawEntity(p_pso, element);
	}
}
