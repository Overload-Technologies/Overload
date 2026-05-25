/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <ranges>
#include <string>

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/ECS/Components/UI/CTransform2D.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Rendering/SkinningDrawableDescriptor.h>
#include <OvCore/Rendering/SkinningUtils.h>
#include <OvCore/Rendering/UIRenderingUtils.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Rendering/DebugModelRenderFeature.h>
#include <OvEditor/Rendering/DebugSceneRenderer.h>
#include <OvEditor/Rendering/PickingRenderPass.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvRendering/HAL/Profiling.h>

namespace
{
	const std::string kPickingPassName = "PICKING_PASS";
	const std::string kSkinningFeatureName = std::string{ OvCore::Rendering::SkinningUtils::kFeatureName };

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

	const OvCore::ECS::Components::UI::CCanvas* FindCanvas(const OvCore::ECS::Actor& p_owner)
	{
		return OvCore::Rendering::UIRenderingUtils::FindCanvas(p_owner);
	}

	OvMaths::FVector2 GetCanvasSize(const OvCore::ECS::Actor& p_owner, const OvMaths::FVector2& p_renderSize)
	{
		return OvCore::Rendering::UIRenderingUtils::GetCanvasSize(p_owner, p_renderSize);
	}

	float GetCanvasScale(const OvCore::ECS::Actor& p_owner, const OvMaths::FVector2& p_renderSize)
	{
		if (const auto* canvas = FindCanvas(p_owner))
		{
			return OvCore::Rendering::UIRenderingUtils::GetCanvasScale(*canvas, p_renderSize);
		}

		return 1.0f;
	}

	OvMaths::FVector2 GetLayoutOffset(const OvCore::ECS::Actor& p_owner)
	{
		return OvCore::Rendering::UIRenderingUtils::GetLayoutOffset(p_owner);
	}

	OvMaths::FVector3 TransformPoint(const OvMaths::FMatrix4& p_matrix, const OvMaths::FVector2& p_point)
	{
		const auto result = p_matrix * OvMaths::FVector4{ p_point.x, p_point.y, 0.0f, 1.0f };
		return { result.x, result.y, result.z };
	}

	OvMaths::FVector2 GetResolvedElementSize(
		const OvCore::ECS::Components::UI::CTransform2D* p_transform2D,
		const OvMaths::FVector2& p_elementSize
	)
	{
		if (!p_transform2D)
		{
			return {
				std::max(p_elementSize.x, 0.0f),
				std::max(p_elementSize.y, 0.0f)
			};
		}

		const auto sizeOverride = p_transform2D->GetSize();
		return {
			sizeOverride.x > 0.0f ? sizeOverride.x : std::max(p_elementSize.x, 0.0f),
			sizeOverride.y > 0.0f ? sizeOverride.y : std::max(p_elementSize.y, 0.0f)
		};
	}

	void ApplyTransform2DSizeOverride(
		OvMaths::FMatrix4& p_matrix,
		const OvCore::ECS::Components::UI::CTransform2D* p_transform2D,
		const OvMaths::FVector2& p_elementSize
	)
	{
		if (!p_transform2D || p_elementSize.x <= 0.0f || p_elementSize.y <= 0.0f)
		{
			return;
		}

		const auto resolvedSize = GetResolvedElementSize(p_transform2D, p_elementSize);
		p_matrix = p_matrix * OvMaths::FMatrix4::Scaling({
			resolvedSize.x / p_elementSize.x,
			resolvedSize.y / p_elementSize.y,
			1.0f
		});
	}

	bool TryGetUIActorGizmoTransform(
		const bool p_includeUI,
		const bool p_renderUIInScreenSpace,
		const uint32_t p_renderWidth,
		const uint32_t p_renderHeight,
		OvCore::ECS::Actor& p_actor,
		OvMaths::FVector3& p_position,
		OvMaths::FQuaternion& p_rotation
	)
	{
		if (!p_includeUI)
		{
			return false;
		}

		auto* transform2D = p_actor.GetComponent<OvCore::ECS::Components::UI::CTransform2D>();
		const auto* canvas = transform2D ? FindCanvas(p_actor) : nullptr;
		if (!transform2D || !canvas)
		{
			return false;
		}

		OvMaths::FVector2 elementSize = OvMaths::FVector2::Zero;
		if (auto* image = p_actor.GetComponent<OvCore::ECS::Components::UI::CImage>())
		{
			elementSize = image->GetSize();
		}
		else if (auto* text = p_actor.GetComponent<OvCore::ECS::Components::UI::CText>())
		{
			elementSize = text->GetSize();
		}
		else if (auto* canvasComponent = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			elementSize = OvCore::Rendering::UIRenderingUtils::GetCanvasSize(*canvasComponent, {
				static_cast<float>(p_renderWidth),
				static_cast<float>(p_renderHeight)
			});
		}
		else
		{
			elementSize = transform2D->GetSize();
		}

		const auto renderSize = OvMaths::FVector2{
			static_cast<float>(p_renderWidth),
			static_cast<float>(p_renderHeight)
		};
		const auto canvasSize = GetCanvasSize(p_actor, renderSize);
		const auto layoutOffset = GetLayoutOffset(p_actor);
		const auto canvasScale = GetCanvasScale(p_actor, renderSize);
		auto matrix = transform2D->GetMatrix(canvasSize, layoutOffset, elementSize);
		ApplyTransform2DSizeOverride(matrix, transform2D, elementSize);

		const auto worldScale = OvCore::Rendering::UIRenderingUtils::GetUIWorldScale(*canvas, p_renderUIInScreenSpace);
		const auto unitsScale = p_renderUIInScreenSpace ? canvasScale : canvasScale * worldScale;
		matrix = OvMaths::FMatrix4::Scaling({ unitsScale, unitsScale, 1.0f }) * matrix;

		p_position = TransformPoint(matrix, OvMaths::FVector2::Zero);
		p_rotation = p_actor.transform.GetWorldRotation();
		return true;
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
		auto gizmoPosition = selectedActor.transform.GetWorldPosition();
		auto gizmoRotation = selectedActor.transform.GetWorldRotation();
		TryGetUIActorGizmoTransform(
			sceneDescriptor.includeUI,
			sceneDescriptor.renderUIInScreenSpace,
			frameDescriptor.renderWidth,
			frameDescriptor.renderHeight,
			selectedActor,
			gizmoPosition,
			gizmoRotation
		);

		DrawPickableGizmo(
			pso,
			gizmoPosition,
			gizmoRotation,
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
			const auto& actor = drawable.template GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDrawableDescriptor>().actor;
			const auto skinnedRenderer = actor.template GetComponent<OvCore::ECS::Components::CSkinnedMeshRenderer>();
			const bool hasSkinningDescriptor = drawable.template HasDescriptor<OvCore::Rendering::SkinningDrawableDescriptor>();
			const bool skinningEnabled = hasSkinningDescriptor &&
				skinnedRenderer &&
				m_actorPickingFallbackMaterial.SupportsFeature(kSkinningFeatureName);

			if (skinningEnabled)
			{
				auto& targetMaterial = m_actorPickingFallbackMaterial;

				PreparePickingMaterial(actor, targetMaterial);

				OvRendering::Entities::Drawable finalDrawable = drawable;
				finalDrawable.material = &targetMaterial;
				finalDrawable.stateMask = targetMaterial.GenerateStateMask();
				finalDrawable.stateMask.frontfaceCulling = false;
				finalDrawable.stateMask.backfaceCulling = false;
				finalDrawable.pass = kPickingPassName;

				OvCore::Rendering::SkinningUtils::ApplyToDrawable(finalDrawable, *skinnedRenderer, &targetMaterial.GetFeatures());
				m_renderer.DrawEntity(p_pso, finalDrawable);
				continue;
			}

			auto& targetMaterial =
				drawable.material &&
				drawable.material->IsValid() &&
				drawable.material->HasPass(kPickingPassName) ?
				drawable.material.value() :
				m_actorPickingFallbackMaterial;

			PreparePickingMaterial(actor, targetMaterial);

			OvRendering::Entities::Drawable finalDrawable = drawable;
			finalDrawable.material = &targetMaterial;
			finalDrawable.stateMask = targetMaterial.GenerateStateMask();
			finalDrawable.stateMask.frontfaceCulling = false;
			finalDrawable.stateMask.backfaceCulling = false;
			finalDrawable.pass = kPickingPassName;
			finalDrawable.featureSetOverride = std::nullopt;

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
			const auto scaling = OvMaths::FMatrix4::Scaling(
				OvMaths::FVector3::One * OvEditor::Settings::EditorSettings::ReflectionProbeScale
			);
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
