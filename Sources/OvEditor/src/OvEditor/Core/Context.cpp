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
	projectScriptsPath(projectFolder / "Scripts"),
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
	std::vector<uint64_t> iconRaw = { 0,0,144115188614240000ULL,7500771567664627712ULL,7860776967494637312ULL,0,0,0,0,7212820467466371072ULL,11247766461832697600ULL,14274185407633888512ULL,12905091124788992000ULL,5626708973701824512ULL,514575842263176960,0,0,6564302121125019648ULL,18381468271671515136ULL,18381468271654737920ULL,18237353083595659264ULL,18165295488836311040ULL,6708138037527189504ULL,0,4186681893338480640ULL,7932834557741046016ULL,17876782538917681152ULL,11319824055216379904ULL,15210934132358518784ULL,18381468271520454400ULL,1085667680982603520ULL,0,18093237891929479168ULL,18309410677600032768ULL,11391881649237530624ULL,7932834561381570304ULL,17300321784231761408ULL,15210934132375296000ULL,8293405106311272448ULL,2961143145139082752ULL,16507969723533236736ULL,17516777143216379904ULL,10671305705855129600ULL,7356091234422036224ULL,16580027318695106560ULL,2240567205413984000ULL,18381468271470188544ULL,10959253511276599296ULL,4330520004484136960ULL,10815138323200743424ULL,11607771853338181632ULL,8364614976649238272ULL,17444719546862998784ULL,2669156352,18381468269893064448ULL,6419342512197474304ULL,11103650170688640000ULL,6492244531366860800ULL,14346241902646925312ULL,13841557270159628032ULL,7428148827772098304ULL,3464698581331941120ULL,18381468268953606144ULL,1645680384,18381468271554008832ULL,7140201027266418688ULL,5987558797656659712ULL,17588834734687262208ULL,7284033640602212096ULL,14273902834169157632ULL,18381468269087692288ULL,6852253225049397248ULL,17732667349600245504ULL,16291515470083266560ULL,10022503688432981760ULL,11968059825861367552ULL,9733991836700645376ULL,14850363587428816640ULL,18381468271168132864ULL,16147400282007410688ULL,656430432014827520,18381468270950094848ULL,15715054717226194944ULL,72057596690306560,11823944635485519872ULL,15859169905251653376ULL,17084149004500473856ULL,8581352906816952064ULL,2527949855582584832ULL,18381468271419856896ULL,8581352907253225472ULL,252776704,1376441223417430016ULL,14994761349590357760ULL,10527190521537370112ULL,0,9806614576878321664ULL,18381468271671515136ULL,17156206598538401792ULL,6059619689256392448ULL,10166619973990488064ULL,18381468271403079424ULL,17444719549178451968ULL,420746240,870625192710242304,4906133035823863552ULL,18381468269289150464ULL,18381468271671515136ULL,18381468271671515136ULL,9950729769032620032ULL,14778305994951169792ULL,269422336,0,0,18381468268785833984ULL,8941923452686178304ULL,18381468270950094848ULL,3440842496,1233456333565402880ULL,0,0,0,11823944636091210240ULL,2383877888,16724143605745719296ULL,2316834816,0,0 };
	window->SetIconFromMemory(reinterpret_cast<uint8_t*>(iconRaw.data()), 16, 16);
	inputManager = std::make_unique<OvWindowing::Inputs::InputManager>(*window);
	window->MakeCurrentContext();
	device->SetVsync(true);

	/* Graphics context creation */
	driver = std::make_unique<OvRendering::Context::Driver>(OvRendering::Settings::DriverSettings{ true });
	textureRegistry = std::make_unique<OvEditor::Utils::TextureRegistry>();

	std::filesystem::create_directories(Utils::FileSystem::kEditorDataPath);

	uiManager = std::make_unique<OvUI::Core::UIManager>(window->GetGlfwWindow(),
		static_cast<OvUI::Styling::EStyle>(OvEditor::Settings::EditorSettings::ColorTheme.Get())
	);

	const auto fontPath = editorAssetsPath / "Fonts" / "Ruda-Bold.ttf";

	if (!std::filesystem::exists(OvEditor::Utils::FileSystem::kLayoutFilePath))
	{
		const auto defaultLayoutPath = std::filesystem::current_path() / "Config" / "layout.ini";
		uiManager->ResetLayout(defaultLayoutPath.string());
	}

	uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::BIG) }, fontPath.string(), 18);
	uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::MEDIUM) }, fontPath.string(), 15);
	uiManager->LoadFont(std::string{ Settings::GetFontID(Settings::EFontSize::SMALL) }, fontPath.string(), 12);
	uiManager->UseFont(std::string{ Settings::GetFontID(
		static_cast<Settings::EFontSize>(Settings::EditorSettings::FontSize.Get())
	) });

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
	scriptEngine = std::make_unique<OvCore::Scripting::ScriptEngine>();
	scriptEngine->SetScriptRootFolder(projectScriptsPath.string());

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
	projectSettings.Add<bool>("dev_build", true);
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
		projectSettings.IsKeyExisting("dev_build");
}

void OvEditor::Core::Context::ApplyProjectSettings()
{
	physicsEngine->SetGravity({ 0.0f, projectSettings.Get<float>("gravity"), 0.0f });
}
