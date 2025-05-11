/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

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
		OvCore::ECS::Actor& p_actor,
		OvCore::Resources::Material& p_material,
		const std::string& p_uniformName = "_PickingColor"
	)
	{
		uint32_t actorID = static_cast<uint32_t>(p_actor.GetID());

		auto bytes = reinterpret_cast<uint8_t*>(&actorID);
		auto color = OvMaths::FVector4{ bytes[0] / 255.0f, bytes[1] / 255.0f, bytes[2] / 255.0f, 1.0f };

		p_material.SetProperty(p_uniformName, color, true);
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
	m_lightMaterial.SetDepthTest(true);

	/* Gizmo Pickable Material */
	m_gizmoPickingMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Gizmo"));
	m_gizmoPickingMaterial.SetGPUInstances(3);
	m_gizmoPickingMaterial.SetProperty("u_IsBall", false);
	m_gizmoPickingMaterial.SetProperty("u_IsPickable", true);

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
	// TODO: Make sure we only renderer when the view is hovered and not being resized

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
	DrawPickableLights(pso, scene);

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
	for (auto modelRenderer : p_scene.GetFastAccessComponents().modelRenderers)
	{
		auto& actor = modelRenderer->owner;

		if (actor.IsActive())
		{
			if (auto model = modelRenderer->GetModel())
			{
				if (auto materialRenderer = modelRenderer->owner.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
				{
					const auto& materials = materialRenderer->GetMaterials();
					const auto& modelMatrix = actor.transform.GetWorldMatrix();

					for (auto mesh : model->GetMeshes())
					{
						const std::string pickingPassName = "PICKING_PASS";

						auto customMaterial = materials.at(mesh->GetMaterialIndex());

						// If the material has picking pass, use it, otherwise use the picking fallback material
						auto& targetMaterial =
							(customMaterial && customMaterial->IsValid() && customMaterial->SupportsFeature(pickingPassName)) ?
							*customMaterial :
							m_actorPickingFallbackMaterial;

						PreparePickingMaterial(actor, targetMaterial);

						// Prioritize using the actual material state mask.
						auto stateMask =
							customMaterial && customMaterial->IsValid() ?
							customMaterial->GenerateStateMask() :
							targetMaterial.GenerateStateMask();

						OvRendering::Entities::Drawable drawable;
						drawable.mesh = *mesh;
						drawable.material = targetMaterial;
						drawable.stateMask = stateMask;
						drawable.stateMask.frontfaceCulling = false;
						drawable.stateMask.backfaceCulling = false;
						drawable.featureSetOverride = { pickingPassName };

						drawable.AddDescriptor<OvCore::Rendering::EngineDrawableDescriptor>({
							modelMatrix
						});

						m_renderer.DrawEntity(p_pso, drawable);
					}
				}
			}
		}
	}
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
