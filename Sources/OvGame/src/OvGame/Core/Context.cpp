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
	projectScriptsPath(std::filesystem::current_path() / "Data" / "User" / "Scripts"),
	projectSettings((std::filesystem::current_path() / "Data" / "User" / "Game.ini").string()),
	sceneManager(projectAssetsPath.string())
{
	ModelManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	TextureManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	ShaderManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	MaterialManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	SoundManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);

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
	auto iconRaw = std::to_array<uint64_t>({ 0,0,144115188614240000ULL,7500771567664627712ULL,7860776967494637312ULL,0,0,0,0,7212820467466371072ULL,11247766461832697600ULL,14274185407633888512ULL,12905091124788992000ULL,5626708973701824512ULL,514575842263176960,0,0,6564302121125019648ULL,18381468271671515136ULL,18381468271654737920ULL,18237353083595659264ULL,18165295488836311040ULL,6708138037527189504ULL,0,4186681893338480640ULL,7932834557741046016ULL,17876782538917681152ULL,11319824055216379904ULL,15210934132358518784ULL,18381468271520454400ULL,1085667680982603520ULL,0,18093237891929479168ULL,18309410677600032768ULL,11391881649237530624ULL,7932834561381570304ULL,17300321784231761408ULL,15210934132375296000ULL,8293405106311272448ULL,2961143145139082752ULL,16507969723533236736ULL,17516777143216379904ULL,10671305705855129600ULL,7356091234422036224ULL,16580027318695106560ULL,2240567205413984000ULL,18381468271470188544ULL,10959253511276599296ULL,4330520004484136960ULL,10815138323200743424ULL,11607771853338181632ULL,8364614976649238272ULL,17444719546862998784ULL,2669156352,18381468269893064448ULL,6419342512197474304ULL,11103650170688640000ULL,6492244531366860800ULL,14346241902646925312ULL,13841557270159628032ULL,7428148827772098304ULL,3464698581331941120ULL,18381468268953606144ULL,1645680384,18381468271554008832ULL,7140201027266418688ULL,5987558797656659712ULL,17588834734687262208ULL,7284033640602212096ULL,14273902834169157632ULL,18381468269087692288ULL,6852253225049397248ULL,17732667349600245504ULL,16291515470083266560ULL,10022503688432981760ULL,11968059825861367552ULL,9733991836700645376ULL,14850363587428816640ULL,18381468271168132864ULL,16147400282007410688ULL,656430432014827520,18381468270950094848ULL,15715054717226194944ULL,72057596690306560,11823944635485519872ULL,15859169905251653376ULL,17084149004500473856ULL,8581352906816952064ULL,2527949855582584832ULL,18381468271419856896ULL,8581352907253225472ULL,252776704,1376441223417430016ULL,14994761349590357760ULL,10527190521537370112ULL,0,9806614576878321664ULL,18381468271671515136ULL,17156206598538401792ULL,6059619689256392448ULL,10166619973990488064ULL,18381468271403079424ULL,17444719549178451968ULL,420746240,870625192710242304,4906133035823863552ULL,18381468269289150464ULL,18381468271671515136ULL,18381468271671515136ULL,9950729769032620032ULL,14778305994951169792ULL,269422336,0,0,18381468268785833984ULL,8941923452686178304ULL,18381468270950094848ULL,3440842496,1233456333565402880ULL,0,0,0,11823944636091210240ULL,2383877888,16724143605745719296ULL,2316834816,0,0 });
	window->SetIconFromMemory(reinterpret_cast<uint8_t*>(iconRaw.data()), 16, 16);
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

	uiManager = std::make_unique<OvUI::Core::UIManager>(window->GetGlfwWindow(), OvUI::Styling::EStyle::DEFAULT_DARK);

	const auto fontPath = engineAssetsPath / "Fonts" / "Ruda-Bold.ttf";

	uiManager->LoadFont("Ruda_Big", fontPath.string(), 16);
	uiManager->LoadFont("Ruda_Small", fontPath.string(), 12);
	uiManager->LoadFont("Ruda_Medium", fontPath.string(), 14);
	uiManager->UseFont("Ruda_Medium");
	uiManager->EnableEditorLayoutSave(false);
	uiManager->EnableDocking(false);

	/* Audio */
	audioEngine = std::make_unique<OvAudio::Core::AudioEngine>();

	/* Physics engine */
	physicsEngine = std::make_unique<OvPhysics::Core::PhysicsEngine>(OvPhysics::Settings::PhysicsSettings{ {0.0f, projectSettings.Get<float>("gravity"), 0.0f } });

	/* Scripting */
	scriptEngine = std::make_unique<OvCore::Scripting::ScriptEngine>();
	scriptEngine->SetScriptRootFolder(projectScriptsPath);

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
