/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Scripting/ScriptEngine.h>

#include <OvDebug/Logger.h>

#include <OvGame/Core/Context.h>
#include <OvTools/Utils/PathParser.h>

using namespace OvCore::Global;
using namespace OvCore::ResourceManagement;

std::array<int, 4> CalculateOptimalWindowSizeAndPosition(
	const std::array<int, 4>& p_workAreaSize,
	std::array<int, 2> p_resolution
)
{
	const auto [workAreaX, workAreaY, workAreaWidth, workAreaHeight] = p_workAreaSize;

	std::array<int, 2> finalResolution = p_resolution;

	// Check if the given resolution fits in the workArea. If not, keep reducing it until it fits
	while (finalResolution[0] > workAreaWidth || finalResolution[1] > workAreaHeight)
	{
		finalResolution[0] /= 2;
		finalResolution[1] /= 2;
	}

	if (finalResolution != p_resolution)
	{
		OVLOG_WARNING("The target resolution didn't fit in the work area. It has been reduced to " + std::to_string(p_resolution[0]) + "x" + std::to_string(p_resolution[1]));
	}

	return { 
		workAreaX + workAreaWidth / 2 - p_resolution[0] / 2,
		workAreaY + workAreaHeight / 2 - p_resolution[1] / 2,
		p_resolution[0],
		p_resolution[1]
	};
}

OvGame::Core::Context::Context() :
	engineAssetsPath(std::filesystem::current_path() / "Data" / "Engine"),
	projectAssetsPath(std::filesystem::current_path() / "Data" / "User" / "Assets"),
	projectSettings((std::filesystem::current_path() / "Data" / "User" / "Game.ini").string()),
	sceneManager(projectAssetsPath.string())
{
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
	projectSettings.TryGet("samples", deviceSettings.samples);

	OvWindowing::Settings::WindowSettings windowSettings;
	projectSettings.TryGet("executable_name", windowSettings.title);
	std::array<int, 2> targetWindowResolution = { 1280, 720 };
	projectSettings.TryGet("x_resolution", targetWindowResolution[0]);
	projectSettings.TryGet("y_resolution", targetWindowResolution[1]);
	windowSettings.maximized = false;
	windowSettings.resizable = false;
	projectSettings.TryGet("fullscreen", windowSettings.fullscreen);
	projectSettings.TryGet("samples", windowSettings.samples);

	/* Window creation */
	device = std::make_unique<OvWindowing::Context::Device>(deviceSettings);
	if (windowSettings.fullscreen)
	{
		windowSettings.width = targetWindowResolution[0];
		windowSettings.height = targetWindowResolution[1];
	}
	else
	{
		const auto workAreaSize = device->GetWorkAreaSize();
		const auto bestFitWindowSizeAndPosition = CalculateOptimalWindowSizeAndPosition(workAreaSize, targetWindowResolution);
		windowSettings.x = bestFitWindowSizeAndPosition[0];
		windowSettings.y = bestFitWindowSizeAndPosition[1];
		windowSettings.width = bestFitWindowSizeAndPosition[2];
		windowSettings.height = bestFitWindowSizeAndPosition[3];
	}
	window = std::make_unique<OvWindowing::Window>(*device, windowSettings);
	bool useFallbackWindowIcon = true;
	if (const std::string windowIconPath = projectSettings.GetOrDefault("window_icon", std::string{}); !windowIconPath.empty())
	{
		const std::filesystem::path realPath = OvTools::Utils::PathParser::GetRealPath(
			std::filesystem::path{ windowIconPath },
			engineAssetsPath,
			projectAssetsPath
		);

		useFallbackWindowIcon = !std::filesystem::exists(realPath) || !window->SetIcon(realPath.string());
	}
	if (useFallbackWindowIcon)
	{
		const auto fallbackWindowIconPath = engineAssetsPath / "Textures" / "Overload.png";
		if (!window->SetIcon(fallbackWindowIconPath.string()))
		{
			OVLOG_WARNING("Fallback window icon could not be loaded from: " + fallbackWindowIconPath.string());
		}
	}
	inputManager = std::make_unique<OvWindowing::Inputs::InputManager>(*window);
	window->MakeCurrentContext();

	device->SetVsync(projectSettings.GetOrDefault<bool>("vsync", true));

	OvRendering::Data::PipelineState basePSO;
	basePSO.multisample = projectSettings.GetOrDefault<bool>("multisampling", false);

	/* Graphics context creation */
	driver = std::make_unique<OvRendering::Context::Driver>(OvRendering::Settings::DriverSettings{
#ifdef _DEBUG
		true,
#else
		false,
#endif
		basePSO
	});

	uiManager = std::make_unique<OvUI::Core::UIManager>(
		*window,
		OvUI::Styling::EStyle::DEFAULT_DARK
	);

	const auto fontPath = engineAssetsPath / "Fonts" / "Ruda-Bold.ttf";

	uiManager->LoadFont("Ruda-Bold", fontPath.string(), 15);
	uiManager->UseFont("Ruda-Bold");
	uiManager->EnableEditorLayoutSave(false);
	uiManager->EnableDocking(false);

	/* Audio */
	audioEngine = std::make_unique<OvAudio::Core::AudioEngine>();

	/* Physics engine */
	physicsEngine = std::make_unique<OvPhysics::Core::PhysicsEngine>(OvPhysics::Settings::PhysicsSettings{ {0.0f, projectSettings.Get<float>("gravity"), 0.0f } });

	/* Scripting */
	scriptEngine = std::make_unique<OvCore::Scripting::ScriptEngine>(
		projectAssetsPath,
		engineAssetsPath
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

	framebuffer = std::make_unique<OvRendering::HAL::Framebuffer>("Main");

	OvCore::Rendering::FramebufferUtil::SetupFramebuffer(
		*framebuffer,
		windowSettings.width,
		windowSettings.height,
		true, false, false
	);
}

OvGame::Core::Context::~Context()
{
	modelManager.UnloadResources();
	textureManager.UnloadResources();
	shaderManager.UnloadResources();
	materialManager.UnloadResources();
	soundManager.UnloadResources();
}
