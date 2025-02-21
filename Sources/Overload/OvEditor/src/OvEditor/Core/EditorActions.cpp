/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <iostream>
#include <fstream>

#include <OvDebug/Logger.h>

#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CAudioSource.h>

#include <OvWindowing/Dialogs/OpenFileDialog.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>
#include <OvWindowing/Dialogs/MessageBox.h>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/String.h>
#include <OvTools/Utils/SystemCalls.h>

#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/SceneView.h"
#include "OvEditor/Panels/AssetView.h"
#include "OvEditor/Panels/GameView.h"
#include "OvEditor/Panels/Inspector.h"
#include "OvEditor/Panels/ProjectSettings.h"
#include "OvEditor/Panels/MaterialEditor.h"

OvEditor::Core::EditorActions::EditorActions(Context& p_context, PanelsManager& p_panelsManager) :
	m_context(p_context), 
	m_panelsManager(p_panelsManager)
{
	OvCore::Global::ServiceLocator::Provide<OvEditor::Core::EditorActions>(*this);

	m_context.sceneManager.CurrentSceneSourcePathChangedEvent += [this](const std::string& p_newPath)
	{
		std::string titleExtra = " - " + (p_newPath.empty() ? "Untitled Scene" : GetResourcePath(p_newPath));
		m_context.window->SetTitle(m_context.windowSettings.title + titleExtra);
	};
}

void OvEditor::Core::EditorActions::LoadEmptyScene()
{
	if (GetCurrentEditorMode() != EEditorMode::EDIT)
		StopPlaying();

	m_context.sceneManager.LoadEmptyScene();
	auto scene = m_context.sceneManager.GetCurrentScene();
	scene->AddDefaultCamera();
	scene->AddDefaultLights();

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
		StopPlaying();

	m_context.sceneManager.LoadScene(p_path, p_absolute);
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
	dialog.SetInitialDirectory(m_context.projectAssetsPath + "New Scene");
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
			case OvWindowing::Dialogs::MessageBox::EUserAction::NO: return;
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
		std::string result = dialog.GetSelectedFilePath();
		result = std::string(result.data(), result.data() + result.size() - std::string("..").size()) + "\\"; // remove auto extension
		if (!std::filesystem::exists(result))
			return result;
		else
		{
			OvWindowing::Dialogs::MessageBox message("Folder already exists!", "The folder \"" + result + "\" already exists.\n\nPlease select another location and try again", OvWindowing::Dialogs::MessageBox::EMessageType::WARNING, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
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
	std::string destinationFolder;

	if (p_tempFolder)
	{
		destinationFolder = OvTools::Utils::SystemCalls::GetPathToAppdata() + "\\OverloadTech\\OvEditor\\TempBuild\\";
		try
		{
			std::filesystem::remove_all(destinationFolder);
		}
		catch (std::filesystem::filesystem_error error)
		{
			OvWindowing::Dialogs::MessageBox message("Temporary build failed", "The temporary folder is currently being used by another process", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
			return;
		}
	}
	else if (auto res = SelectBuildFolder(); res.has_value())
		destinationFolder = res.value();
	else
		return; // Operation cancelled (No folder selected)

	BuildAtLocation(m_context.projectSettings.Get<bool>("dev_build") ? "Development" : "Shipping", destinationFolder, p_autoRun);
}

void OvEditor::Core::EditorActions::BuildAtLocation(const std::string & p_configuration, const std::string p_buildPath, bool p_autoRun)
{
	std::string buildPath(p_buildPath);
	std::string executableName = m_context.projectSettings.Get<std::string>("executable_name") + ".exe";

	bool failed = false;

	OVLOG_INFO("Preparing to build at location: \"" + buildPath + "\"");

	std::filesystem::remove_all(buildPath);

	if (std::filesystem::create_directory(buildPath))
	{
		OVLOG_INFO("Build directory created");

		if (std::filesystem::create_directory(buildPath + "Data\\"))
		{
			OVLOG_INFO("Data directory created");

			if (std::filesystem::create_directory(buildPath + "Data\\User\\"))
			{
				OVLOG_INFO("Data\\User directory created");

				std::error_code err;

				std::filesystem::copy(m_context.projectFilePath, buildPath + "Data\\User\\Game.ini", err);

				if (!err)
				{
					OVLOG_INFO("Data\\User\\Game.ini file generated");
		
					std::filesystem::copy(m_context.projectAssetsPath, buildPath + "Data\\User\\Assets\\", std::filesystem::copy_options::recursive, err);

					if (!std::filesystem::exists(buildPath + "Data\\User\\Assets\\" + (m_context.projectSettings.Get<std::string>("start_scene"))))
					{
						OVLOG_ERROR("Failed to find Start Scene at expected path. Verify your Project Setings.");
						OvWindowing::Dialogs::MessageBox message("Build Failure", "An error occured during the building of your game.\nCheck the console for more information", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
						std::filesystem::remove_all(buildPath);
						return;						
					}

					if (!err)
					{
						OVLOG_INFO("Data\\User\\Assets\\ directory copied");

						std::filesystem::copy(m_context.projectScriptsPath, buildPath + "Data\\User\\Scripts\\", std::filesystem::copy_options::recursive, err);

						if (!err)
						{
							OVLOG_INFO("Data\\User\\Scripts\\ directory copied");

							std::filesystem::copy(m_context.engineAssetsPath, buildPath + "Data\\Engine\\", std::filesystem::copy_options::recursive, err);

							if (!err)
							{
								OVLOG_INFO("Data\\Engine\\ directory copied");
							}
							else
							{
								OVLOG_ERROR("Data\\Engine\\ directory failed to copy");
								failed = true;
							}
						}
						else
						{
							OVLOG_ERROR("Data\\User\\Scripts\\ directory failed to copy");
							failed = true;
						}
					}
					else
					{
						OVLOG_ERROR("Data\\User\\Assets\\ directory failed to copy");
						failed = true;
					}
				}
				else
				{
					OVLOG_ERROR("Data\\User\\Game.ini file failed to generate");
					failed = true;
				}

				std::string builderFolder = "Builder\\" + p_configuration + "\\";

				if (std::filesystem::exists(builderFolder))
				{
					std::error_code err;

					std::filesystem::copy(builderFolder, buildPath, err);

					if (!err)
					{
						OVLOG_INFO("Builder data (Dlls and executatble) copied");

						std::filesystem::rename(buildPath + "OvGame.exe", buildPath + executableName, err);

						if (!err)
						{
							OVLOG_INFO("Game executable renamed to " + executableName);

							if (p_autoRun)
							{
								std::string exePath = buildPath + executableName;
								OVLOG_INFO("Launching the game at location: \"" + exePath + "\"");
								if (std::filesystem::exists(exePath))
									OvTools::Utils::SystemCalls::OpenFile(exePath, buildPath);
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
					const std::string buildConfiguration = p_configuration == "Development" ? "Debug" : "Release";
					OVLOG_ERROR("Builder folder for \"" + p_configuration + "\" not found. Verify you have compiled Engine source code in '" + buildConfiguration + "' configuration.");
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
		std::filesystem::remove_all(buildPath);
		OvWindowing::Dialogs::MessageBox message("Build Failure", "An error occured during the building of your game.\nCheck the console for more information", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
	}
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
    DelayAction([this]() {m_context.uiManager->ResetLayout("Config\\layout.ini"); });
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
			m_sceneBackup.Clear();
			tinyxml2::XMLNode* node = m_sceneBackup.NewElement("root");
			m_sceneBackup.InsertFirstChild(node);
			m_context.sceneManager.GetCurrentScene()->OnSerialize(m_sceneBackup, node);
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
		ImGui::GetIO().DisableMouseUpdate = false;
		m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
		SetEditorMode(EEditorMode::EDIT);
		bool loadedFromDisk = m_context.sceneManager.IsCurrentSceneLoadedFromDisk();
		std::string sceneSourcePath = m_context.sceneManager.GetCurrentSceneSourcePath();

		int64_t focusedActorID = -1;

		if (auto targetActor = EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor())
			focusedActorID = targetActor->GetID();

		m_context.sceneManager.LoadSceneFromMemory(m_sceneBackup);
		if (loadedFromDisk)
			m_context.sceneManager.StoreCurrentSceneSourcePath(sceneSourcePath); // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
		m_sceneBackup.Clear();
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
		modelRenderer.SetModel(model);

	auto& materialRenderer = instance.AddComponent<OvCore::ECS::Components::CMaterialRenderer>();
    const auto material = m_context.materialManager[":Materials\\Default.ovmat"];
	if (material)
		materialRenderer.FillWithMaterial(*material);

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
	tinyxml2::XMLElement* currentActor = actorsRoot->FirstChildElement("actor");
	newActor.OnDeserialize(doc, currentActor);
	
	newActor.SetID(idToUse);

	if (p_forcedParent)
		newActor.SetParent(*p_forcedParent);
	else
	{
        auto currentScene = m_context.sceneManager.GetCurrentScene();

        if (newActor.GetParentID() > 0)
        {
            if (auto found = currentScene->FindActorByID(newActor.GetParentID()); found)
            {
                newActor.SetParent(*found);
            }
        }

        const auto uniqueName = FindDuplicatedActorUniqueName(p_toDuplicate, newActor, *currentScene);
        newActor.SetName(uniqueName);
	}

	if (p_focus)
		SelectActor(newActor);

	for (auto& child : p_toDuplicate.GetChildren())
		DuplicateActor(*child, &newActor, false);
}

void OvEditor::Core::EditorActions::SelectActor(OvCore::ECS::Actor & p_target)
{
	EDITOR_PANEL(Panels::Inspector, "Inspector").FocusActor(p_target);
}

void OvEditor::Core::EditorActions::UnselectActor()
{
	EDITOR_PANEL(Panels::Inspector, "Inspector").UnFocus();
}

bool OvEditor::Core::EditorActions::IsAnyActorSelected() const
{
	return EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor();
}

OvCore::ECS::Actor & OvEditor::Core::EditorActions::GetSelectedActor() const
{
	return *EDITOR_PANEL(Panels::Inspector, "Inspector").GetTargetActor();
}

void OvEditor::Core::EditorActions::MoveToTarget(OvCore::ECS::Actor& p_target)
{
	EDITOR_PANEL(Panels::SceneView, "Scene View").GetCameraController().MoveToTarget(p_target);
}

void OvEditor::Core::EditorActions::CompileShaders()
{
	for (auto shader : m_context.shaderManager.GetResources())
		OvRendering::Resources::Loaders::ShaderLoader::Recompile(*shader.second, GetRealPath(shader.second->path));
}

void OvEditor::Core::EditorActions::SaveMaterials()
{
	for (auto& [id, material] : m_context.materialManager.GetResources())
		OvCore::Resources::Loaders::MaterialLoader::Save(*material, GetRealPath(material->path));
}

bool OvEditor::Core::EditorActions::ImportAsset(const std::string& p_initialDestinationDirectory)
{
	using namespace OvWindowing::Dialogs;

	std::string modelFormats = "*.fbx;*.obj;";
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga";
	std::string shaderFormats = "*.ovfx";
	std::string shaderPartFormats = "*.ovfxh";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + soundFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.ovfx)", shaderFormats);
	selectAssetDialog.AddFileType("Shader Parts (.ovfxh)", shaderPartFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string extension = '.' + OvTools::Utils::PathParser::GetExtension(source);
		std::string filename = selectAssetDialog.GetSelectedFileName();

		SaveFileDialog saveLocationDialog("Where to import?");
		saveLocationDialog.SetInitialDirectory(p_initialDestinationDirectory + filename);
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
	std::string textureFormats = "*.png;*.jpeg;*.jpg;*.tga;";
	std::string shaderFormats = "*.ovfx";
	std::string shaderPartFormats = "*.ovfxh";
	std::string soundFormats = "*.mp3;*.ogg;*.wav;";

	OpenFileDialog selectAssetDialog("Select an asset to import");
	selectAssetDialog.AddFileType("Any supported format", modelFormats + textureFormats + shaderFormats + soundFormats);
	selectAssetDialog.AddFileType("Model (.fbx, .obj)", modelFormats);
	selectAssetDialog.AddFileType("Texture (.png, .jpeg, .jpg, .tga)", textureFormats);
	selectAssetDialog.AddFileType("Shader (.ovfx)", shaderFormats);
	selectAssetDialog.AddFileType("Shader Parts (.ovfxh)", shaderPartFormats);
	selectAssetDialog.AddFileType("Sound (.mp3, .ogg, .wav)", soundFormats);
	selectAssetDialog.Show();

	if (selectAssetDialog.HasSucceeded())
	{
		std::string source = selectAssetDialog.GetSelectedFilePath();
		std::string destination = p_destination + selectAssetDialog.GetSelectedFileName();

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
std::string OvEditor::Core::EditorActions::GetRealPath(const std::string & p_path)
{
	std::string result;

	if (p_path[0] == ':') // The path is an engine path
	{
		result = m_context.engineAssetsPath + std::string(p_path.data() + 1, p_path.data() + p_path.size());
	}
	else // The path is a project path
	{
		result = m_context.projectAssetsPath + p_path;
	}

	return result;
}

std::string OvEditor::Core::EditorActions::GetResourcePath(const std::string & p_path, bool p_isFromEngine)
{
	std::string result = p_path;

	if (OvTools::Utils::String::Replace(result, p_isFromEngine ? m_context.engineAssetsPath : m_context.projectAssetsPath, ""))
	{
		if (p_isFromEngine)
			result = ':' + result;
	}

	return result;
}

std::string OvEditor::Core::EditorActions::GetScriptPath(const std::string & p_path)
{
	std::string result = p_path;

	OvTools::Utils::String::Replace(result, m_context.projectScriptsPath, "");

	for (auto& extension : OVSERVICE(OvCore::Scripting::ScriptEngine).GetValidExtensions())
	{
		if (result.ends_with(extension))
		{
			result = result.substr(0, result.size() - extension.size());
		}
	}

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

			PropagateFileRename(OvTools::Utils::PathParser::MakeWindowsStyle(previousFileName), OvTools::Utils::PathParser::MakeWindowsStyle(newFileName));
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

void OvEditor::Core::EditorActions::PropagateScriptRename(std::string p_previousName, std::string p_newName)
{
	p_previousName = GetScriptPath(p_previousName);
	p_newName = GetScriptPath(p_newName);

	if (auto currentScene = m_context.sceneManager.GetCurrentScene())
		for (auto actor : currentScene->GetActors())
			if (actor->RemoveBehaviour(p_previousName))
				actor->AddBehaviour(p_newName);

	PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);

	EDITOR_PANEL(Panels::Inspector, "Inspector").Refresh();
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
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvRendering::Resources::Model, path)) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().MoveResource(p_previousName, p_newName))
		{
			OvRendering::Resources::Texture* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvRendering::Resources::Texture, path)) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>().MoveResource(p_previousName, p_newName))
		{
			OvRendering::Resources::Shader* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvRendering::Resources::Shader, path)) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().MoveResource(p_previousName, p_newName))
		{
			OvCore::Resources::Material* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvCore::Resources::Material, path)) = p_newName;
		}

		if (OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>().MoveResource(p_previousName, p_newName))
		{
			OvAudio::Resources::Sound* resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>()[p_newName];
			*reinterpret_cast<std::string*>(reinterpret_cast<char*>(resource) + offsetof(OvAudio::Resources::Sound, path)) = p_newName;
		}
	}
	else
	{
		if (auto texture = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResource(p_previousName, false))
		{
			for (auto[name, instance] : OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResources())
				if (instance)
					for (auto&[name, prop] : instance->GetProperties())
						if (prop.value.has_value() && prop.value.type() == typeid(OvRendering::Resources::Texture*))
							if (std::any_cast<OvRendering::Resources::Texture*>(prop.value) == texture)
								prop.value = static_cast<OvRendering::Resources::Texture*>(nullptr);

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
	case OvTools::Utils::PathParser::EFileType::MATERIAL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);
		break;
	case OvTools::Utils::PathParser::EFileType::MODEL:
		PropagateFileRenameThroughSavedFilesOfType(p_previousName, p_newName, OvTools::Utils::PathParser::EFileType::SCENE);
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
	const auto replaceFrom = std::string{ ">" + p_previousName + "<" };
	const auto replaceTo = std::string{ ">" + p_newName + "<" };

	for (const auto& entry : std::filesystem::recursive_directory_iterator(m_context.projectAssetsPath))
	{
		const std::filesystem::path entryPath = entry.path();

		if (OvTools::Utils::PathParser::GetFileType(entryPath.string()) == p_fileType)
		{
			try
			{
				const uint64_t occurences = ReplaceStringInFile(entryPath, replaceFrom, replaceTo);

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
