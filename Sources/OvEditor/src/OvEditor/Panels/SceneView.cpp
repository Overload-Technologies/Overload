/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <optional>

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Rendering/UIRenderingUtils.h>

#include <OvEditor/Rendering/DebugSceneRenderer.h>
#include <OvEditor/Rendering/PickingRenderPass.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/SceneView.h>
#include <OvEditor/Panels/GameView.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvUI/Plugins/DDTarget.h>

namespace
{
	struct UIGizmoContext
	{
		OvMaths::FVector3 origin = OvMaths::FVector3::Zero;
		float unitsScale = 1.0f;
	};

	OvMaths::FVector2 GetResolvedElementSize(
		const OvCore::ECS::Components::CTransform& p_transform,
		const OvMaths::FVector2& p_elementSize
	)
	{
		return p_transform.GetUIEffectiveSize(p_elementSize);
	}

	OvTools::Utils::OptRef<OvCore::ECS::Actor> GetActorFromPickingResult(
		OvEditor::Rendering::PickingRenderPass::PickingResult p_result
	)
	{
		if (p_result)
		{
			if (const auto actor = std::get_if<OvTools::Utils::OptRef<OvCore::ECS::Actor>>(&p_result.value()))
			{
				return *actor;
			}
		}

		return std::nullopt;
	}

	OvMaths::FVector3 TransformPoint(const OvMaths::FMatrix4& p_matrix, const OvMaths::FVector2& p_point)
	{
		const auto result = p_matrix * OvMaths::FVector4{ p_point.x, p_point.y, 0.0f, 1.0f };
		return { result.x, result.y, result.z };
	}

	std::optional<UIGizmoContext> ResolveUIGizmoContext(
		OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_renderUIInScreenSpace
	)
	{
		auto& transform = p_actor.transform;
		if (!transform.HasUIData())
		{
			return std::nullopt;
		}

		const auto* canvas = OvCore::Rendering::UIRenderingUtils::FindCanvas(p_actor);
		if (!canvas)
		{
			return std::nullopt;
		}

		OvMaths::FVector2 elementSize = OvMaths::FVector2::Zero;
		if (const auto* image = p_actor.GetComponent<OvCore::ECS::Components::UI::CImage>())
		{
			elementSize = image->GetSize();
		}
		else if (const auto* text = p_actor.GetComponent<OvCore::ECS::Components::UI::CText>())
		{
			elementSize = text->GetSize();
		}
		else if (const auto* canvasComponent = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			elementSize = OvCore::Rendering::UIRenderingUtils::GetCanvasSize(*canvasComponent, p_renderSize);
		}
		else
		{
			elementSize = transform.GetUISize();
		}

		const auto canvasSize = OvCore::Rendering::UIRenderingUtils::GetCanvasSize(p_actor, p_renderSize);
		const auto layoutOffset = OvCore::Rendering::UIRenderingUtils::GetLayoutOffset(p_actor);
		const auto canvasScale = OvCore::Rendering::UIRenderingUtils::GetCanvasScale(*canvas, p_renderSize);
		const auto worldScale = OvCore::Rendering::UIRenderingUtils::GetUIWorldScale(*canvas, p_renderUIInScreenSpace);
		const auto unitsScale = p_renderUIInScreenSpace ? canvasScale : canvasScale * worldScale;

		auto matrix = transform.GetUIMatrix(canvasSize, layoutOffset, elementSize);
		if (elementSize.x > 0.0f && elementSize.y > 0.0f)
		{
			const auto resolvedSize = GetResolvedElementSize(transform, elementSize);

			matrix = matrix * OvMaths::FMatrix4::Scaling({
				resolvedSize.x / elementSize.x,
				resolvedSize.y / elementSize.y,
				1.0f
			});
		}
		matrix = OvMaths::FMatrix4::Scaling({ unitsScale, unitsScale, 1.0f }) * matrix;

		return UIGizmoContext{
			TransformPoint(matrix, OvMaths::FVector2::Zero),
			unitsScale
		};
	}
}

OvEditor::Panels::SceneView::SceneView
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : AViewControllable(p_title, p_opened, p_windowSettings),
	m_sceneManager(EDITOR_CONTEXT(sceneManager))
{
	m_renderer = std::make_unique<OvEditor::Rendering::DebugSceneRenderer>(*EDITOR_CONTEXT(driver));

	m_camera.SetFar(5000.0f);

	m_fallbackMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Unlit.ovfx"]);
	m_fallbackMaterial.SetProperty("u_Diffuse", OvMaths::FVector4{ 1.f, 0.f, 1.f, 1.0f });
	m_fallbackMaterial.SetProperty("u_DiffuseMap", static_cast<OvRendering::Resources::Texture*>(nullptr));

	m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	{
		const std::string path = p_data.first;

		using namespace OvTools::Utils;
		using enum PathParser::EFileType;

		switch (PathParser::GetFileType(path))
		{
			case SCENE: OnSceneDropped(path); break;
			case MODEL: OnModelDropped(path); break;
			case MATERIAL: OnMaterialDropped(path); break;
			case PREFAB: OnPrefabDropped(path); break;
			default: break;
		}
	};
	
	OvCore::ECS::Actor::DestroyedEvent += [this](const OvCore::ECS::Actor& actor)
	{
		if (m_highlightedActor.has_value() && m_highlightedActor.value().GetID() == actor.GetID())
		{
			m_highlightedActor.reset();
		}
	};
}

void OvEditor::Panels::SceneView::Update(float p_deltaTime)
{
	AViewControllable::Update(p_deltaTime);

	using namespace OvWindowing::Inputs;

	if (IsFocused() && !m_cameraController.IsRightMousePressed())
	{
		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_W))
		{
			SetGizmoOperation(Core::EGizmoOperation::TRANSLATE);
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_E))
		{
			SetGizmoOperation(Core::EGizmoOperation::ROTATE);
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_R))
		{
			SetGizmoOperation(Core::EGizmoOperation::SCALE);
		}
	}
}

void OvEditor::Panels::SceneView::InitFrame()
{
	AViewControllable::InitFrame();

	OvTools::Utils::OptRef<OvCore::ECS::Actor> selectedActor;

	if (EDITOR_EXEC(IsAnyActorSelected()))
	{
		selectedActor = EDITOR_EXEC(GetSelectedActor());
	}

	m_renderer->AddDescriptor<Rendering::DebugSceneRenderer::DebugSceneDescriptor>({
		m_currentOperation,
		m_highlightedActor,
		selectedActor,
		m_highlightedGizmoDirection
	});

	auto& pickingPass = m_renderer->GetPass<OvEditor::Rendering::PickingRenderPass>("Picking");

	// Enable picking pass only when the scene view is hovered, not picking, and not operating the camera
	pickingPass.SetEnabled(
		IsHovered() &&
		!m_gizmoOperations.IsPicking() &&
		!m_cameraController.IsOperating()
	);
}

OvCore::SceneSystem::Scene* OvEditor::Panels::SceneView::GetScene()
{
	return m_sceneManager.GetCurrentScene();
}

void OvEditor::Panels::SceneView::SetGizmoOperation(OvEditor::Core::EGizmoOperation p_operation)
{
	m_currentOperation = p_operation;
	EDITOR_EVENT(EditorOperationChanged).Invoke(m_currentOperation);
}

OvEditor::Core::EGizmoOperation OvEditor::Panels::SceneView::GetGizmoOperation() const
{
	return m_currentOperation;
}

OvCore::Rendering::SceneRenderer::SceneDescriptor OvEditor::Panels::SceneView::CreateSceneDescriptor()
{
	auto descriptor = AViewControllable::CreateSceneDescriptor();
	descriptor.fallbackMaterial = m_fallbackMaterial;
	descriptor.includeUI = true;
	descriptor.renderUIInScreenSpace = EDITOR_EXEC(IsSceneUIRenderingEnabled());

	if (Settings::EditorSettings::DebugFrustumCulling)
	{
		auto& scene = *GetScene();

		if (auto mainCameraComponent = scene.FindMainCamera())
		{
			auto& sceneCamera = mainCameraComponent->GetCamera();
			m_camera.SetFrustumGeometryCulling(sceneCamera.HasFrustumGeometryCulling());
			m_camera.SetFrustumLightCulling(sceneCamera.HasFrustumLightCulling());
			descriptor.frustumOverride = sceneCamera.GetFrustum();
		}
	}

	return descriptor;
}

void OvEditor::Panels::SceneView::DrawFrame()
{
	OvEditor::Panels::AViewControllable::DrawFrame();
	HandleActorPicking();
}

bool IsResizing()
{
	auto cursor = ImGui::GetMouseCursor();

	return
		cursor == ImGuiMouseCursor_ResizeEW ||
		cursor == ImGuiMouseCursor_ResizeNS ||
		cursor == ImGuiMouseCursor_ResizeNWSE ||
		cursor == ImGuiMouseCursor_ResizeNESW ||
		cursor == ImGuiMouseCursor_ResizeAll;
}

void OvEditor::Panels::SceneView::HandleActorPicking()
{
	using namespace OvWindowing::Inputs;

	auto& inputManager = *EDITOR_CONTEXT(inputManager);

	if (inputManager.IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_LEFT))
	{
		m_gizmoOperations.StopPicking();
	}

	if (!m_gizmoOperations.IsPicking() && IsHovered() && !IsResizing())
	{
		const auto pickingResult = GetPickingResult();

		m_highlightedActor = {};
		m_highlightedGizmoDirection = {};

		if (!m_cameraController.IsRightMousePressed() && pickingResult.has_value())
		{
			if (const auto pval = std::get_if<OvTools::Utils::OptRef<OvCore::ECS::Actor>>(&pickingResult.value()))
			{
				m_highlightedActor = *pval;
			}
			else if (const auto pval = std::get_if<OvEditor::Core::GizmoBehaviour::EDirection>(&pickingResult.value()))
			{
				m_highlightedGizmoDirection = *pval;
			}
		}
		else
		{
			m_highlightedActor = {};
			m_highlightedGizmoDirection = {};
		}

		if (inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_LEFT) && !m_cameraController.IsRightMousePressed())
		{
			if (m_highlightedGizmoDirection)
			{
				auto& selectedActor = EDITOR_EXEC(GetSelectedActor());
				auto [winWidth, winHeight] = GetSafeSize();
				const auto renderSize = OvMaths::FVector2{
					winWidth > 0 ? static_cast<float>(winWidth) : 1.0f,
					winHeight > 0 ? static_cast<float>(winHeight) : 1.0f
				};
				const auto uiGizmoContext = ResolveUIGizmoContext(
					selectedActor,
					renderSize,
					EDITOR_EXEC(IsSceneUIRenderingEnabled())
				);
				const auto* uiOrigin = uiGizmoContext ? &uiGizmoContext->origin : nullptr;
				const auto* uiUnitsScale = uiGizmoContext ? &uiGizmoContext->unitsScale : nullptr;

				m_gizmoOperations.StartPicking(
					selectedActor,
					m_camera.GetPosition(),
					m_currentOperation,
					m_highlightedGizmoDirection.value(),
					uiOrigin,
					uiUnitsScale
				);
			}
			else if (m_highlightedActor)
			{
				EDITOR_EXEC(SelectActor(m_highlightedActor.value()));
			}
			else
			{
				EDITOR_EXEC(UnselectActor());
			}
		}
	}
	else
	{
		m_highlightedActor = std::nullopt;
		m_highlightedGizmoDirection = std::nullopt;
	}

	if (m_gizmoOperations.IsPicking())
	{
		auto [winWidth, winHeight] = GetSafeSize();

		auto mousePosition = EDITOR_CONTEXT(inputManager)->GetMousePosition();

		m_gizmoOperations.SetCurrentMouse({ static_cast<float>(mousePosition.first - m_position.x), static_cast<float>(mousePosition.second - m_position.y - ImGui::GetFrameHeight()) });
		m_gizmoOperations.ApplyOperation(m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(), m_camera.GetPosition(), { static_cast<float>(winWidth), static_cast<float>(winHeight) });
		m_highlightedGizmoDirection = m_gizmoOperations.GetDirection();
	}
}

OvEditor::Rendering::PickingRenderPass::PickingResult OvEditor::Panels::SceneView::GetPickingResult()
{
	auto [mouseX, mouseY] = EDITOR_CONTEXT(inputManager)->GetMousePosition();
	mouseX -= m_position.x;
	mouseY -= m_position.y;
	mouseY = GetSafeSize().second - mouseY + ImGui::GetFrameHeight();

	auto& scene = *GetScene();

	auto& actorPickingFeature = m_renderer->GetPass<OvEditor::Rendering::PickingRenderPass>("Picking");

	return actorPickingFeature.ReadbackPickingResult(
		scene,
		static_cast<uint32_t>(mouseX),
		static_cast<uint32_t>(mouseY)
	);
}

void OvEditor::Panels::SceneView::OnSceneDropped(const std::string& p_path)
{
	EDITOR_EXEC(LoadSceneFromDisk(p_path));
}

void OvEditor::Panels::SceneView::OnModelDropped(const std::string& p_path)
{
	EDITOR_EXEC(CreateActorWithModel(p_path, true));
}

void OvEditor::Panels::SceneView::OnMaterialDropped(const std::string& p_path)
{
	const auto pickingResult = GetPickingResult();

	if (auto actor = GetActorFromPickingResult(pickingResult))
	{
		if (auto materialRenderer = actor->GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
		{
			const auto resourcePath = EDITOR_EXEC(GetResourcePath(p_path));

			if (const auto material = EDITOR_CONTEXT(materialManager)[resourcePath])
			{
				materialRenderer->SetMaterialAtIndex(0, *material);
			}
		}
	}
}

void OvEditor::Panels::SceneView::OnPrefabDropped(const std::string& p_path)
{
	if (auto* actor = EDITOR_EXEC(InstantiatePrefab(p_path)); actor)
	{
		EDITOR_EXEC(SelectActor(*actor));
	}
}
