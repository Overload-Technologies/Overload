/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvEditor/Rendering/DebugSceneRenderer.h>
#include <OvEditor/Rendering/PickingRenderPass.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/SceneView.h>
#include <OvEditor/Panels/GameView.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvUI/Plugins/DDTarget.h>

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
	m_fallbackMaterial.Set<OvMaths::FVector4>("u_Diffuse", { 1.f, 0.f, 1.f, 1.0f });
	m_fallbackMaterial.Set<OvRendering::Resources::Texture*>("u_DiffuseMap", nullptr);

	m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	{
		std::string path = p_data.first;

		switch (OvTools::Utils::PathParser::GetFileType(path))
		{
		case OvTools::Utils::PathParser::EFileType::SCENE:	EDITOR_EXEC(LoadSceneFromDisk(path));			break;
		case OvTools::Utils::PathParser::EFileType::MODEL:	EDITOR_EXEC(CreateActorWithModel(path, true));	break;
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
			m_currentOperation = OvEditor::Core::EGizmoOperation::TRANSLATE;
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_E))
		{
			m_currentOperation = OvEditor::Core::EGizmoOperation::ROTATE;
		}

		if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(EKey::KEY_R))
		{
			m_currentOperation = OvEditor::Core::EGizmoOperation::SCALE;
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
}

OvCore::SceneSystem::Scene* OvEditor::Panels::SceneView::GetScene()
{
	return m_sceneManager.GetCurrentScene();
}

OvCore::Rendering::SceneRenderer::SceneDescriptor OvEditor::Panels::SceneView::CreateSceneDescriptor()
{
	auto descriptor = AViewControllable::CreateSceneDescriptor();
	descriptor.fallbackMaterial = m_fallbackMaterial;

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

	if (IsHovered() && !IsResizing())
	{
		auto [mouseX, mouseY] = inputManager.GetMousePosition();
		mouseX -= m_position.x;
		mouseY -= m_position.y;
		mouseY = GetSafeSize().second - mouseY + 25;

		auto& scene = *GetScene();
		
		auto& actorPickingFeature = m_renderer->GetPass<Rendering::PickingRenderPass>("Picking");

		auto pickingResult = actorPickingFeature.ReadbackPickingResult(
			scene,
			static_cast<uint32_t>(mouseX),
			static_cast<uint32_t>(mouseY)
		);

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
				m_gizmoOperations.StartPicking(
					EDITOR_EXEC(GetSelectedActor()),
					m_camera.GetPosition(),
					m_currentOperation,
					m_highlightedGizmoDirection.value());
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

		m_gizmoOperations.SetCurrentMouse({ static_cast<float>(mousePosition.first - m_position.x), static_cast<float>(mousePosition.second - m_position.y) });
		m_gizmoOperations.ApplyOperation(m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(), m_camera.GetPosition(), { static_cast<float>(winWidth), static_cast<float>(winHeight) });
	}
}
