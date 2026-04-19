/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Core/EditorActions.h"
#include <tracy/Tracy.hpp>

#include <filesystem>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/GUIHelpers.h>

#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/MaterialManager.h>

#include <OvTools/Utils/SystemCalls.h>
#include <OvTools/Utils/PathParser.h>

#include <OvEditor/Core/Editor.h>
#include <OvEditor/Panels/AssetBrowser.h>
#include <OvEditor/Panels/ItemPicker.h>
#include <OvEditor/Panels/AssetProperties.h>
#include <OvEditor/Panels/AssetView.h>
#include <OvEditor/Panels/Console.h>
#include <OvEditor/Panels/FrameInfo.h>
#include <OvEditor/Panels/GameView.h>
#include <OvEditor/Panels/HardwareInfo.h>
#include <OvEditor/Panels/Hierarchy.h>
#include <OvEditor/Panels/Inspector.h>
#include <OvEditor/Panels/MaterialEditor.h>
#include <OvEditor/Panels/MenuBar.h>
#include <OvEditor/Panels/ProjectSettings.h>
#include <OvEditor/Panels/SceneView.h>
#include <OvEditor/Panels/TextureDebugger.h>
#include <OvEditor/Panels/Toolbar.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvPhysics/Core/PhysicsEngine.h>
#include <OvUI/Settings/PanelWindowSettings.h>

using namespace OvCore::ResourceManagement;
using namespace OvEditor::Panels;
using namespace OvRendering::Resources::Loaders;
using namespace OvRendering::Resources::Parsers;

OvEditor::Core::Editor::Editor(Context& p_context) :
	m_context(p_context),
	m_panelsManager(m_canvas),
	m_editorActions(m_context, m_panelsManager)
{
	SetupUI();

	m_context.sceneManager.LoadDefaultScene();
}

OvEditor::Core::Editor::~Editor()
{
	Settings::EditorSettings::Save();
	m_context.sceneManager.UnloadCurrentScene();
}

void OvEditor::Core::Editor::SetupUI()
{
	OvUI::Settings::PanelWindowSettings settings;
	settings.closable = true;
	settings.collapsable = true;
	settings.dockable = true;

	OvCore::Helpers::GUIHelpers::SetPickerProvider(
		[this](OvCore::Helpers::GUIHelpers::PickerItemList p_items, std::string p_title) {
			m_itemPicker->Open(std::move(p_items), std::move(p_title));
		}
	);

	OvCore::Helpers::GUIHelpers::SetIconProvider(
		[this](OvTools::Utils::PathParser::EFileType p_fileType) -> uint32_t {
			auto* texture = m_context.editorResources->GetTexture(OvTools::Utils::PathParser::FileTypeToString(p_fileType));
			return texture ? texture->GetTexture().GetID() : 0;
		}
	);

	OvCore::Helpers::GUIHelpers::SetOpenProvider(
		[this](const std::string& p_path)
		{
			using EFileType = OvTools::Utils::PathParser::EFileType;
			const auto fileType = OvTools::Utils::PathParser::GetFileType(p_path);
			const auto path = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_path);

			auto openInAssetView = [&](auto* p_resource)
			{
				if (!p_resource) return;
				auto& assetView = EDITOR_PANEL(AssetView, "Asset View");
				assetView.SetResource(AssetView::ViewableResource{ p_resource });
				assetView.Open();
				assetView.Focus();
			};

			if (fileType == EFileType::TEXTURE)
			{
				openInAssetView(OVSERVICE(TextureManager).GetResource(path));
			}
			else if (fileType == EFileType::MODEL)
			{
				openInAssetView(OVSERVICE(ModelManager).GetResource(path));
			}
			else if (fileType == EFileType::MATERIAL)
			{
				auto* material = OVSERVICE(MaterialManager).GetResource(path);
				openInAssetView(material);
				if (material)
				{
					auto& materialEditor = EDITOR_PANEL(MaterialEditor, "Material Editor");
					EDITOR_EXEC(DelayAction([material, &materialEditor]() {
						materialEditor.SetTarget(*material);
						materialEditor.Open();
						materialEditor.Focus();
					}));
				}
			}
			else if (fileType == EFileType::SCENE)
			{
				EDITOR_EXEC(LoadSceneFromDisk(path));
			}
			else if (fileType == EFileType::SCRIPT || fileType == EFileType::SHADER || fileType == EFileType::SHADER_PART)
			{
				EDITOR_EXEC(OpenInCodeEditor(m_editorActions.GetRealPath(path)));
			}
			else
			{
				// SOUND, FONT, UNKNOWN → open with OS default
				OvTools::Utils::SystemCalls::OpenFile(EDITOR_EXEC(GetRealPath(path)));
			}
		}
	);

	// Provide the actor icon ID for ActorField widgets.
	if (auto* actorTexture = m_context.editorResources->GetTexture("Actor"))
		OvCore::Helpers::GUIHelpers::SetActorIconID(actorTexture->GetTexture().GetID());

	// Provide asset existence checker so AssetFields show "(Missing Reference)" for invalid paths.
	OvCore::Helpers::GUIHelpers::SetAssetExistsChecker(
		[this](const std::string& p_path)
		{
			return std::filesystem::exists(m_editorActions.GetRealPath(p_path));
		}
	);

	// Provide actor selection so double-clicking an ActorField selects it in the inspector.
	OvCore::Helpers::GUIHelpers::SetActorSelectionProvider(
		[this](uint64_t p_guid)
		{
			// Defer to next frame — selecting an actor rebuilds the inspector widget tree,
			// which would corrupt iteration if called directly from within DrawWidgets().
			EDITOR_EXEC(DelayAction([this, p_guid]()
			{
				auto* scene = m_context.sceneManager.GetCurrentScene();
				if (!scene) return;
				auto* actor = scene->FindActorByGUID(p_guid);
				if (actor)
					EDITOR_EXEC(SelectActor(*actor));
			}));
		}
	);

	m_panelsManager.CreatePanel<Panels::MenuBar>("Menu Bar");
	m_panelsManager.CreatePanel<Panels::AssetBrowser>("Asset Browser", true, settings);
	m_panelsManager.CreatePanel<Panels::HardwareInfo>("Hardware Info", false, settings);
	m_panelsManager.CreatePanel<Panels::FrameInfo>("Frame Info", true, settings);
	m_panelsManager.CreatePanel<Panels::Console>("Console", true, settings);
	m_panelsManager.CreatePanel<Panels::AssetView>("Asset View", false, settings);
	m_panelsManager.CreatePanel<Panels::Hierarchy>("Hierarchy", true, settings);
	m_panelsManager.CreatePanel<Panels::Inspector>("Inspector", true, settings);
	m_panelsManager.CreatePanel<Panels::SceneView>("Scene View", true, settings);
	m_panelsManager.CreatePanel<Panels::GameView>("Game View", true, settings);
	m_panelsManager.CreatePanel<Panels::Toolbar>("Toolbar", true, settings);
	m_panelsManager.CreatePanel<Panels::MaterialEditor>("Material Editor", false, settings);
	m_panelsManager.CreatePanel<Panels::ProjectSettings>("Project Settings", false, settings);
	m_panelsManager.CreatePanel<Panels::AssetProperties>("Asset Properties", false, settings);
	m_panelsManager.CreatePanel<Panels::TextureDebugger>("Texture Debugger", false, settings);

	// Needs to be called after all panels got created, because some settings in this menu depend on other panels
	m_panelsManager.GetPanelAs<Panels::MenuBar>("Menu Bar").InitializeSettingsMenu();

	m_canvas.MakeDockspace(true);
	m_context.uiManager->SetCanvas(m_canvas);

	m_itemPicker = std::make_unique<OvEditor::Panels::ItemPicker>(
		false,
		OvUI::Settings::PanelWindowSettings{ .closable = true }
	);

	m_canvas.AddPanel(*m_itemPicker);
}

void OvEditor::Core::Editor::PreUpdate()
{
	ZoneScopedN("Editor Pre-Update");
	m_context.device->PollEvents();
}

void OvEditor::Core::Editor::Update(float p_deltaTime)
{
	// Disable mouse input when the cursor is locked during gameplay or view interaction.
	const bool mouseEnabled = m_context.window->GetCursorMode() != OvWindowing::Cursor::ECursorMode::DISABLED;
	m_context.uiManager->EnableMouse(mouseEnabled);

	HandleGlobalShortcuts();
	UpdateCurrentEditorMode(p_deltaTime);
	RenderViews(p_deltaTime);
	UpdateEditorPanels(p_deltaTime);
	RenderEditorUI(p_deltaTime);
	m_editorActions.ExecuteDelayedActions();
}

void OvEditor::Core::Editor::HandleGlobalShortcuts()
{
	auto& sceneView = EDITOR_PANEL(SceneView, "Scene View");
	auto& hierarchy = EDITOR_PANEL(Hierarchy, "Hierarchy");
	const bool isSceneViewFocused = sceneView.IsFocused();
	const bool isHierarchyFocused = hierarchy.IsFocused();

	// If the [Del] key is pressed while an actor is selected and the Scene View or Hierarchy is focused
	if (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_DELETE) && EDITOR_EXEC(IsAnyActorSelected()) && (isSceneViewFocused || isHierarchyFocused))
	{
		EDITOR_EXEC(DestroyActor(EDITOR_EXEC(GetSelectedActor())));
	}

	const bool isControlPressed =
		m_context.inputManager->GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_CONTROL) == OvWindowing::Inputs::EKeyState::KEY_DOWN ||
		m_context.inputManager->GetKeyState(OvWindowing::Inputs::EKey::KEY_RIGHT_CONTROL) == OvWindowing::Inputs::EKeyState::KEY_DOWN;

	if (isControlPressed && (isSceneViewFocused || isHierarchyFocused))
	{
		if (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_C) && EDITOR_EXEC(IsAnyActorSelected()))
		{
			EDITOR_EXEC(CopyActor(EDITOR_EXEC(GetSelectedActor())));
		}

		if (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_V))
		{
			OvCore::ECS::Actor* parent = nullptr;

			if (EDITOR_EXEC(IsAnyActorSelected()))
			{
				parent = &EDITOR_EXEC(GetSelectedActor());
			}

			EDITOR_EXEC(PasteActor(parent));
		}
	}
}

void OvEditor::Core::Editor::UpdateCurrentEditorMode(float p_deltaTime)
{
	if (auto editorMode = m_editorActions.GetCurrentEditorMode(); editorMode == EditorActions::EEditorMode::PLAY || editorMode == EditorActions::EEditorMode::FRAME_BY_FRAME)
		UpdatePlayMode(p_deltaTime);
	else
		UpdateEditMode(p_deltaTime);

	{
		ZoneScopedN("Scene garbage collection");
		m_context.sceneManager.GetCurrentScene()->CollectGarbages();
		m_context.sceneManager.Update();
	}
}

void OvEditor::Core::Editor::UpdatePlayMode(float p_deltaTime)
{
	auto currentScene = m_context.sceneManager.GetCurrentScene();
	bool simulationApplied = false;

	{
		ZoneScopedN("Physics Update");
		simulationApplied = m_context.physicsEngine->Update(p_deltaTime);
	}

	if (simulationApplied)
	{
		ZoneScopedN("Fixed Update");
		currentScene->FixedUpdate(p_deltaTime);
	}

	{
		ZoneScopedN("Update");
		currentScene->Update(p_deltaTime);
	}

	{
		ZoneScopedN("Late Update");
		currentScene->LateUpdate(p_deltaTime);
	}

	{
		ZoneScopedN("Audio Update");
		m_context.audioEngine->Update();
	}

	if (m_editorActions.GetCurrentEditorMode() == EditorActions::EEditorMode::FRAME_BY_FRAME)
		m_editorActions.PauseGame();

	if (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_ESCAPE))
		m_editorActions.StopPlaying();
}

void OvEditor::Core::Editor::UpdateEditMode(float p_deltaTime)
{
	if (m_context.inputManager->IsKeyPressed(OvWindowing::Inputs::EKey::KEY_F5))
		m_editorActions.StartPlaying();
}

void OvEditor::Core::Editor::UpdateEditorPanels(float p_deltaTime)
{
	auto& menuBar = m_panelsManager.GetPanelAs<OvEditor::Panels::MenuBar>("Menu Bar");
	auto& frameInfo = m_panelsManager.GetPanelAs<OvEditor::Panels::FrameInfo>("Frame Info");
	auto& hardwareInfo = m_panelsManager.GetPanelAs<OvEditor::Panels::HardwareInfo>("Hardware Info");
	auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View");
	auto& gameView = m_panelsManager.GetPanelAs<OvEditor::Panels::GameView>("Game View");
	auto& assetView = m_panelsManager.GetPanelAs<OvEditor::Panels::AssetView>("Asset View");
	auto& textureDebugger = m_panelsManager.GetPanelAs<OvEditor::Panels::TextureDebugger>("Texture Debugger");

	menuBar.HandleShortcuts(p_deltaTime);

	if (m_elapsedFrames == 1) // Let the first frame happen and then make the scene view the first seen view
		sceneView.Focus();
	
	m_lastFocusedView =
		sceneView.IsVisible() && sceneView.IsFocused() ? sceneView :
		gameView.IsVisible() && gameView.IsFocused() ? gameView :
		assetView.IsVisible() && assetView.IsFocused() ? assetView :
		m_lastFocusedView;

	if (frameInfo.IsOpened())
	{
		ZoneScopedN("Frame Info Update");
		frameInfo.Update(m_lastFocusedView, p_deltaTime);
	}

	if (textureDebugger.IsOpened())
	{
		ZoneScopedN("Texture Debugger Update");
		textureDebugger.Update(p_deltaTime);
	}
}

void OvEditor::Core::Editor::RenderViews(float p_deltaTime)
{
	auto& assetView = m_panelsManager.GetPanelAs<OvEditor::Panels::AssetView>("Asset View");
	auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View");
	auto& gameView = m_panelsManager.GetPanelAs<OvEditor::Panels::GameView>("Game View");

	{
		ZoneScopedN("Editor Views Update");

		if (assetView.IsOpened())
		{
			assetView.Update(p_deltaTime);
		}

		if (gameView.IsOpened())
		{
			gameView.Update(p_deltaTime);
		}

		if (sceneView.IsOpened())
		{
			sceneView.Update(p_deltaTime);
		}
	}

	if (assetView.IsOpened() && assetView.IsVisible())
	{
		ZoneScopedN("Asset View Rendering");
		assetView.Render();
	}

	if (gameView.IsOpened() && gameView.IsVisible())
	{
		ZoneScopedN("Game View Rendering");
		gameView.Render();
	}

	if (sceneView.IsOpened() && sceneView.IsVisible())
	{
		ZoneScopedN("Scene View Rendering");
		sceneView.Render();
	}
}

void OvEditor::Core::Editor::RenderEditorUI(float p_deltaTime)
{
	ZoneScopedN("Editor UI Rendering");

	EDITOR_CONTEXT(uiManager)->Render();
}

void OvEditor::Core::Editor::PostUpdate()
{
	ZoneScopedN("Editor Post-Update");

	m_context.window->SwapBuffers();
	m_context.inputManager->ClearEvents();
	m_context.driver->OnFrameCompleted();
	++m_elapsedFrames;
}
