/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Filesystem/IniFile.h>

#include <OvRendering/HAL/UniformBuffer.h>
#include <OvRendering/HAL/ShaderStorageBuffer.h>

#include <OvPhysics/Core/PhysicsEngine.h>

#include <OvAudio/Core/AudioEngine.h>

#include <OvWindowing/Context/Device.h>
#include <OvWindowing/Inputs/InputManager.h>
#include <OvWindowing/Window.h>

#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/SoundManager.h>
#include <OvCore/SceneSystem/SceneManager.h>
#include <OvCore/Scripting/ScriptEngine.h>

#include <OvUI/Core/UIManager.h>
#include <OvAudio/Core/AudioEngine.h>
#include <OvAudio/Core/AudioPlayer.h>

#include "OvEditor/Core/EditorResources.h"
#include <OvEditor/Utils/TextureRegistry.h>

namespace OvEditor::Core
{
	/**
	* The Context handle the engine features setup
	*/
	class Context
	{
	public:
		/**
		* Constructor
		* @param p_projectPath
		* @param p_projectName
		*/
		Context(const std::string& p_projectPath, const std::string& p_projectName);

		/**
		* Destructor
		*/
		~Context();

		/**
		* Reset project settings ini file
		*/
		void ResetProjectSettings();

		/**
		* Verify that project settings are complete (No missing key).
		* Returns true if the integrity is verified
		*/
		bool IsProjectSettingsIntegrityVerified();

		/**
		* Apply project settings to the ini file
		*/
		void ApplyProjectSettings();

	public:
		const std::string projectPath;
		const std::string projectName;
		const std::string projectFilePath;
		const std::string engineAssetsPath;
		const std::string projectAssetsPath;
		const std::string projectScriptsPath;
		const std::string editorAssetsPath;

		std::unique_ptr<OvWindowing::Context::Device> device;
		std::unique_ptr<OvWindowing::Window> window;
		std::unique_ptr<OvWindowing::Inputs::InputManager> inputManager;
		std::unique_ptr<OvEditor::Utils::TextureRegistry> textureRegistry;
		std::unique_ptr<OvRendering::Context::Driver> driver;
		std::unique_ptr<OvUI::Core::UIManager> uiManager;
		std::unique_ptr<OvPhysics::Core::PhysicsEngine> physicsEngine;
		std::unique_ptr<OvAudio::Core::AudioEngine> audioEngine;
		std::unique_ptr<OvAudio::Core::AudioPlayer> audioPlayer;
		std::unique_ptr<OvEditor::Core::EditorResources> editorResources;

		std::unique_ptr<OvCore::Scripting::ScriptEngine> scriptEngine;

		OvCore::SceneSystem::SceneManager sceneManager;

		OvCore::ResourceManagement::ModelManager modelManager;
		OvCore::ResourceManagement::TextureManager textureManager;
		OvCore::ResourceManagement::ShaderManager shaderManager;
		OvCore::ResourceManagement::MaterialManager materialManager;
		OvCore::ResourceManagement::SoundManager soundManager;

		OvWindowing::Settings::WindowSettings windowSettings;

		OvTools::Filesystem::IniFile projectSettings;
	};
}
