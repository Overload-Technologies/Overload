/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Scripting/ScriptEngine.h>
#include <OvCore/Rendering/FramebufferUtil.h>

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
	engineAssetsPath("Data\\Engine\\"),
	projectAssetsPath("Data\\User\\Assets\\"),
	projectScriptsPath("Data\\User\\Scripts\\"),
	projectSettings("Data\\User\\Game.ini"),
	sceneManager(projectAssetsPath)
{
	ModelManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	TextureManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	ShaderManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	MaterialManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	SoundManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);

	/* Settings */
	OvWindowing::Settings::DeviceSettings deviceSettings;
	projectSettings.TryGet("opengl_major", deviceSettings.contextMajorVersion);
	projectSettings.TryGet("opengl_minor", deviceSettings.contextMinorVersion);
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
	auto iconRaw = std::to_array<uint64_t>({ 0,0,144115188614240000,7500771567664627712,7860776967494637312,0,0,0,0,7212820467466371072,11247766461832697600,14274185407633888512,12905091124788992000,5626708973701824512,514575842263176960,0,0,6564302121125019648,18381468271671515136,18381468271654737920,18237353083595659264,18165295488836311040,6708138037527189504,0,4186681893338480640,7932834557741046016,17876782538917681152,11319824055216379904,15210934132358518784,18381468271520454400,1085667680982603520,0,18093237891929479168,18309410677600032768,11391881649237530624,7932834561381570304,17300321784231761408,15210934132375296000,8293405106311272448,2961143145139082752,16507969723533236736,17516777143216379904,10671305705855129600,7356091234422036224,16580027318695106560,2240567205413984000,18381468271470188544,10959253511276599296,4330520004484136960,10815138323200743424,11607771853338181632,8364614976649238272,17444719546862998784,2669156352,18381468269893064448,6419342512197474304,11103650170688640000,6492244531366860800,14346241902646925312,13841557270159628032,7428148827772098304,3464698581331941120,18381468268953606144,1645680384,18381468271554008832,7140201027266418688,5987558797656659712,17588834734687262208,7284033640602212096,14273902834169157632,18381468269087692288,6852253225049397248,17732667349600245504,16291515470083266560,10022503688432981760,11968059825861367552,9733991836700645376,14850363587428816640,18381468271168132864,16147400282007410688,656430432014827520,18381468270950094848,15715054717226194944,72057596690306560,11823944635485519872,15859169905251653376,17084149004500473856,8581352906816952064,2527949855582584832,18381468271419856896,8581352907253225472,252776704,1376441223417430016,14994761349590357760,10527190521537370112,0,9806614576878321664,18381468271671515136,17156206598538401792,6059619689256392448,10166619973990488064,18381468271403079424,17444719549178451968,420746240,870625192710242304,4906133035823863552,18381468269289150464,18381468271671515136,18381468271671515136,9950729769032620032,14778305994951169792,269422336,0,0,18381468268785833984,8941923452686178304,18381468270950094848,3440842496,1233456333565402880,0,0,0,11823944636091210240,2383877888,16724143605745719296,2316834816,0,0 });
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

	uiManager = std::make_unique<OvUI::Core::UIManager>(window->GetGlfwWindow(), OvUI::Styling::EStyle::ALTERNATIVE_DARK);
	uiManager->LoadFont("Ruda_Big", engineAssetsPath + "Fonts\\Ruda-Bold.ttf", 16);
	uiManager->LoadFont("Ruda_Small", engineAssetsPath + "Fonts\\Ruda-Bold.ttf", 12);
	uiManager->LoadFont("Ruda_Medium", engineAssetsPath + "Fonts\\Ruda-Bold.ttf", 14);
	uiManager->UseFont("Ruda_Medium");
	uiManager->EnableEditorLayoutSave(false);
	uiManager->EnableDocking(false);

	/* Audio */
	audioEngine = std::make_unique<OvAudio::Core::AudioEngine>(projectAssetsPath);
	audioPlayer = std::make_unique<OvAudio::Core::AudioPlayer>(*audioEngine);

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
	ServiceLocator::Provide<OvAudio::Core::AudioPlayer>(*audioPlayer);
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
