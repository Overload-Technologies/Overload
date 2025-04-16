/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Rendering/DebugModelRenderFeature.h"
#include "OvEditor/Rendering/OutlineRenderFeature.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Settings/EditorSettings.h"

#include <OvCore/ECS/Components/CMaterialRenderer.h>

#include <OvRendering/Utils/Conversions.h>

constexpr uint32_t kStencilMask = 0xFF;
constexpr int32_t kStencilReference = 1;

OvEditor::Rendering::OutlineRenderFeature::OutlineRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Features::ARenderFeature(p_renderer)
{
	/* Stencil Fill Material */
	m_stencilFillMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Unlit.ovfx"]);
	m_stencilFillMaterial.SetBackfaceCulling(true);
	m_stencilFillMaterial.SetDepthTest(false);
	m_stencilFillMaterial.SetColorWriting(false);
	m_stencilFillMaterial.SetProperty("u_DiffuseMap", static_cast<OvRendering::Resources::Texture*>(nullptr));

	/* Outline Material */
	m_outlineMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Unlit.ovfx"]);
	m_outlineMaterial.SetProperty("u_DiffuseMap", static_cast<OvRendering::Resources::Texture*>(nullptr));
	m_outlineMaterial.SetDepthTest(false);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawOutline(
	OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector4& p_color,
	float p_thickness
)
{
	DrawStencilPass(p_actor);
	DrawOutlinePass(p_actor, p_color, p_thickness);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawStencilPass(OvCore::ECS::Actor& p_actor)
{
	auto pso = m_renderer.CreatePipelineState();

	pso.stencilTest = true;
	pso.stencilWriteMask = kStencilMask;
	pso.stencilFuncRef = kStencilReference;
	pso.stencilFuncMask = kStencilMask;
	pso.stencilOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.depthOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.bothOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.colorWriting.mask = 0x00;

	DrawActorToStencil(pso, p_actor);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawOutlinePass(OvCore::ECS::Actor& p_actor, const OvMaths::FVector4& p_color, float p_thickness)
{
	auto pso = m_renderer.CreatePipelineState();

	pso.stencilTest = true;
	pso.stencilOpFail = OvRendering::Settings::EOperation::KEEP;
	pso.depthOpFail = OvRendering::Settings::EOperation::KEEP;
	pso.bothOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.stencilFuncOp = OvRendering::Settings::EComparaisonAlgorithm::NOTEQUAL;
	pso.stencilFuncRef = kStencilReference;
	pso.stencilFuncMask = kStencilMask;
	pso.rasterizationMode = OvRendering::Settings::ERasterizationMode::LINE;
	pso.lineWidthPow2 = OvRendering::Utils::Conversions::FloatToPow2(p_thickness);

	// Prepare the outline material
	m_outlineMaterial.SetProperty("u_Diffuse", p_color);

	DrawActorOutline(pso, p_actor);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawActorToStencil(OvRendering::Data::PipelineState p_pso, OvCore::ECS::Actor& p_actor)
{
	if (p_actor.IsActive())
	{
		/* Render static mesh outline and bounding spheres */
		if (auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
		{
			DrawModelToStencil(p_pso, p_actor.transform.GetWorldMatrix(), *modelRenderer->GetModel());
		}

		/* Render camera component outline */
		if (auto cameraComponent = p_actor.GetComponent<OvCore::ECS::Components::CCamera>(); cameraComponent)
		{
			auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			auto model = translation * rotation;
			DrawModelToStencil(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Camera"));
		}

		for (auto& child : p_actor.GetChildren())
		{
			DrawActorToStencil(p_pso, *child);
		}
	}
}

void OvEditor::Rendering::OutlineRenderFeature::DrawActorOutline(OvRendering::Data::PipelineState p_pso, OvCore::ECS::Actor& p_actor)
{
	if (p_actor.IsActive())
	{
		if (auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
		{
			DrawModelOutline(p_pso, p_actor.transform.GetWorldMatrix(), *modelRenderer->GetModel());
		}

		if (auto cameraComponent = p_actor.GetComponent<OvCore::ECS::Components::CCamera>(); cameraComponent)
		{
			auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			auto model = translation * rotation;
			DrawModelOutline(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Camera"));
		}

		for (auto& child : p_actor.GetChildren())
		{
			DrawActorOutline(p_pso, *child);
		}
	}
}

void OvEditor::Rendering::OutlineRenderFeature::DrawModelToStencil(
	OvRendering::Data::PipelineState p_pso,
	const OvMaths::FMatrix4& p_worldMatrix,
	OvRendering::Resources::Model& p_model
)
{
	m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(p_pso, p_model, m_stencilFillMaterial, p_worldMatrix);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawModelOutline(
	OvRendering::Data::PipelineState p_pso,
	const OvMaths::FMatrix4& p_worldMatrix,
	OvRendering::Resources::Model& p_model
)
{
	m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(p_pso, p_model, m_outlineMaterial, p_worldMatrix);
}
