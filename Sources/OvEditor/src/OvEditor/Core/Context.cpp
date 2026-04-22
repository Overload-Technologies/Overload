/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Scripting/ScriptEngine.h>
#include <OvDebug/Assertion.h>
#include <OvEditor/Core/Context.h>
#include <OvEditor/Utils/FileSystem.h>
#include <OvEditor/Utils/ProjectManagement.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvRendering/Entities/Light.h>
#include <OvTools/Utils/SystemCalls.h>
#include <optional>

using namespace OvCore::Global;
using namespace OvCore::ResourceManagement;

constexpr std::array<std::pair<int, int>, 13> kResolutions
{
	std::make_pair(640, 360), // nHD
	std::make_pair(854, 480), // FWVGA
	std::make_pair(960, 540), // qHD
	std::make_pair(1024, 576), // WSVGA
	std::make_pair(1280, 720), // HD
	std::make_pair(1366, 768), // FWXGA
	std::make_pair(1600, 900), // HD+
	std::make_pair(1920, 1080), // Full HD
	std::make_pair(2560, 1440), // QHD
	std::make_pair(3200, 1800), // QHD+
	std::make_pair(3840, 2160), // 4K UHD
	std::make_pair(5120, 2880), // 5K
	std::make_pair(7680, 4320), // 8K UHD
};

std::array<int, 4> FindBestFitWindowSizeAndPosition(std::array<int, 4> p_workAreaSize)
{
	// Extract work area dimensions
	int workAreaX = p_workAreaSize[0];
	int workAreaY = p_workAreaSize[1];
	int workAreaWidth = p_workAreaSize[2];
	int workAreaHeight = p_workAreaSize[3];

	// Iterate over available resolutions
	for (auto it = kResolutions.rbegin(); it != kResolutions.rend(); ++it)
	{
		int width = it->first;
		int height = it->second;

		// Check if resolution fits within work area
		if (width <= workAreaWidth && height <= workAreaHeight)
		{
			// Center the resolution within the work area
			int posX = workAreaX + workAreaWidth / 2 - width / 2;
			int posY = workAreaY + workAreaHeight / 2 - height / 2;

			return { posX, posY, width, height };
		}
	}

	OVASSERT(false, "No resolution found to fit the work area");
	return {};
}

OvEditor::Core::Context::Context(const std::filesystem::path& p_projectFolder) :
	projectFolder(p_projectFolder),
	projectFile(Utils::ProjectManagement::GetProjectFile(p_projectFolder)),
	engineAssetsPath(std::filesystem::current_path() / "Data" / "Engine"),
	projectAssetsPath(projectFolder / "Assets"),
	editorAssetsPath(std::filesystem::current_path() / "Data" / "Editor"),
	sceneManager(projectAssetsPath.string()),
	projectSettings(projectFile.string())
{
	if (!IsProjectSettingsIntegrityVerified())
	{
		ResetProjectSettings();
		projectSettings.Rewrite();
	}

	ModelManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	TextureManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	ShaderManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	MaterialManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	SoundManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);

	materialManager.ProvideStandardShaderDefinition({
		.shaderPath = ":Shaders/Standard.ovfx"
	});

	/* Settings */
	OvWindowing::Settings::DeviceSettings deviceSettings;
	deviceSettings.contextMajorVersion = 4;
	deviceSettings.contextMinorVersion = 5;

	/* Window creation */
	device = std::make_unique<OvWindowing::Context::Device>(deviceSettings);
	const auto workAreaSize = device->GetWorkAreaSize();
	const auto bestFitWindowSizeAndPosition = FindBestFitWindowSizeAndPosition(workAreaSize);
	windowSettings.title = "Overload";
	windowSettings.x = bestFitWindowSizeAndPosition[0];
	windowSettings.y = bestFitWindowSizeAndPosition[1];
	windowSettings.width = bestFitWindowSizeAndPosition[2];
	windowSettings.height = bestFitWindowSizeAndPosition[3];
	window = std::make_unique<OvWindowing::Window>(*device, windowSettings);
	const auto fallbackWindowIconPath = engineAssetsPath / "Textures" / "Overload.png";
	window->SetIcon(fallbackWindowIconPath.string());
	inputManager = std::make_unique<OvWindowing::Inputs::InputManager>(*window);
	window->MakeCurrentContext();
	device->SetVsync(true);

	/* Graphics context creation */
	driver = std::make_unique<OvRendering::Context::Driver>(OvRendering::Settings::DriverSettings{ true });
	textureRegistry = std::make_unique<OvEditor::Utils::TextureRegistry>();

	std::filesystem::create_directories(Utils::FileSystem::kEditorDataPath);

	uiManager = std::make_unique<OvUI::Core::UIManager>(
		*window,
		static_cast<OvUI::Styling::EStyle>(OvEditor::Settings::EditorSettings::ColorTheme.Get())
	);

	if (!std::filesystem::exists(OvEditor::Utils::FileSystem::kLayoutFilePath))
	{
		const auto defaultLayoutPath = std::filesystem::current_path() / "Config" / "layout.ini";
		uiManager->ResetLayout(defaultLayoutPath.string());
	}

	const auto fontPath = editorAssetsPath / "Fonts" / "Ruda-Bold.ttf";
	uiManager->LoadFont("Ruda-Bold", fontPath.string(), 15);
	uiManager->UseFont("Ruda-Bold");
	const int uiScale = Settings::EditorSettings::UIScale.Get();
	uiManager->SetScale(uiScale == 0 ? std::nullopt : std::make_optional(uiScale / 100.0f));
	uiManager->SetEditorLayoutSaveFilename(OvEditor::Utils::FileSystem::kLayoutFilePath.string());
	uiManager->SetEditorLayoutAutosaveFrequency(60.0f);
	uiManager->EnableEditorLayoutSave(true);
	uiManager->EnableDocking(true);

	/* Audio */
	audioEngine = std::make_unique<OvAudio::Core::AudioEngine>();

	/* Editor resources */
	editorResources = std::make_unique<OvEditor::Core::EditorResources>(editorAssetsPath.string());

	/* Physics engine */
	physicsEngine = std::make_unique<OvPhysics::Core::PhysicsEngine>(OvPhysics::Settings::PhysicsSettings{ {0.0f, -9.81f, 0.0f } });

	/* Scripting */
	scriptEngine = std::make_unique<OvCore::Scripting::ScriptEngine>(
		projectAssetsPath,
		engineAssetsPath
	);

	// Ensures lua project files are up-to-date. This is necessary for Lua's LSP to function properly.
	// If Overload's installation directory changes, references to engine symbols would be lost,
	// hence this invocation.
	scriptEngine->CreateProjectFiles(
		projectFolder,
		Settings::EditorSettings::RegenerateScriptingProjectFilesOnStartup
	);

	/* Service Locator providing */
	ServiceLocator::Provide<OvPhysics::Core::PhysicsEngine>(*physicsEngine);
	ServiceLocator::Provide<ModelManager>(modelManager);
	ServiceLocator::Provide<TextureManager>(textureManager);
	ServiceLocator::Provide<ShaderManager>(shaderManager);
	ServiceLocator::Provide<MaterialManager>(materialManager);
	ServiceLocator::Provide<SoundManager>(soundManager);
	ServiceLocator::Provide<OvWindowing::Inputs::InputManager>(*inputManager);
	ServiceLocator::Provide<OvWindowing::Window>(*window);
	ServiceLocator::Provide<OvCore::SceneSystem::SceneManager>(sceneManager);
	ServiceLocator::Provide<OvAudio::Core::AudioEngine>(*audioEngine);
	ServiceLocator::Provide<OvCore::Scripting::ScriptEngine>(*scriptEngine);
	ServiceLocator::Provide<OvEditor::Utils::TextureRegistry>(*textureRegistry);

	ApplyProjectSettings();
}

OvEditor::Core::Context::~Context()
{
	modelManager.UnloadResources();
	textureManager.UnloadResources();
	shaderManager.UnloadResources();
	materialManager.UnloadResources();
	soundManager.UnloadResources();
}

void OvEditor::Core::Context::ResetProjectSettings()
{
	projectSettings.RemoveAll();
	projectSettings.Add<float>("gravity", -9.81f);
	projectSettings.Add<int>("x_resolution", 1280);
	projectSettings.Add<int>("y_resolution", 720);
	projectSettings.Add<bool>("fullscreen", false);
	projectSettings.Add<std::string>("executable_name", "Game");
	projectSettings.Add<std::string>("start_scene", "Scene.ovscene");
	projectSettings.Add<bool>("vsync", true);
	projectSettings.Add<bool>("multisampling", false);
	projectSettings.Add<int>("samples", 4);
	projectSettings.Add<int>("build_type", 0);
	projectSettings.Add<std::string>("window_icon", "");
}

bool OvEditor::Core::Context::IsProjectSettingsIntegrityVerified()
{
	return
		projectSettings.IsKeyExisting("gravity") &&
		projectSettings.IsKeyExisting("x_resolution") &&
		projectSettings.IsKeyExisting("y_resolution") &&
		projectSettings.IsKeyExisting("fullscreen") &&
		projectSettings.IsKeyExisting("executable_name") &&
		projectSettings.IsKeyExisting("start_scene") &&
		projectSettings.IsKeyExisting("vsync") &&
		projectSettings.IsKeyExisting("multisampling") &&
		projectSettings.IsKeyExisting("samples") &&
		projectSettings.IsKeyExisting("build_type") &&
		projectSettings.IsKeyExisting("window_icon");
}

void OvEditor::Core::Context::ApplyProjectSettings()
{
	physicsEngine->SetGravity({ 0.0f, projectSettings.Get<float>("gravity"), 0.0f });
}
