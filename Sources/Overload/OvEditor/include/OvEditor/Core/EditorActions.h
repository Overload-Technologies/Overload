/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Global/ServiceLocator.h>
#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Utils/PathParser.h>

#include "OvEditor/Core/Context.h"
#include "OvEditor/Core/PanelsManager.h"

#define EDITOR_EXEC(action)					OvCore::Global::ServiceLocator::Get<OvEditor::Core::EditorActions>().action
#define EDITOR_BIND(method, ...)			std::bind(&OvEditor::Core::EditorActions::method, &OvCore::Global::ServiceLocator::Get<OvEditor::Core::EditorActions>(), ##__VA_ARGS__)
#define EDITOR_EVENT(target)				OvCore::Global::ServiceLocator::Get<OvEditor::Core::EditorActions>().target
#define EDITOR_CONTEXT(instance)			OvCore::Global::ServiceLocator::Get<OvEditor::Core::EditorActions>().GetContext().instance
#define EDITOR_PANEL(type, id)				OvCore::Global::ServiceLocator::Get<OvEditor::Core::EditorActions>().GetPanelsManager().GetPanelAs<type>(id)

namespace OvEditor::Core
{
	/**
	* A set of editor actions
	*/
	class EditorActions
	{
	public:
		/**
		* Constructor
		* @param p_context
		* @param p_panelsManager
		*/
		EditorActions(Context& p_context, PanelsManager& p_panelsManager);

		#pragma region TOOLS
		/**
		* Returns the context
		*/
		Context& GetContext();

		/**
		* Returns the panels manager
		*/
		PanelsManager& GetPanelsManager();
		#pragma endregion

		#pragma region SETTINGS
		/**
		* A simple enumeration that define two actor spawn modes
		*/
		enum class EActorSpawnMode { ORIGIN, FRONT };

		/**
		* Defines if new actors should be spawned at origin
		* @param p_value
		*/
		void SetActorSpawnAtOrigin(bool p_value);

		/**
		* Defines how new actors should be spawned
		* @param p_value
		*/
		void SetActorSpawnMode(EActorSpawnMode p_value);

		/**
		* Reset the editor layout
		*/
		void ResetLayout();

		/**
		* Save the editor layout to the given configuration file
		* @param p_filePath
		*/
		void SaveLayout(const std::filesystem::path& p_filePath);

		/**
		 * Save the current editor layout to the last used configuration file
		 */
		void SaveCurrentLayout();

		/**
		 * Set and load the editor layout from the given configuration file
		 * @param p_filePath
		 */
		void SetLayout(const std::filesystem::path& p_filePath);

		/**
		* Defines the scene view camera speed
		* @param p_speed
		*/
		void SetSceneViewCameraSpeed(int p_speed);

		/**
		* Returns the scene view camera speed
		*/
		int GetSceneViewCameraSpeed();

		/**
		* Defines the asset view camera speed
		* @param p_speed
		*/
		void SetAssetViewCameraSpeed(int p_speed);

		/**
		* Returns the asset view camera speed
		*/
		int GetAssetViewCameraSpeed();

		/**
		* Resets the scene view camera position to the default one
		*/
		void ResetSceneViewCameraPosition();

		/**
		* Resets the scene view camera position to the default one
		*/
		void ResetAssetViewCameraPosition();
		#pragma endregion

		#pragma region GAME
		/**
		* Defines some states for the editor
		*/
		enum class EEditorMode { EDIT, PLAY, PAUSE, FRAME_BY_FRAME };

		/**
		* Returns the current editor state/mode
		*/
		EEditorMode GetCurrentEditorMode() const;

		/**
		* Defines the editor state/mode
		* @param p_newEditorMode
		*/
		void SetEditorMode(EEditorMode p_newEditorMode);

		/**
		* Start playing the current scene and update the editor mode
		*/
		void StartPlaying();

		/**
		* Pause the current playing scene and update the editor mode
		*/
		void PauseGame();

		/**
		* Stop platying the current scene and update the editor mode
		*/
		void StopPlaying();

		/**
		* Play the current frame and pause the editor
		*/
		void NextFrame();
		#pragma endregion

		#pragma region ACTOR_CREATION_DESTRUCTION
		/**
		* Create an actor with the given component type
		* @param p_focusOnCreation
		* @param p_parent
		*/
		template<typename T> OvCore::ECS::Actor& CreateMonoComponentActor(bool p_focusOnCreation = true, OvCore::ECS::Actor* p_parent = nullptr);

		/**
		* Calculate the position where to spawn the actor using the current camera position and forward
		* @param p_distanceToCamera
		*/
		OvMaths::FVector3 CalculateActorSpawnPoint(float p_distanceToCamera);

		/**
		* Create an empty actor
		* @param p_focusOnCreation
		* @param p_parent
        * @param p_name
		*/
		OvCore::ECS::Actor&	CreateEmptyActor(bool p_focusOnCreation = true, OvCore::ECS::Actor* p_parent = nullptr, const std::string& p_name = "");

		/**
		* Create an actor with a model renderer and a material renderer. The model renderer with use the model identified
		* by the given path
		* @param p_path
		* @param p_focusOnCreation
		* @param p_parent
        * @param p_name
		*/
		OvCore::ECS::Actor&	CreateActorWithModel(const std::string& p_path, bool p_focusOnCreation = true, OvCore::ECS::Actor* p_parent = nullptr, const std::string& p_name = "");

		/**
		* Destroy an actor from his scene
		* @param p_focusOnCreation
		* @param p_parent
		*/
		bool DestroyActor(OvCore::ECS::Actor& p_actor);

		/**
		* Duplicate an actor
		* @param p_toDuplicate
		* @param p_forcedParent
		* @param bool
		*/
		void DuplicateActor(OvCore::ECS::Actor& p_toDuplicate, OvCore::ECS::Actor* p_forcedParent = nullptr, bool p_focus = true);
		#pragma endregion

		#pragma region ACTOR_MANIPULATION
		/**
		* Select an actor and show him in inspector
		* @param p_target
		*/
		void SelectActor(OvCore::ECS::Actor& p_target);

		/**
		* Unselect any selected actor and clearing the inspector
		*/
		void UnselectActor();

		/**
		* Returns true if any actor is selected
		*/
		bool IsAnyActorSelected() const;

		/**
		* Returns the selected actor. Make sur you verified that an actor is selected
		* with IsAnyActorSelected() before calling this method
		*/
		OvCore::ECS::Actor&		GetSelectedActor() const;

		/**
		* Moves the camera to the target actor
		*/
		void					MoveToTarget(OvCore::ECS::Actor& p_target);
		#pragma endregion

		#pragma region RESOURCE_MANAGEMENT
		/**
		* Compile every loaded shaders
		*/
		void CompileShaders();

		/**
		* Save every materials to their respective files
		*/
		void SaveMaterials();
		
		/**
		* Import an asset
		* @param p_initialDestinationDirectory
		*/
		bool ImportAsset(const std::string& p_initialDestinationDirectory);

		/**
		* Import an asset at location
		* @param p_destination
		*/
		bool ImportAssetAtLocation(const std::string& p_destination);

		/**
		* Returns the real path of a resource (complete absolute path)
		* @param p_path
		*/
		std::string GetRealPath(const std::string& p_path);

		/**
		* Returns the resource path of a file
		* @param p_path
		* @param p_isFromEngine
		*/
		std::string GetResourcePath(const std::string& p_path, bool p_isFromEngine = false);

		/**
		* Returns the script path of a file
		* @param p_path
		*/
		std::string GetScriptPath(const std::string& p_path);

		/**
		* Propagate the folder rename everywhere (Resource manager, scenes, materials...)
		* @param p_previousName
		* @param p_newName
		*/
		void PropagateFolderRename(std::string p_previousName, const std::string p_newName);

		/**
		* Propagate the folder destruction everywhere (Resource manager, scenes, materials...)
		* @param p_folderPath
		*/
		void PropagateFolderDestruction(std::string p_folderPath);

		/**
		* Propagate the script rename in scenes and inspector
		* @param p_previousName
		* @param p_newName
		*/
		void PropagateScriptRename(std::string p_previousName, std::string p_newName);

		/**
		* Propagate the file rename everywhere it is used
		* @param p_previousName
		* @param p_newName
		*/
		void PropagateFileRename(std::string p_previousName, std::string p_newName);

		/**
		* Propagate the file rename through concerned files
		* @param p_previousName
		* @param p_newName
		* @param p_fileType
		*/
		void PropagateFileRenameThroughSavedFilesOfType(const std::string& p_previousName, const std::string& p_newName, OvTools::Utils::PathParser::EFileType p_fileType);
		#pragma endregion

		#pragma region SCENE
		/**
		* Load an empty scene. Any unsaved changes to the current scene will be discarded
		*/
		void LoadEmptyScene();

		/**
		* Save the current scene to the given path
		* @param p_sceneToSave
		* @param p_path
		*/
		void SaveSceneToDisk(OvCore::SceneSystem::Scene& p_scene, const std::string& p_path);

		/**
		* Load a scene from the disk
		* @param p_path
		* @param p_absolute
		*/
		void LoadSceneFromDisk(const std::string& p_path, bool p_absolute = false);

		/**
		* Returns true if the current scene has been loaded from disk
		*/
		bool IsCurrentSceneLoadedFromDisk() const;

		/**
		* Save the current scene to its disk location
		*/
		void SaveSceneChanges();

		/**
		* Save the current scene to a new disk location (Can create a duplication of the scene file)
		*/
		void SaveAs();
		#pragma endregion

		#pragma region SCRIPTING
		/**
		* Refresh every scripts (Re-interpret)
		*/
		void RefreshScripts();
		#pragma endregion

		#pragma region BUILDING
		/**
		* Ask the user to select the build folder
		*/
		std::optional<std::string> SelectBuildFolder();

		/**
		* Build the current project
		* @param p_autoRun
		* @param p_tempFolder
		*/
		void Build(bool p_autoRun = false, bool p_tempFolder = false);

		/**
		* Build the current project at the given location
		* @param p_configuration
		* @param p_buildPath
		* @param p_autoRun
		*/
		void BuildAtLocation(const std::string& p_configuration, const std::string p_buildPath, bool p_autoRun = false);
		#pragma endregion

		#pragma region ACTION_SYSTEM
		/**
		* Prepare an action for a future call
		* @param p_action
		* @param p_frames
		*/
		void DelayAction(std::function<void()> p_action, uint32_t p_frames = 1);

		/**
		* Execute every actions that should be executed at this frame (Decrement the frame counter for each actions)
		*/
		void ExecuteDelayedActions();
		#pragma endregion

	public:
		OvTools::Eventing::Event<OvCore::ECS::Actor&> ActorSelectedEvent;
		OvTools::Eventing::Event<OvCore::ECS::Actor&> ActorUnselectedEvent;
		OvTools::Eventing::Event<EEditorMode> EditorModeChangedEvent;
		OvTools::Eventing::Event<> PlayEvent;

	private:
		Context& m_context;
		PanelsManager& m_panelsManager;

		EActorSpawnMode m_actorSpawnMode = EActorSpawnMode::ORIGIN;
		EEditorMode m_editorMode = EEditorMode::EDIT;

		std::vector<std::pair<uint32_t, std::function<void()>>> m_delayedActions;

		tinyxml2::XMLDocument m_sceneBackup;
	};
}

#include "OvEditor/Core/EditorActions.inl"