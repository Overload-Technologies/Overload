/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <ranges>

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/FramebufferUtil.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Rendering/DebugModelRenderFeature.h>
#include <OvEditor/Rendering/DebugSceneRenderer.h>
#include <OvEditor/Rendering/PickingRenderPass.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvRendering/HAL/Profiling.h>

namespace
{
	void PreparePickingMaterial(
		const OvCore::ECS::Actor& p_actor,
		OvRendering::Data::Material& p_material,
		const std::string& p_uniformName = "_PickingColor"
	)
	{
		uint32_t actorID = static_cast<uint32_t>(p_actor.GetID());

		auto bytes = reinterpret_cast<uint8_t*>(&actorID);
		auto color = OvMaths::FVector4{ bytes[0] / 255.0f, bytes[1] / 255.0f, bytes[2] / 255.0f, 1.0f };

		// Set the picking color property if it exists
		if (p_material.GetProperty(p_uniformName))
		{
			p_material.SetProperty(p_uniformName, color, true);
		}
	}
}

OvEditor::Rendering::PickingRenderPass::PickingRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer),
	m_actorPickingFramebuffer("ActorPicking")
{
	OvCore::Rendering::FramebufferUtil::SetupFramebuffer(
		m_actorPickingFramebuffer, 1, 1, true, false, false
	);

	/* Light Material */
	m_lightMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Billboard"));
	m_lightMaterial.SetDepthTest(false);

	/* Gizmo Pickable Material */
	m_gizmoPickingMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Gizmo"));
	m_gizmoPickingMaterial.SetGPUInstances(3);
	m_gizmoPickingMaterial.SetProperty("u_IsBall", false);
	m_gizmoPickingMaterial.SetProperty("u_IsPickable", true);
	m_gizmoPickingMaterial.SetDepthTest(true);

	m_reflectionProbeMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("PickingFallback"));
	m_reflectionProbeMaterial.SetDepthTest(false);

	/* Picking Material */
	m_actorPickingFallbackMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("PickingFallback"));
}

OvEditor::Rendering::PickingRenderPass::PickingResult OvEditor::Rendering::PickingRenderPass::ReadbackPickingResult(
	const OvCore::SceneSystem::Scene& p_scene,
	uint32_t p_x,
	uint32_t p_y
)
{
	uint8_t pixel[3];

	m_actorPickingFramebuffer.ReadPixels(
		p_x, p_y, 1, 1,
		OvRendering::Settings::EPixelDataFormat::RGB,
		OvRendering::Settings::EPixelDataType::UNSIGNED_BYTE,
		pixel
	);

	uint32_t actorID = (0 << 24) | (pixel[2] << 16) | (pixel[1] << 8) | (pixel[0] << 0);
	auto actorUnderMouse = p_scene.FindActorByID(actorID);

	if (actorUnderMouse)
	{
		return OvTools::Utils::OptRef(*actorUnderMouse);
	}
	else if (
		pixel[0] == 255 &&
		pixel[1] == 255 &&
		pixel[2] >= 252 &&
		pixel[2] <= 254
		)
	{
		return static_cast<OvEditor::Core::GizmoBehaviour::EDirection>(pixel[2] - 252);
	}

	return std::nullopt;
}

void OvEditor::Rendering::PickingRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	ZoneScoped;
	TracyGpuZone("PickingRenderPass");

	using namespace OvCore::Rendering;

	OVASSERT(m_renderer.HasDescriptor<SceneRenderer::SceneDescriptor>(), "Cannot find SceneDescriptor attached to this renderer");
	OVASSERT(m_renderer.HasDescriptor<DebugSceneRenderer::DebugSceneDescriptor>(), "Cannot find DebugSceneDescriptor attached to this renderer");

	auto& sceneDescriptor = m_renderer.GetDescriptor<SceneRenderer::SceneDescriptor>();
	auto& debugSceneDescriptor = m_renderer.GetDescriptor<DebugSceneRenderer::DebugSceneDescriptor>();
	auto& frameDescriptor = m_renderer.GetFrameDescriptor();
	auto& scene = sceneDescriptor.scene;

	m_actorPickingFramebuffer.Resize(frameDescriptor.renderWidth, frameDescriptor.renderHeight);

	m_actorPickingFramebuffer.Bind();
	
	auto pso = m_renderer.CreatePipelineState();

	m_renderer.Clear(true, true, true);

	DrawPickableModels(pso, scene);
	DrawPickableCameras(pso, scene);
	DrawPickableReflectionProbes(pso, scene);
	DrawPickableLights(pso, scene);

	// Clear depth, gizmos are rendered on top of everything else
	m_renderer.Clear(false, true, false);

	if (debugSceneDescriptor.selectedActor)
	{
		auto& selectedActor = debugSceneDescriptor.selectedActor.value();

		DrawPickableGizmo(
			pso,
			selectedActor.transform.GetWorldPosition(),
			selectedActor.transform.GetWorldRotation(),
			debugSceneDescriptor.gizmoOperation
		);
	}

	m_actorPickingFramebuffer.Unbind();

	if (auto output = frameDescriptor.outputBuffer)
	{
		output.value().Bind();
	}
}

void OvEditor::Rendering::PickingRenderPass::DrawPickableModels(
	OvRendering::Data::PipelineState p_pso,
	OvCore::SceneSystem::Scene& p_scene
)
{
	const auto& filteredDrawables = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneFilteredDrawablesDescriptor>();

	auto drawPickableModels = [&](auto drawables) {
		for (auto& drawable : drawables)
		{
			const std::string pickingPassName = "PICKING_PASS";

			// If the material has picking pass, use it, otherwise use the picking fallback material
			auto& targetMaterial =
				(drawable.material && drawable.material->IsValid() && drawable.material->HasPass(pickingPassName)) ?
				drawable.material.value() :
				m_actorPickingFallbackMaterial;

			const auto& actor = drawable.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDrawableDescriptor>().actor;

			PreparePickingMaterial(actor, targetMaterial);

			// Prioritize using the actual material state mask.
			auto stateMask = 
				drawable.material && drawable.material->IsValid() ?
				drawable.material->GenerateStateMask() :
				targetMaterial.GenerateStateMask();

			OvRendering::Entities::Drawable finalDrawable = drawable;
			finalDrawable.material = targetMaterial;
			finalDrawable.stateMask = stateMask;
			finalDrawable.stateMask.frontfaceCulling = false;
			finalDrawable.stateMask.backfaceCulling = false;
			finalDrawable.pass = pickingPassName;

			m_renderer.DrawEntity(p_pso, finalDrawable);
		}
	};

	drawPickableModels(filteredDrawables.opaques | std::views::values);
	drawPickableModels(filteredDrawables.transparents | std::views::values);
	drawPickableModels(filteredDrawables.ui | std::views::values);
}

void OvEditor::Rendering::PickingRenderPass::DrawPickableCameras(
	OvRendering::Data::PipelineState p_pso,
	OvCore::SceneSystem::Scene& p_scene
)
{
	for (auto camera : p_scene.GetFastAccessComponents().cameras)
	{
		auto& actor = camera->owner;

		if (actor.IsActive())
		{
			PreparePickingMaterial(actor, m_actorPickingFallbackMaterial);
			auto& cameraModel = *EDITOR_CONTEXT(editorResources)->GetModel("Camera");
			auto translation = OvMaths::FMatrix4::Translation(actor.transform.GetWorldPosition());
			auto rotation = OvMaths::FQuaternion::ToMatrix4(actor.transform.GetWorldRotation());
			auto modelMatrix = translation * rotation;

			m_renderer.GetFeature<DebugModelRenderFeature>()
				.DrawModelWithSingleMaterial(p_pso, cameraModel, m_actorPickingFallbackMaterial, modelMatrix);
		}
	}
}

void OvEditor::Rendering::PickingRenderPass::DrawPickableReflectionProbes(OvRendering::Data::PipelineState p_pso, OvCore::SceneSystem::Scene& p_scene)
{
	for (auto reflectionProbe : p_scene.GetFastAccessComponents().reflectionProbes)
	{
		auto& actor = reflectionProbe->owner;

		if (actor.IsActive())
		{
			PreparePickingMaterial(actor, m_reflectionProbeMaterial);
			auto& reflectionProbeModel = *EDITOR_CONTEXT(editorResources)->GetModel("Sphere");
			const auto translation = OvMaths::FMatrix4::Translation(
				actor.transform.GetWorldPosition() +
				reflectionProbe->GetCapturePosition()
			);
			const auto rotation = OvMaths::FQuaternion::ToMatrix4(actor.transform.GetWorldRotation());
			const auto scaling = OvMaths::FMatrix4::Scaling({ 0.5f, 0.5f, 0.5f });
			auto modelMatrix = translation * rotation * scaling;

			m_renderer.GetFeature<DebugModelRenderFeature>()
				.DrawModelWithSingleMaterial(p_pso, reflectionProbeModel, m_reflectionProbeMaterial, modelMatrix);
		}
	}
}

void OvEditor::Rendering::PickingRenderPass::DrawPickableLights(
	OvRendering::Data::PipelineState p_pso,
	OvCore::SceneSystem::Scene& p_scene
)
{
	if (Settings::EditorSettings::LightBillboardScale > 0.001f)
	{
		m_renderer.Clear(false, true, false);

		m_lightMaterial.SetProperty("u_Scale", Settings::EditorSettings::LightBillboardScale * 0.1f);

		for (auto light : p_scene.GetFastAccessComponents().lights)
		{
			auto& actor = light->owner;

			if (actor.IsActive())
			{
				PreparePickingMaterial(actor, m_lightMaterial, "u_Diffuse");
				auto& lightModel = *EDITOR_CONTEXT(editorResources)->GetModel("Vertical_Plane");
				auto modelMatrix = OvMaths::FMatrix4::Translation(actor.transform.GetWorldPosition());

				m_renderer.GetFeature<DebugModelRenderFeature>()
					.DrawModelWithSingleMaterial(p_pso, lightModel, m_lightMaterial, modelMatrix);
			}
		}
	}
}

void OvEditor::Rendering::PickingRenderPass::DrawPickableGizmo(
	OvRendering::Data::PipelineState p_pso,
	const OvMaths::FVector3& p_position,
	const OvMaths::FQuaternion& p_rotation,
	OvEditor::Core::EGizmoOperation p_operation
)
{
	auto modelMatrix =
		OvMaths::FMatrix4::Translation(p_position) *
		OvMaths::FQuaternion::ToMatrix4(OvMaths::FQuaternion::Normalize(p_rotation));

	auto arrowModel = EDITOR_CONTEXT(editorResources)->GetModel("Arrow_Picking");

	m_renderer.GetFeature<DebugModelRenderFeature>()
		.DrawModelWithSingleMaterial(p_pso, *arrowModel, m_gizmoPickingMaterial, modelMatrix);
}
