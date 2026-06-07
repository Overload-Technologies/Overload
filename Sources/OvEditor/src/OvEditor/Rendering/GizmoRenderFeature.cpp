/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CSpotLight.h>

#include <OvRendering/Features/DebugShapeRenderFeature.h>

#include <OvDebug/Assertion.h>

#include "OvEditor/Rendering/DebugModelRenderFeature.h"
#include "OvEditor/Rendering/DebugSceneRenderer.h"
#include "OvEditor/Core/EditorResources.h"
#include "OvEditor/Panels/AView.h"
#include "OvEditor/Settings/EditorSettings.h"
#include "OvEditor/Core/GizmoBehaviour.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Rendering/GizmoRenderFeature.h"

namespace
{
	constexpr float kDistanceBasedGizmoScale = -1.0f;
	constexpr const char* kGizmoScaleUniform = "u_GizmoScale";
}

OvEditor::Rendering::GizmoRenderFeature::GizmoRenderFeature(
	OvRendering::Core::CompositeRenderer& p_renderer,
	OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
) :
	OvRendering::Features::ARenderFeature(p_renderer, p_executionPolicy)
{
	/* Gizmo Arrow Material */
	m_gizmoArrowMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Gizmo"));
	m_gizmoArrowMaterial.SetGPUInstances(3);
	m_gizmoArrowMaterial.SetProperty("u_IsBall", false);
	m_gizmoArrowMaterial.SetProperty("u_IsPickable", false);
	m_gizmoArrowMaterial.TrySetProperty(kGizmoScaleUniform, kDistanceBasedGizmoScale);

	/* Gizmo Ball Material */
	m_gizmoBallMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Gizmo"));
	m_gizmoBallMaterial.SetProperty("u_IsBall", true);
	m_gizmoBallMaterial.SetProperty("u_IsPickable", false);
	m_gizmoBallMaterial.TrySetProperty(kGizmoScaleUniform, kDistanceBasedGizmoScale);
}

std::string GetArrowModelName(OvEditor::Core::EGizmoOperation p_operation)
{
	using namespace OvEditor::Core;

	switch (p_operation)
	{
		default:
		case EGizmoOperation::TRANSLATE: return "Arrow_Translate";
		case EGizmoOperation::ROTATE: return "Arrow_Rotate";
		case EGizmoOperation::SCALE: return "Arrow_Scale";
	}
}

int GetAxisIndexFromDirection(std::optional<OvEditor::Core::GizmoBehaviour::EDirection> p_direction)
{
	return p_direction ? static_cast<int>(p_direction.value()) : -1;
}

void OvEditor::Rendering::GizmoRenderFeature::DrawGizmo(
	const OvMaths::FVector3& p_position,
	const OvMaths::FQuaternion& p_rotation,
	OvEditor::Core::EGizmoOperation p_operation,
	bool p_pickable,
	std::optional<OvEditor::Core::GizmoBehaviour::EDirection> p_highlightedDirection,
	std::optional<OvMaths::FMatrix4> p_viewMatrixOverride,
	std::optional<OvMaths::FMatrix4> p_projectionMatrixOverride,
	std::optional<float> p_scaleOverride
)
{
	auto pso = m_renderer.CreatePipelineState();
	const float gizmoScale = p_scaleOverride.value_or(kDistanceBasedGizmoScale);
	m_gizmoBallMaterial.TrySetProperty(kGizmoScaleUniform, gizmoScale);
	m_gizmoArrowMaterial.TrySetProperty(kGizmoScaleUniform, gizmoScale);

	auto modelMatrix =
		OvMaths::FMatrix4::Translation(p_position) *
		OvMaths::FQuaternion::ToMatrix4(OvMaths::FQuaternion::Normalize(p_rotation));

	if (auto sphereModel = EDITOR_CONTEXT(editorResources)->GetModel("Sphere"))
	{
		auto sphereModelMatrix = modelMatrix * OvMaths::FMatrix4::Scaling({ 0.1f, 0.1f, 0.1f });

		m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(
			pso,
			*sphereModel,
			m_gizmoBallMaterial,
			sphereModelMatrix,
			p_viewMatrixOverride,
			p_projectionMatrixOverride
		);
	}
	
	auto arrowModelName = GetArrowModelName(p_operation);

	if (auto arrowModel = EDITOR_CONTEXT(editorResources)->GetModel(arrowModelName))
	{
		const auto axisIndex = GetAxisIndexFromDirection(p_highlightedDirection);
		m_gizmoArrowMaterial.SetProperty("u_HighlightedAxis", axisIndex);

		m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(
			pso,
			*arrowModel,
			m_gizmoArrowMaterial,
			modelMatrix,
			p_viewMatrixOverride,
			p_projectionMatrixOverride
		);
	}
}
