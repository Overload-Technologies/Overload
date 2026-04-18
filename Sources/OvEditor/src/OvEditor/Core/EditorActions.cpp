/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include "OvDebug/Assertion.h"
#include "OvTools/Utils/OptRef.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string_view>
#include <vector>
#include <tinyxml2.h>

#include <OvDebug/Logger.h>

#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/Resources/Loaders/MaterialLoader.h>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/GUIHelpers.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/GizmoBehaviour.h>
#include <OvEditor/Helpers/PickerHelpers.h>
#include <OvEditor/Panels/AssetView.h>
#include <OvEditor/Panels/GameView.h>
#include <OvEditor/Panels/Inspector.h>
#include <OvEditor/Panels/MaterialEditor.h>
#include <OvEditor/Panels/ProjectSettings.h>
#include <OvEditor/Panels/SceneView.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvEditor/Utils/FileSystem.h>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/String.h>
#include <OvTools/Utils/SystemCalls.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>

#include <OvWindowing/Dialogs/OpenFileDialog.h>
#include <OvWindowing/Dialogs/MessageBox.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>

namespace
{
	constexpr std::string_view kDefaultMaterialPath = ":Materials\\Default.ovmat";

	void RefreshMaterialsUsingShader(
		OvCore::ResourceManagement::MaterialManager& p_materialManager,
		OvRendering::Resources::Shader& p_shader
	)
	{
		for (const auto material : p_materialManager.GetResources() | std::views::values)
		{
			if (material && material->GetShader() == &p_shader)
			{
				material->UpdateProperties();
			}
		}
	}

	void RefreshMaterialEditorIfTargetUsesShader(OvRendering::Resources::Shader& p_shader)
	{
		auto& materialEditor = EDITOR_PANEL(OvEditor::Panels::MaterialEditor, "Material Editor");

		if (auto* targetMaterial = materialEditor.GetTarget(); targetMaterial && targetMaterial->GetShader() == &p_shader)
		{
			materialEditor.Refresh();
		}
	}

	template<typename TResourceManager, typename TAssetNameValidator>
	void MoveEmbeddedResourcesForRenamedModel(
		TResourceManager& p_resourceManager,
		const std::string& p_previousModelPath,
		const std::string& p_newModelPath,
		TAssetNameValidator p_validateAssetName
	)
	{
		std::vector<std::pair<std::string, std::string>> moves;

		for (const auto& [resourcePath, resource] : p_resourceManager.GetResources())
		{
			(void)resource;

			const auto embeddedPath = OvRendering::Resources::Parsers::ParseEmbeddedAssetPath(resourcePath.string());
			if (!embeddedPath || embeddedPath->modelPath != p_previousModelPath)
			{
				continue;
			}

			if (!p_validateAssetName(embeddedPath->assetName))
			{
				continue;
			}

			moves.emplace_back(resourcePath.string(), p_newModelPath + ":" + embeddedPath->assetName);
		}

		for (const auto& [oldPath, newPath] : moves)
		{
			if (!p_resourceManager.MoveResource(oldPath, newPath))
			{
				continue;
			}

			if (auto* resource = p_resourceManager.GetResource(newPath, false))
			{
				const_cast<std::string&>(resource->path) = newPath;
			}
		}
	}

	void MoveAllEmbeddedResourcesForRenamedModel(const std::string& p_previousModelPath, const std::string& p_newModelPath)
	{
		MoveEmbeddedResourcesForRenamedModel(
			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>(),
			p_previousModelPath,
			p_newModelPath,
			[](const std::string& p_assetName)
			{
				return OvRendering::Resources::Parsers::ParseEmbeddedMaterialIndex(p_assetName).has_value();
			}
		);

		MoveEmbeddedResourcesForRenamedModel(
			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>(),
			p_previousModelPath,
			p_newModelPath,
			[](const std::string& p_assetName)
			{
				return OvRendering::Resources::Parsers::ParseEmbeddedTextureIndex(p_assetName).has_value();
			}
		);
	}
}

std::string OvEditor::Core::GetBuildTypeName(OvEditor::Core::EBuildType p_buildType)
{
	switch (p_buildType)
	{
		case OvEditor::Core::EBuildType::Debug: return "Debug";
		case OvEditor::Core::EBuildType::Release: return "Release";
		case OvEditor::Core::EBuildType::Publish: return "Publish";
	}

	OVASSERT(false, "Unknown build type used.");
	return "";
}

OvEditor::Core::EditorActions::EditorActions(Context& p_context, PanelsManager& p_panelsManager) :
	m_context(p_context), 
	m_panelsManager(p_panelsManager)
{
	OvCore::Global::ServiceLocator::Provide<OvEditor::Core::EditorActions>(*this);

	OvCore::Helpers::GUIHelpers::SetFileItemBuilder(
		[](OvTools::Utils::PathParser::EFileType p_type, std::function<void(std::string)> p_callback, bool p_searchProject, bool p_searchEngine) {
			OvCore::Helpers::GUIHelpers::PickerItemList items;
			OvEditor::Helpers::PickerHelpers::AddFileItems(items, p_type, std::move(p_callback), p_searchProject, p_searchEngine);
			return items;
		}
	);

	m_context.sceneManager.CurrentSceneSourcePathChangedEvent += [this](const std::string& p_newPath)
	{
		std::string titleExtra = " - " + (p_newPath.empty() ? "Untitled Scene" : GetResourcePath(p_newPath));
		m_context.window->SetTitle(m_context.windowSettings.title + titleExtra);
	};
}

OvEditor::Core::EditorActions::~EditorActions()
{
}

void OvEditor::Core::EditorActions::LoadEmptyScene()
{
	if (GetCurrentEditorMode() != EEditorMode::EDIT)
		StopPlaying();

	m_context.sceneManager.LoadDefaultScene();

	OVLOG_INFO("New scene created");
}

void OvEditor::Core::EditorActions::SaveSceneToDisk(OvCore::SceneSystem::Scene& p_scene, const std::string& p_path)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* node = doc.NewElement("root");
	doc.InsertFirstChild(node);
	m_context.sceneManager.StoreCurrentSceneSourcePath(p_path);
	p_scene.OnSerialize(doc, node);
	doc.SaveFile(p_path.c_str());
}

void OvEditor::Core::EditorActions::LoadSceneFromDisk(const std::string& p_path, bool p_absolute)
{
	if (GetCurrentEditorMode() != EEditorMode::EDIT)
	{
		StopPlaying();
	}

	if (!m_context.sceneManager.LoadScene(p_path, p_absolute))
	{
		OVLOG_ERROR("Failed to load scene from disk: " + p_path);
		return;
	}

	OVLOG_INFO("Scene loaded from disk: " + m_context.sceneManager.GetCurrentSceneSourcePath());
	m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View").Focus();
}

bool OvEditor::Core::EditorActions::IsCurrentSceneLoadedFromDisk() const
{
	return m_context.sceneManager.IsCurrentSceneLoadedFromDisk();
}

void OvEditor::Core::EditorActions::SaveSceneChanges()
{
	if (IsCurrentSceneLoadedFromDisk())
	{
		auto currentScene = m_context.sceneManager.GetCurrentScene();
		OVASSERT(currentScene, "Current scene is null");

		const std::string currentScenePath = m_context.sceneManager.GetCurrentSceneSourcePath();
		SaveSceneToDisk(*currentScene, currentScenePath);
		OVLOG_INFO("Current scene saved to: " + currentScenePath);
	}
	else
	{
		SaveAs();
	}
}

void OvEditor::Core::EditorActions::SaveAs()
{
	OvWindowing::Dialogs::SaveFileDialog dialog("New Scene");
	const auto initialPath = m_context.projectAssetsPath / "New Scene";
	dialog.SetInitialDirectory(initialPath.string());
	dialog.DefineExtension("Overload Scene", ".ovscene");
	dialog.Show();

	if (dialog.HasSucceeded())
	{
		if (dialog.IsFileExisting())
		{
			OvWindowing::Dialogs::MessageBox message("File already exists!", "The file \"" + dialog.GetSelectedFileName() + "\" already exists.\n\nUsing this file as the new home for your scene will erase any content stored in this file.\n\nAre you ok with that?", OvWindowing::Dialogs::MessageBox::EMessageType::WARNING, OvWindowing::Dialogs::MessageBox::EButtonLayout::YES_NO, true);
			switch (message.GetUserAction())
			{
			case OvWindowing::Dialogs::MessageBox::EUserAction::YES: break;
			default: return;
			}
		}
		
		auto currentScene = m_context.sceneManager.GetCurrentScene();
		SaveSceneToDisk(*currentScene, dialog.GetSelectedFilePath());
		OVLOG_INFO("Current scene saved to: " + dialog.GetSelectedFilePath());
	}
}

void OvEditor::Core::EditorActions::RefreshScripts()
{
	m_context.scriptEngine->Reload();
	m_panelsManager.GetPanelAs<Panels::Inspector>("Inspector").Refresh();
	if (m_context.scriptEngine->IsOk())
		OVLOG_INFO("Scripts interpretation succeeded!");
}

std::optional<std::string> OvEditor::Core::EditorActions::SelectBuildFolder()
{
	OvWindowing::Dialogs::SaveFileDialog dialog("Build location");
	dialog.DefineExtension("Game Build", "..");
	dialog.Show();
	if (dialog.HasSucceeded())
	{
		std::string selectedPath = dialog.GetSelectedFilePath();

		if (selectedPath.ends_with(".."))
		{
			selectedPath.erase(selectedPath.size() - 2);
		}

		if (!std::filesystem::exists(selectedPath))
		{
			return selectedPath;
		}
		else
		{
			OvWindowing::Dialogs::MessageBox message(
				"Folder already exists!",
				std::format("The folder \"{}\" already exists.\n\nPlease select another location and try again", selectedPath),
				OvWindowing::Dialogs::MessageBox::EMessageType::WARNING,
				OvWindowing::Dialogs::MessageBox::EButtonLayout::OK,
				true
			);

			return {};
		}
	}
	else
	{
		return {};
	}
}

void OvEditor::Core::EditorActions::Build(bool p_autoRun, bool p_tempFolder)
{
	std::filesystem::path destinationFolder;

	if (p_tempFolder)
	{
		destinationFolder = Utils::FileSystem::kEditorDataPath / "TempBuild";

		try
		{
			std::filesystem::remove_all(destinationFolder);
		}
		catch (std::filesystem::filesystem_error error)
		{
			OvWindowing::Dialogs::MessageBox message(
				"Temporary build failed",
				"The temporary folder is currently being used by another process",
				OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
				OvWindowing::Dialogs::MessageBox::EButtonLayout::OK,
				true
			);

			return;
		}
	}
	else if (auto res = SelectBuildFolder(); res.has_value())
	{
		destinationFolder = res.value();
	}
	else
	{
		return; // Operation cancelled (No folder selected)
	}

	BuildAtLocation(
		static_cast<EBuildType>(m_context.projectSettings.Get<int>("build_type")),
		destinationFolder,
		p_autoRun
	);
}

void OvEditor::Core::EditorActions::BuildAtLocation(
	EBuildType p_buildType,
	const std::filesystem::path& p_buildPath,
	bool p_autoRun
)
{
	const std::string extension = 
#if defined(_WIN32)
		".exe";
#else
		"";
#endif

	const std::string executableName = m_context.projectSettings.Get<std::string>("executable_name") + extension;

	bool failed = false;

	OVLOG_INFO(std::format("Preparing to build at location: \"{}\"", p_buildPath.string()));

	std::filesystem::remove_all(p_buildPath);

	if (std::filesystem::create_directory(p_buildPath))
	{
		OVLOG_INFO("Build directory created");

		if (std::filesystem::create_directory(p_buildPath / "Data"))
		{
			OVLOG_INFO("Data directory created");

			if (std::filesystem::create_directory(p_buildPath / "Data" / "User"))
			{
				OVLOG_INFO("Data/User directory created");

				std::error_code err;

				std::filesystem::copy(m_context.projectFile, p_buildPath / "Data" / "User" / "Game.ini", err);

				if (!err)
				{
					OVLOG_INFO("Data/User/Game.ini file generated");
		
					std::filesystem::copy(
						m_context.projectAssetsPath,
						p_buildPath / "Data" / "User" / "Assets",
						std::filesystem::copy_options::recursive,
						err
					);

					auto sceneFileName = m_context.projectSettings.Get<std::string>("start_scene");
					sceneFileName = OvTools::Utils::PathParser::MakeNonWindowsStyle(sceneFileName);

					if (!std::filesystem::exists(p_buildPath / "Data" / "User" / "Assets" / sceneFileName))
					{
						OVLOG_ERROR("Failed to find Start Scene at expected path. Verify your Project Setings.");

						OvWindowing::Dialogs::MessageBox message(
							"Build Failure",
							"An error occured during the building of your game.\nCheck the console for more information",
							OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
							OvWindowing::Dialogs::MessageBox::EButtonLayout::OK,
							true
						);

						std::filesystem::remove_all(p_buildPath);
						return;						
					}

					if (!err)
					{
						OVLOG_INFO("Data/User/Assets/ directory copied");

						std::filesystem::copy(
							m_context.engineAssetsPath,
							p_buildPath / "Data" / "Engine",
							std::filesystem::copy_options::recursive,
							err
						);

						if (!err)
						{
							OVLOG_INFO("Data/Engine/ directory copied");
						}
						else
						{
							OVLOG_ERROR("Data/Engine/ directory failed to copy");
							failed = true;
						}
					}
					else
					{
						OVLOG_ERROR("Data/User/Assets/ directory failed to copy");
						failed = true;
					}
				}
				else
				{
					OVLOG_ERROR("Data/User/Game.ini file failed to generate");
					failed = true;
				}

				const auto builderFolder = std::filesystem::current_path() / "Builder" / GetBuildTypeName(p_buildType);

				const std::string initialExecutableName = "OvGame" + extension;

				if (std::filesystem::exists(builderFolder) && std::filesystem::exists(builderFolder / initialExecutableName))
				{
					std::error_code err;

					std::filesystem::copy(builderFolder, p_buildPath, err);

					if (!err)
					{
						OVLOG_INFO("Builder data (Dlls and executable) copied");
							
						std::filesystem::rename(p_buildPath / initialExecutableName, p_buildPath / executableName, err);

						if (!err)
						{
							OVLOG_INFO("Game executable renamed to " + executableName);

							if (p_autoRun)
							{
								const auto exePath = p_buildPath / executableName;
								OVLOG_INFO(std::format("Launching the game at location: \"{}\"", exePath.string()));

								if (std::filesystem::exists(exePath))
								{
									OvTools::Utils::SystemCalls::RunProgram(exePath.string(), p_buildPath.string());
								}
								else
								{
									OVLOG_ERROR("Failed to start the game: Executable not found");
									failed = true;
								}
							}
						}
						else
						{
							OVLOG_ERROR("Game executable failed to rename");
							failed = true;
						}
					}
					else
					{
						OVLOG_ERROR("Builder data (Dlls and executatble) failed to copy");
						failed = true;
					}
				}
				else
				{
					OVLOG_ERROR(std::format(
						"OvGame executable not found for \"{}\" configuration. Build OvGame and OvEditor in that configuration first, then try again.",
						GetBuildTypeName(p_buildType)
					));
					failed = true;
				}
			}
		}
	}
	else
	{
		OVLOG_ERROR("Build directory failed to create");
		failed = true;
	}

	if (failed)
	{
		std::filesystem::remove_all(p_buildPath);
		OvWindowing::Dialogs::MessageBox message(
			"Build Failure",
			"An error occured during the building of your game.\nCheck the console for more information",
			OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
			OvWindowing::Dialogs::MessageBox::EButtonLayout::OK,
			true
		);
	}
}

void OvEditor::Core::EditorActions::OpenProfiler()
{
	const auto profilerPath = std::filesystem::current_path() / "Tools" / "tracy-profiler.exe";
	OvTools::Utils::SystemCalls::OpenFile(profilerPath.string());
}

void OvEditor::Core::EditorActions::DelayAction(std::function<void()> p_action, uint32_t p_frames)
{
	m_delayedActions.emplace_back(p_frames + 1, p_action);
}

void OvEditor::Core::EditorActions::ExecuteDelayedActions()
{
	std::for_each(m_delayedActions.begin(), m_delayedActions.end(), [](std::pair<uint32_t, std::function<void()>> & p_element)
	{
		--p_element.first;

		if (p_element.first == 0)
			p_element.second();
	});

	m_delayedActions.erase(std::remove_if(m_delayedActions.begin(), m_delayedActions.end(), [](std::pair<uint32_t, std::function<void()>> & p_element)
	{
		return p_element.first == 0;
	}), m_delayedActions.end());
}

OvEditor::Core::Context& OvEditor::Core::EditorActions::GetContext()
{
	return m_context;
}

OvEditor::Core::PanelsManager& OvEditor::Core::EditorActions::GetPanelsManager()
{
	return m_panelsManager;
}

void OvEditor::Core::EditorActions::SetActorSpawnAtOrigin(bool p_value)
{
	if (p_value)
		m_actorSpawnMode = EActorSpawnMode::ORIGIN;
	else
		m_actorSpawnMode = EActorSpawnMode::FRONT;
}

void OvEditor::Core::EditorActions::SetActorSpawnMode(EActorSpawnMode p_value)
{
	m_actorSpawnMode = p_value;
}

void OvEditor::Core::EditorActions::ResetLayout()
{
	DelayAction([this]() {
		m_context.uiManager->ResetLayout(
			(std::filesystem::current_path() / "Config" / "layout.ini").string()
		);
	});
}

void OvEditor::Core::EditorActions::SetSceneViewCameraSpeed(int p_speed)
{
	EDITOR_PANEL(Panels::SceneView, "Scene View").GetCameraController().SetSpeed((float)p_speed);
}

int OvEditor::Core::EditorActions::GetSceneViewCameraSpeed()
{
	return (int)EDITOR_PANEL(Panels::SceneView, "Scene View").GetCameraController().GetSpeed();
}

void OvEditor::Core::EditorActions::SetAssetViewCameraSpeed(int p_speed)
{
	EDITOR_PANEL(Panels::AssetView, "Asset View").GetCameraController().SetSpeed((float)p_speed);
}

int OvEditor::Core::EditorActions::GetAssetViewCameraSpeed()
{
	return (int)EDITOR_PANEL(Panels::AssetView, "Asset View").GetCameraController().GetSpeed();
}

void OvEditor::Core::EditorActions::ResetSceneViewCameraPosition()
{
	EDITOR_PANEL(Panels::SceneView, "Scene View").ResetCameraTransform();
}

void OvEditor::Core::EditorActions::ResetAssetViewCameraPosition()
{
	EDITOR_PANEL(Panels::AssetView, "Asset View").ResetCameraTransform();
}

OvEditor::Core::EditorActions::EEditorMode OvEditor::Core::EditorActions::GetCurrentEditorMode() const
{
	return m_editorMode;
}

void OvEditor::Core::EditorActions::SetEditorMode(EEditorMode p_newEditorMode)
{
	m_editorMode = p_newEditorMode;
	EditorModeChangedEvent.Invoke(m_editorMode);
}

void OvEditor::Core::EditorActions::StartPlaying()
{
	if (m_editorMode == EEditorMode::EDIT)
	{
		m_context.scriptEngine->Reload();
		EDITOR_PANEL(Panels::Inspector, "Inspector").Refresh();

		if (m_context.scriptEngine->IsOk())
		{
			PlayEvent.Invoke();
			m_sceneBackup = std::make_unique<tinyxml2::XMLDocument>();
			tinyxml2::XMLNode* node = m_sceneBackup->NewElement("root");
			m_sceneBackup->InsertFirstChild(node);
			m_context.sceneManager.GetCurrentScene()->OnSerialize(*m_sceneBackup, node);
			m_panelsManager.GetPanelAs<OvEditor::Panels::GameView>("Game View").Focus();
			m_context.sceneManager.GetCurrentScene()->Play();
			SetEditorMode(EEditorMode::PLAY);
		}
	}
	else
	{
		m_context.audioEngine->Unsuspend();
		SetEditorMode(EEditorMode::PLAY);
	}
}

void OvEditor::Core::EditorActions::PauseGame()
{
	SetEditorMode(EEditorMode::PAUSE);
	m_context.audioEngine->Suspend();
}

void OvEditor::Core::EditorActions::StopPlaying()
{
	if (m_editorMode != EEditorMode::EDIT)
	{
		m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
		SetEditorMode(EEditorMode::EDIT);
		bool loadedFromDisk = m_context.sceneManager.IsCurrentSceneLoadedFromDisk();
		std::string sceneSourcePath = m_context.sceneManager.GetCurrentSceneSourcePath();

		int64_t focusedActorID = -1;

		if (auto targetActor = EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor())
			focusedActorID = targetActor->GetID();

		OVASSERT(m_sceneBackup.operator bool(), "No scene backup to restore");

		m_context.sceneManager.LoadSceneFromMemory(*m_sceneBackup);
		if (loadedFromDisk)
			m_context.sceneManager.StoreCurrentSceneSourcePath(sceneSourcePath); // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
		m_sceneBackup.reset();
		EDITOR_PANEL(Panels::SceneView, "Scene View").Focus();
		if (auto actorInstance = m_context.sceneManager.GetCurrentScene()->FindActorByID(focusedActorID))
			EDITOR_PANEL(Panels::Inspector, "Inspector").FocusActor(*actorInstance);
	}
}

void OvEditor::Core::EditorActions::NextFrame()
{
	if (m_editorMode == EEditorMode::PLAY || m_editorMode == EEditorMode::PAUSE)
		SetEditorMode(EEditorMode::FRAME_BY_FRAME);
}

void OvEditor::Core::EditorActions::SetGizmoOperation(OvEditor::Core::EGizmoOperation p_operation)
{
	auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View");
	sceneView.SetGizmoOperation(p_operation);
}

OvEditor::Core::EGizmoOperation OvEditor::Core::EditorActions::GetGizmoOperation() const
{
	auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View");
	return sceneView.GetGizmoOperation();
}

OvMaths::FVector3 OvEditor::Core::EditorActions::CalculateActorSpawnPoint(float p_distanceToCamera)
{
	auto& sceneView = m_panelsManager.GetPanelAs<OvEditor::Panels::SceneView>("Scene View");

	if (auto camera = sceneView.GetCamera())
	{
		return camera->GetPosition() + camera->transform->GetWorldForward() * p_distanceToCamera;
	}

	return OvMaths::FVector3::Zero;
}

OvCore::ECS::Actor & OvEditor::Core::EditorActions::CreateEmptyActor(bool p_focusOnCreation, OvCore::ECS::Actor* p_parent, const std::string& p_name)
{
    const auto currentScene = m_context.sceneManager.GetCurrentScene();
	auto& instance = p_name.empty() ? currentScene->CreateActor() : currentScene->CreateActor(p_name);

	if (p_parent)
		instance.SetParent(*p_parent);

	if (m_actorSpawnMode == EActorSpawnMode::FRONT)
		instance.transform.SetLocalPosition(CalculateActorSpawnPoint(10.0f));

	if (p_focusOnCreation)
		SelectActor(instance);

	OVLOG_INFO("Actor created");

	return instance;
}

OvCore::ECS::Actor & OvEditor::Core::EditorActions::CreateActorWithModel(const std::string& p_path, bool p_focusOnCreation, OvCore::ECS::Actor* p_parent, const std::string& p_name)
{
	auto& instance = CreateEmptyActor(false, p_parent, p_name);

	auto& modelRenderer = instance.AddComponent<OvCore::ECS::Components::CModelRenderer>();

	const auto model = m_context.modelManager[p_path];
	if (model)
	{
		modelRenderer.SetModel(model);
	}

	auto& materialRenderer = instance.AddComponent<OvCore::ECS::Components::CMaterialRenderer>();
	const auto defaultMaterial = m_context.materialManager[std::string{ kDefaultMaterialPath }];
	if (model)
	{
		materialRenderer.FillWithEmbeddedMaterials(true, defaultMaterial);
	}
	else if (defaultMaterial)
	{
		materialRenderer.FillWithMaterial(*defaultMaterial);
	}

	if (p_focusOnCreation)
		SelectActor(instance);

	return instance;
}

bool OvEditor::Core::EditorActions::DestroyActor(OvCore::ECS::Actor & p_actor)
{
	p_actor.MarkAsDestroy();
	OVLOG_INFO("Actor destroyed");
	return true;
}

std::string FindDuplicatedActorUniqueName(OvCore::ECS::Actor& p_duplicated, OvCore::ECS::Actor& p_newActor, OvCore::SceneSystem::Scene& p_scene)
{
    const auto parent = p_newActor.GetParent();
    const auto adjacentActors = parent ? parent->GetChildren() : p_scene.GetActors();

    auto availabilityChecker = [&parent, &adjacentActors](std::string target) -> bool
    {
        const auto isActorNameTaken = [&target, parent](auto actor) { return (parent || !actor->GetParent()) && actor->GetName() == target; };
        return std::find_if(adjacentActors.begin(), adjacentActors.end(), isActorNameTaken) == adjacentActors.end();
    };

    return OvTools::Utils::String::GenerateUnique(p_duplicated.GetName(), availabilityChecker);
}

void OvEditor::Core::EditorActions::DuplicateActor(OvCore::ECS::Actor & p_toDuplicate, OvCore::ECS::Actor* p_forcedParent, bool p_focus)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* actorsRoot = doc.NewElement("actors");
	p_toDuplicate.OnSerialize(doc, actorsRoot);
	auto& newActor = CreateEmptyActor(false);
	int64_t idToUse = newActor.GetID();
	uint64_t guidToUse = newActor.GetGUID();
	tinyxml2::XMLElement* currentActor = actorsRoot->FirstChildElement("actor");
	newActor.OnDeserialize(doc, currentActor);
	
	newActor.SetID(idToUse);
	newActor.SetGUID(guidToUse);
	auto currentScene = m_context.sceneManager.GetCurrentScene();

	if (p_forcedParent)
	{
		newActor.SetParent(*p_forcedParent);
	}
	else if (newActor.GetParentID() > 0)
	{
		if (auto found = currentScene->FindActorByID(newActor.GetParentID()); found)
		{
			newActor.SetParent(*found);
		}
	}

	if (p_focus || !p_forcedParent)
	{
		const auto uniqueName = FindDuplicatedActorUniqueName(p_toDuplicate, newActor, *currentScene);
		newActor.SetName(uniqueName);
	}

	if (p_focus)
	{
		SelectActor(newActor);
	}

	for (auto& child : p_toDuplicate.GetChildren())
		DuplicateActor(*child, &newActor, false);
}

void OvEditor::Core::EditorActions::CopyActor(OvCore::ECS::Actor& p_actor)
{
	m_context.copyBuffer = Context::ActorCopyBuffer{
		.guid = p_actor.GetGUID()
	};
}

void OvEditor::Core::EditorActions::PasteActor(OvCore::ECS::Actor* p_parent)
{
	const auto actorCopyBuffer = std::get_if<Context::ActorCopyBuffer>(&m_context.copyBuffer);
	if (!actorCopyBuffer)
	{
		return;
	}

	const auto currentScene = m_context.sceneManager.GetCurrentScene();
	if (!currentScene)
	{
		return;
	}

	if (const auto copiedActor = currentScene->FindActorByGUID(actorCopyBuffer->guid))
	{
		auto* destinationParent = p_parent;

		// Pasting on the copied actor itself falls back to its current parent,
		// preserving the "duplicate-like" behavior by default.
		if (destinationParent && destinationParent->GetGUID() == copiedActor->GetGUID())
		{
			destinationParent = copiedActor->GetParent();
		}

		DuplicateActor(*copiedActor, destinationParent, true);
	}
}

void OvEditor::Core::EditorActions::SelectActor(OvCore::ECS::Actor& p_target)
{
	EDITOR_PANEL(Panels::Inspector, "Inspector").FocusActor(p_target);
}

void OvEditor::Core::EditorActions::UnselectActor()
{
	EDITOR_PANEL(Panels::Inspector, "Inspector").UnFocus();
}

bool OvEditor::Core::EditorActions::IsAnyActorSelected() const
{
	return EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor().has_value();
}

OvCore::ECS::Actor& OvEditor::Core::EditorActions::GetSelectedActor() const
{
	return EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor().value();
}

void OvEditor::Core::EditorActions::MoveToTarget(OvCore::ECS::Actor& p_target)
{
	EDITOR_PANEL(Panels::SceneView, "Scene View").GetCameraController().MoveToTarget(p_target);
}

void OvEditor::Core::EditorActions::CompileShaders()
{
	for (const auto shader : m_context.shaderManager.GetResources() | std::views::values)
	{
		if (shader)
		{
			CompileShader(shader->path);
		}
	}
}

void OvEditor::Core::EditorActions::CompileShader(OvRendering::Resources::Shader& p_shader)
{
	CompileShader(p_shader.path);
}

void OvEditor::Core::EditorActions::CompileShader(const std::filesystem::path& p_shaderPath)
{
	using namespace OvRendering::Resources::Loaders;

	const auto previousLoggingSettings = ShaderLoader::GetLoggingSettings();
	auto newLoggingSettings = previousLoggingSettings;
	newLoggingSettings.summary = true; // Force enable summary logging
	ShaderLoader::SetLoggingSettings(newLoggingSettings);
	OvRendering::Resources::Shader* compiledShader = nullptr;

	if (m_context.shaderManager.IsResourceRegistered(p_shaderPath))
	{
		compiledShader = m_context.shaderManager[p_shaderPath];

		if (compiledShader)
		{
			m_context.shaderManager.ReloadResource(compiledShader, GetRealPath(compiledShader->path));
		}
	}
	else
	{
		compiledShader = m_context.shaderManager.LoadResource(p_shaderPath);
	}

	ShaderLoader::SetLoggingSettings(previousLoggingSettings);

	if (compiledShader)
	{
		RefreshMaterialsUsingShader(m_context.materialManager, *compiledShader);
		RefreshMaterialEditorIfTargetUsesShader(*compiledShader);
	}
}

void OvEditor::Core::EditorActions::SaveMaterials()
{
	for (const auto material : m_context.materialManager.GetResources() | std::views::values)
	{
		if (!material)
		{
			continue;
		}

		if (OvRendering::Resources::Parsers::ParseEmbeddedAssetPath(material->path))
		{
			continue;
		}

		OvCore::Resources::Loaders::MaterialLoader::Save(*material, GetRealPath(material->path));
	}
}

void OvEditor::Core::EditorActions::RegenerateScriptingProjectFiles()
{
	if (m_context.scriptEngine->CreateProjectFiles(m_context.projectFolder, true))
	{
		OVLOG_INFO("Lua symbol regenerated (.luarc.json created)");
	}
	else
	{
		OVLOG_ERROR("Failed to regenerate lua symbols (.luarc.json failed to create)");
	}
}

bool OvEditor::Core::EditorActions::OpenInCodeEditor(const std::filesystem::path& p_path, OvTools::Utils::OptRef<const std::filesystem::path> p_workdir)
{
	std::string command = OvEditor::Settings::EditorSettings::CodeEditorCommand.Get();
	if (!command.empty())
	{
		auto preferredPath = p_path;
		preferredPath.make_preferred();

		if (command.find("{path}") == std::string::npos)
		{
			OVLOG_ERROR("Failed to open in code editor, missing {path} in custom command.");
			return false;
		}

		OvTools::Utils::String::ReplaceAll(command, "{workdir}", p_workdir ? p_workdir->string() : m_context.projectFolder.string());
		OvTools::Utils::String::ReplaceAll(command, "{path}", p_path.string());
		if (!OvTools::Utils::SystemCalls::ExecuteCommand(command))
		{
			OVLOG_ERROR(std::format("Failed to open in code editor using command: \"{}\"", command));
			return false;
		}
	}
	else
	{
		OVLOG_ERROR("No command provided to open in code editor.");
		return false;
	}

	return true;
}

bool OvEditor::Core::EditorActions::ImportAsset(const std::string& p_initialDestinationDirectory)
{
	using namespace OvWindowing::Dialogs;

	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga;*.hdr;";
	std::string shaderFormats = "*.ovfx;";
	std::string shaderPartFormats = "*.ovfxh;";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";
	std::string scriptFormats = "*.lua;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + shaderPartFormats + soundFormats + scriptFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga, .hdr)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.ovfx)", shaderFormats);
	selectAssetDialog.AddFileType("Shader Parts (.ovfxh)", shaderPartFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.AddFileType("Script (.lua)", scriptFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string extension = '.' + OvTools::Utils::PathParser::GetExtension(source);
		std::string filename = selectAssetDialog.GetSelectedFileName();

		SaveFileDialog saveLocationDialog("Where to import?");
		saveLocationDialog.SetInitialDirectory(p_initialDestinationDirectory);
		saveLocationDialog.DefineExtension(extension, extension);
		saveLocationDialog.Show();

		if (saveLocationDialog.HasSucceeded())
		{
			std::string destination = saveLocationDialog.GetSelectedFilePath();

			if (!std::filesystem::exists(destination) || MessageBox("File already exists", "The destination you have selected already exists, importing this file will erase the previous file content, are you sure about that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::OK_CANCEL).GetUserAction() == MessageBox::EUserAction::OK)
			{
				std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
				OVLOG_INFO("Asset \"" + destination + "\" imported");
				return true;
			}
		}
	}

	return false;
}

bool OvEditor::Core::EditorActions::ImportAssetAtLocation(const std::string& p_destination)
{
	using namespace OvWindowing::Dialogs;

	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga;*.hdr;";
	std::string shaderFormats = "*.ovfx;";
	std::string shaderPartFormats = "*.ovfxh;";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";
	std::string scriptFormats = "*.lua;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + soundFormats + scriptFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga, .hdr)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.ovfx)", shaderFormats);
	selectAssetDialog.AddFileType("Shader Parts (.ovfxh)", shaderPartFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.AddFileType("Script (.lua)", scriptFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string destination = (std::filesystem::path{ p_destination } / selectAssetDialog.GetSelectedFileName()).string();

		if (!std::filesystem::exists(destination) || MessageBox("File already exists", "The destination you have selected already exists, importing this file will erase the previous file content, are you sure about that?", MessageBox::EMessageType::WARNING, MessageBox::EButtonLayout::OK_CANCEL).GetUserAction() == MessageBox::EUserAction::OK)
		{
			std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing);
			OVLOG_INFO("Asset \"" + destination + "\" imported");
			return true;
		}
	}

	return false;
}

// Duplicate from AResourceManager.h
std::string OvEditor::Core::EditorActions::GetRealPath(const std::string& p_path)
{
	std::filesystem::path result;

	const std::string normalizedPath = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_path);

	if (normalizedPath.starts_with(':')) // The path is an engine path
	{
		result = m_context.engineAssetsPath / normalizedPath.substr(1);
	}
	else // The path is a project path
	{
		result = m_context.projectAssetsPath / normalizedPath;
	}

	return result.lexically_normal().string();
}

std::string OvEditor::Core::EditorActions::GetResourcePath(const std::string& p_path, bool p_isFromEngine)
{
	std::string result = p_path;

	if (OvTools::Utils::String::Replace(result, p_isFromEngine ? m_context.engineAssetsPath.string() : m_context.projectAssetsPath.string(), ""))
	{
		if (result.starts_with(std::filesystem::path::preferred_separator))
		{
			result = result.substr(1);
		}

		if (p_isFromEngine)
		{
			result = ':' + result;
		}
	}

	return result;
}

std::string OvEditor::Core::EditorActions::GetScriptPath(const std::string & p_path)
{
	std::string result = p_path;

	OvTools::Utils::String::Replace(result, m_context.projectAssetsPath.string(), "");

	if (result.starts_with(std::filesystem::path::preferred_separator))
	{
		result = result.substr(1);
	}

	// Normalize to forward slashes for cross-platform consistency
	std::replace(result.begin(), result.end(), '\\', '/');

	return result;
}

void OvEditor::Core::EditorActions::PropagateFolderRename(std::string p_previousName, std::string p_newName)
{
	p_previousName = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_previousName);
	p_newName = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_newName);

	for (auto& p : std::filesystem::recursive_directory_iterator(p_newName))
	{
		if (!p.is_directory())
		{
			std::string newFileName = OvTools::Utils::PathParser::MakeNonWindowsStyle(p.path().string());
			std::string previousFileName;

			for (char c : newFileName)
			{
				previousFileName += c;
				if (previousFileName == p_newName)
					previousFileName = p_previousName;
			}

			PropagateFileRename(
				OvTools::Utils::PathParser::MakeWindowsStyle(previousFileName),
				OvTools::Utils::PathParser::MakeWindowsStyle(newFileName)
			);
		}
	}
}

void OvEditor::Core::EditorActions::PropagateFolderDestruction(std::string p_folderPath)
{
	for (auto& p : std::filesystem::recursive_directory_iterator(p_folderPath))
	{
		if (!p.is_directory())
		{
			PropagateFileRename(OvTools::Utils::PathParser::MakeWindowsStyle(p.path().string()), "?");
		}
	}
}

void OvEditor::Core::EditorActions::MigrateScripts()
{
	const auto legacyScriptsPath = m_context.projectFolder / "Scripts";

	if (!std::filesystem::exists(legacyScriptsPath) || !std::filesystem::is_directory(legacyScriptsPath))
	{
		return;
	}

	using namespace OvWindowing::Dialogs;

	MessageBox message(
		"Legacy Scripts/ folder detected",
		"A \"Scripts/\" folder was found in your project directory.\n\n"
		"Scripts are now stored inside \"Assets/\" and support subdirectories.\n\n"
		"Would you like to migrate your scripts to \"Assets/Scripts/\"?\n"
		"All scene files referencing these scripts will be updated automatically.\n\n"
		"Note that any script already present in Assets/Scripts/ with the same name as a script in Scripts/ will be overridden.",
		MessageBox::EMessageType::WARNING,
		MessageBox::EButtonLayout::YES_NO,
		true
	);

	if (message.GetUserAction() != MessageBox::EUserAction::YES)
	{
		return;
	}

	const auto targetPath = m_context.projectAssetsPath / "Scripts";
	std::vector<std::filesystem::path> migratedScriptNames;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(legacyScriptsPath))
	{
		if (!entry.is_directory())
		{
			if (OvTools::Utils::PathParser::GetFileType(entry.path().string()) == OvTools::Utils::PathParser::EFileType::SCRIPT)
			{
				migratedScriptNames.push_back(entry.path().filename());
			}
		}
	}

	std::error_code err;

	if (!std::filesystem::exists(targetPath))
	{
		std::filesystem::rename(legacyScriptsPath, targetPath, err);
	}
	else if (std::filesystem::is_directory(targetPath))
	{
		std::filesystem::create_directories(targetPath, err);

		if (!err)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(legacyScriptsPath))
			{
				const auto destination = targetPath / entry.path().lexically_relative(legacyScriptsPath);

				if (entry.is_directory())
				{
					std::filesystem::create_directories(destination, err);
				}
				else
				{
					std::filesystem::create_directories(destination.parent_path(), err);

					if (!err)
					{
						std::filesystem::copy_file(entry.path(), destination, std::filesystem::copy_options::overwrite_existing, err);
					}
				}

				if (err)
				{
					break;
				}
			}
		}

		if (!err)
		{
			std::filesystem::remove_all(legacyScriptsPath, err);
		}
	}
	else
	{
		OVLOG_ERROR("Failed to migrate Scripts/ folder: Assets/Scripts exists but is not a directory.");
		return;
	}

	if (err)
	{
		OVLOG_ERROR("Failed to migrate Scripts/ folder: " + err.message());
		return;
	}

	OVLOG_INFO("Scripts/ folder migrated to Assets/Scripts/");

	// Update all scene files: replace old behaviour type (just the stem) with the new relative path
	for (const auto& scriptName : migratedScriptNames)
	{
		const auto stem = scriptName.stem().string();
		const auto newRelPath = (std::filesystem::path("Scripts") / scriptName).generic_string();

		PropagateFileRenameThroughSavedFilesOfType(stem, newRelPath, OvTools::Utils::PathParser::EFileType::SCENE);
	}

	OVLOG_INFO("Scene files updated with new script paths");
}

void OvEditor::Core::EditorActions::PropagateFileRename(std::string p_previousName, std::string p_newName)
{
	p_previousName = GetResourcePath(p_previousName);
	p_newName = GetResourcePath(p_newName);

	if (p_newName != "?")
	{
		/* If not a real rename is asked (Not delete) */

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().MoveResource(p_previousName, p_newName))
		{
			OvRendering::Resources::Model* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>()[p_newName];
			const_cast<std::string&>(resource->path) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().MoveResource(p_previousName, p_newName))
		{
			OvRendering::Resources::Texture* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>()[p_newName];
			const_cast<std::string&>(resource->path) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>().MoveResource(p_previousName, p_newName))
		{
			OvRendering::Resources::Shader* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[p_newName];
			const_cast<std::string&>(resource->path) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().MoveResource(p_previousName, p_newName))
		{
			OvCore::Resources::Material* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>()[p_newName];
			const_cast<std::string&>(resource->path) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>().MoveResource(p_previousName, p_newName))
		{
			OvAudio::Resources::Sound* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>()[p_newName];
			const_cast<std::string&>(resource->path) = p_newName;
		}

		if (OvTools::Utils::PathParser::GetFileType(p_previousName) == OvTools::Utils::PathParser::EFileType::MODEL)
		{
			MoveAllEmbeddedResourcesForRenamedModel(p_previousName, p_newName);
		}
	}
	else
	{
		if (auto texture = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResource(p_previousName, false))
		{
			for (auto [name, instance] : OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResources())
			{
				if (instance)
				{
					for (auto& [name, prop] : instance->GetProperties())
					{
						if (const auto pval = std::get_if<OvRendering::Resources::Texture*>(&prop.value); pval && *pval)
						{
							if (*pval == texture)
							{
								prop.value = static_cast<OvRendering::Resources::Texture*>(nullptr);
							}
						}
					}
				}
			}

			auto& assetView = EDITOR_PANEL(Panels::AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<OvRendering::Resources::Texture*>(&assetViewRes); pval && *pval)
				assetView.ClearResource();

			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().UnloadResource(p_previousName);
		}
		
		if (auto shader = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>().GetResource(p_previousName, false))
		{
			auto& materialEditor = EDITOR_PANEL(Panels::MaterialEditor, "Material Editor");

			for (auto[name, instance] : OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResources())
				if (instance && instance->GetShader() == shader)
					instance->SetShader(nullptr);

			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>().UnloadResource(p_previousName);
		}

		if (auto model = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().GetResource(p_previousName, false))
		{
			auto& assetView = EDITOR_PANEL(Panels::AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<OvRendering::Resources::Model*>(&assetViewRes); pval && *pval)
				assetView.ClearResource();

			if (auto currentScene = m_context.sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto modelRenderer = actor->GetComponent<OvCore::ECS::Components::CModelRenderer>(); modelRenderer && modelRenderer->GetModel() == model)
						modelRenderer->SetModel(nullptr);

			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().UnloadResource(p_previousName);
		}

		if (auto material = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResource(p_previousName, false))
		{
			auto& materialEditor = EDITOR_PANEL(Panels::MaterialEditor, "Material Editor");
			
			if (materialEditor.GetTarget() == material)
				materialEditor.RemoveTarget();

			auto& assetView = EDITOR_PANEL(Panels::AssetView, "Asset View");
			auto assetViewRes = assetView.GetResource();
			if (auto pval = std::get_if<OvCore::Resources::Material*>(&assetViewRes); pval && *pval)
				assetView.ClearResource();

			if (auto currentScene = m_context.sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto materialRenderer = actor->GetComponent<OvCore::ECS::Components::CMaterialRenderer>(); materialRenderer)
						materialRenderer->RemoveMaterialByInstance(*material);

			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().UnloadResource(p_previousName);
		}

		if (auto sound = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>().GetResource(p_previousName, false))
		{
			if (auto currentScene = m_context.sceneManager.GetCurrentScene())
				for (auto actor : currentScene->GetActors())
					if (auto audioSource = actor->GetComponent<OvCore::ECS::Components::CAudioSource>(); audioSource && audioSource->GetSound() == sound)
						audioSource->SetSound(nullptr);

			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>().UnloadResource(p_previousName);
		}
	}

	switch (OvTools::Utils::PathParser::GetFileType(p_previousName))
	{
	case OvTools::Utils::PathParser::EFileType::SCRIPT:
	{
		// Normalize to forward slashes (Behaviour::name uses forward slashes as path separator)
		std::string prev = p_previousName;
		std::string next = p_newName;
		std::replace(prev.begin(), prev.end(), '\\', '/');
		if (next != "?") std::replace(next.begin(), next.end(), '\\', '/');

		if (auto currentScene = m_context.sceneManager.GetCurrentScene())
		{
			for (auto actor : currentScene->GetActors())
			{
				if (next != "?")
					actor->RenameBehaviour(prev, next);
				else
					actor->RemoveBehaviour(prev);
			}
		}

		if (next != "?")
		{
			PropagateFileRenameThroughSavedFilesOfType(prev, next, OvTools::Utils::PathParser::EFileType::SCENE);
		}

		EDITOR_PANEL(Panels::Inspector, "Inspector").Refresh();
		break;
	}
	case OvTools::Utils::PathParser::EFileType::MATERIAL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);
		break;
	case OvTools::Utils::PathParser::EFileType::MODEL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::MATERIAL);
		break;
	case OvTools::Utils::PathParser::EFileType::SHADER:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::MATERIAL);
		break;
	case OvTools::Utils::PathParser::EFileType::TEXTURE:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::MATERIAL);
		break;
	case OvTools::Utils::PathParser::EFileType::SOUND:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);
		break;
	default:
		break;
	}

	EDITOR_PANEL(Panels::Inspector, "Inspector").Refresh();
	EDITOR_PANEL(Panels::MaterialEditor, "Material Editor").Refresh();
}

uint64_t ReplaceStringInFile(const std::filesystem::path& p_filePath,
	const std::string_view p_searchStr,
	const std::string_view p_replaceStr
)
{
	uint64_t occurences = 0;

	if (!std::filesystem::exists(p_filePath))
	{
		throw std::runtime_error("File does not exist: " + p_filePath.string());
	}

	std::string content;

	if (auto inFile = std::ifstream{ p_filePath, std::ios::in })
	{
		std::stringstream buffer;
		buffer << inFile.rdbuf();
		content = buffer.str();
	}
	else
	{
		throw std::runtime_error("Cannot open file for reading: " + p_filePath.string());
	}
	 
	size_t pos = 0;

	while ((pos = content.find(p_searchStr, pos)) != std::string::npos)
	{
		content.replace(pos, p_searchStr.length(), p_replaceStr);
		pos += p_replaceStr.length();
		++occurences;
	}

	if (occurences > 0)
	{
		if (auto outFile = std::ofstream{ p_filePath, std::ios::out | std::ios::trunc })
		{
			outFile << content;
		}
		else
		{
			throw std::runtime_error("Cannot open file for writing: " + p_filePath.string());
		}
	}

	return occurences;
}

void OvEditor::Core::EditorActions::PropagateFileRenameThroughSavedFilesOfType(
	const std::string& p_previousName,
	const std::string& p_newName,
	OvTools::Utils::PathParser::EFileType p_fileType
)
{
	const bool renameEmbeddedAssets =
		p_newName != "?" &&
		OvTools::Utils::PathParser::GetFileType(p_previousName) == OvTools::Utils::PathParser::EFileType::MODEL;

	const auto replaceFrom = std::string{ ">" + p_previousName + "<" };
	const auto replaceTo = std::string{ ">" + p_newName + "<" };
	const auto embeddedReplaceFrom = std::string{ ">" + p_previousName + ":" };
	const auto embeddedReplaceTo = std::string{ ">" + p_newName + ":" };

	for (const auto& entry : std::filesystem::recursive_directory_iterator(m_context.projectAssetsPath))
	{
		const std::filesystem::path entryPath = entry.path();

		if (OvTools::Utils::PathParser::GetFileType(entryPath.string()) == p_fileType)
		{
			try
			{
				uint64_t occurences = ReplaceStringInFile(entryPath, replaceFrom, replaceTo);

				if (renameEmbeddedAssets)
				{
					occurences += ReplaceStringInFile(entryPath, embeddedReplaceFrom, embeddedReplaceTo);
				}

				if (occurences > 0)
				{
					OVLOG_INFO("Asset retargeting: \"" + p_previousName + "\" to \"" + p_newName + "\" in \"" + entryPath.string() + "\"");
				}
			}
			catch (const std::exception& e)
			{
				const auto errorMessage = std::string{ e.what() };
				OVLOG_ERROR("Asset retargeting failed: " + errorMessage);
			}
		}
	}
}
