/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Logger.h>
#include <OvTools/Utils/Random.h>

#include "OvCore/ECS/Actor.h"
#include "OvCore/ECS/PhysicsWrapper.h"
#include "OvCore/Global/ServiceLocator.h"
#include "OvCore/SceneSystem/SceneManager.h"
#include "OvCore/ResourceManagement/ModelManager.h"
#include "OvCore/ResourceManagement/ShaderManager.h"
#include "OvCore/ResourceManagement/TextureManager.h"
#include "OvCore/ResourceManagement/MaterialManager.h"
#include "OvCore/ResourceManagement/SoundManager.h"

#include <OvPhysics/Entities/PhysicalObject.h>

#include <OvWindowing/Inputs/InputManager.h>

#include <sol/sol.hpp>

void BindLuaGlobal(sol::state& p_luaState)
{
	using namespace OvWindowing;
	using namespace OvWindowing::Inputs;
	using namespace OvMaths;
	using namespace OvCore::ECS;
	using namespace OvCore::SceneSystem;
	using namespace OvCore::ResourceManagement;

	p_luaState.new_usertype<Scene>("Scene",
		"FindActorByName", &Scene::FindActorByName,
		"FindActorByTag", &Scene::FindActorByTag,
		"FindActorsByName", &Scene::FindActorsByName,
		"FindActorsByTag", &Scene::FindActorsByTag,
		"CreateActor", sol::overload(
			sol::resolve<Actor&(void)>(&Scene::CreateActor),
			sol::resolve<Actor&(const std::string&, const std::string&)>(&Scene::CreateActor))
	);

	p_luaState.new_enum<EKey>("Key", {
		{"UNKNOWN",			EKey::KEY_UNKNOWN},
		{"SPACE",			EKey::KEY_SPACE},
		{"APOSTROPHE",		EKey::KEY_APOSTROPHE},
		{"COMMA",			EKey::KEY_COMMA},
		{"MINUS",			EKey::KEY_MINUS},
		{"PERIOD",			EKey::KEY_PERIOD},
		{"SLASH",			EKey::KEY_SLASH},
		{"ALPHA_0",			EKey::KEY_0},
		{"ALPHA_1",			EKey::KEY_1},
		{"ALPHA_2",			EKey::KEY_2},
		{"ALPHA_3",			EKey::KEY_3},
		{"ALPHA_4",			EKey::KEY_4},
		{"ALPHA_5",			EKey::KEY_5},
		{"ALPHA_6",			EKey::KEY_6},
		{"ALPHA_7",			EKey::KEY_7},
		{"ALPHA_8",			EKey::KEY_8},
		{"ALPHA_9",			EKey::KEY_9},
		{"SEMICOLON",		EKey::KEY_SEMICOLON},
		{"EQUAL",			EKey::KEY_EQUAL},
		{"A",				EKey::KEY_A},
		{"B",				EKey::KEY_B},
		{"C",				EKey::KEY_C},
		{"D",				EKey::KEY_D},
		{"E",				EKey::KEY_E},
		{"F",				EKey::KEY_F},
		{"G",				EKey::KEY_G},
		{"H",				EKey::KEY_H},
		{"I",				EKey::KEY_I},
		{"J",				EKey::KEY_J},
		{"K",				EKey::KEY_K},
		{"L",				EKey::KEY_L},
		{"M",				EKey::KEY_M},
		{"N",				EKey::KEY_N},
		{"O",				EKey::KEY_O},
		{"P",				EKey::KEY_P},
		{"Q",				EKey::KEY_Q},
		{"R",				EKey::KEY_R},
		{"S",				EKey::KEY_S},
		{"T",				EKey::KEY_T},
		{"U",				EKey::KEY_U},
		{"V",				EKey::KEY_V},
		{"W",				EKey::KEY_W},
		{"X",				EKey::KEY_X},
		{"Y",				EKey::KEY_Y},
		{"Z",				EKey::KEY_Z},
		{"LEFT_BRACKET",	EKey::KEY_LEFT_BRACKET},
		{"BACKSLASH",		EKey::KEY_BACKSLASH},
		{"RIGHT_BRACKET",	EKey::KEY_RIGHT_BRACKET},
		{"GRAVE_ACCENT",	EKey::KEY_GRAVE_ACCENT},
		{"WORLD_1",			EKey::KEY_WORLD_1},
		{"WORLD_2",			EKey::KEY_WORLD_2},
		{"ESCAPE",			EKey::KEY_ESCAPE},
		{"ENTER",			EKey::KEY_ENTER},
		{"TAB",				EKey::KEY_TAB},
		{"BACKSPACE",		EKey::KEY_BACKSPACE},
		{"INSERT",			EKey::KEY_INSERT},
		{"DELETE",			EKey::KEY_DELETE},
		{"RIGHT",			EKey::KEY_RIGHT},
		{"LEFT",			EKey::KEY_LEFT},
		{"DOWN",			EKey::KEY_DOWN},
		{"UP",				EKey::KEY_UP},
		{"PAGE_UP",			EKey::KEY_PAGE_UP},
		{"PAGE_DOWN",		EKey::KEY_PAGE_DOWN},
		{"HOME",			EKey::KEY_HOME},
		{"END",				EKey::KEY_END},
		{"CAPS_LOCK",		EKey::KEY_CAPS_LOCK},
		{"SCROLL_LOCK",		EKey::KEY_SCROLL_LOCK},
		{"NUM_LOCK",		EKey::KEY_NUM_LOCK},
		{"PRINT_SCREEN",	EKey::KEY_PRINT_SCREEN},
		{"PAUSE",			EKey::KEY_PAUSE},
		{"F1",				EKey::KEY_F1},
		{"F2",				EKey::KEY_F2},
		{"F3",				EKey::KEY_F3},
		{"F4",				EKey::KEY_F4},
		{"F5",				EKey::KEY_F5},
		{"F6",				EKey::KEY_F6},
		{"F7",				EKey::KEY_F7},
		{"F8",				EKey::KEY_F8},
		{"F9",				EKey::KEY_F9},
		{"F10",				EKey::KEY_F10},
		{"F11",				EKey::KEY_F11},
		{"F12",				EKey::KEY_F12},
		{"F13",				EKey::KEY_F13},
		{"F14",				EKey::KEY_F14},
		{"F15",				EKey::KEY_F15},
		{"F16",				EKey::KEY_F16},
		{"F17",				EKey::KEY_F17},
		{"F18",				EKey::KEY_F18},
		{"F19",				EKey::KEY_F19},
		{"F20",				EKey::KEY_F20},
		{"F21",				EKey::KEY_F21},
		{"F22",				EKey::KEY_F22},
		{"F23",				EKey::KEY_F23},
		{"F24",				EKey::KEY_F24},
		{"F25",				EKey::KEY_F25},
		{"KP_0",			EKey::KEY_KP_0},
		{"KP_1",			EKey::KEY_KP_1},
		{"KP_2",			EKey::KEY_KP_2},
		{"KP_3",			EKey::KEY_KP_3},
		{"KP_4",			EKey::KEY_KP_4},
		{"KP_5",			EKey::KEY_KP_5},
		{"KP_6",			EKey::KEY_KP_6},
		{"KP_7",			EKey::KEY_KP_7},
		{"KP_8",			EKey::KEY_KP_8},
		{"KP_9",			EKey::KEY_KP_9},
		{"KP_DECIMAL",		EKey::KEY_KP_DECIMAL},
		{"KP_DIVIDE",		EKey::KEY_KP_DIVIDE},
		{"KP_MULTIPLY",		EKey::KEY_KP_MULTIPLY},
		{"KP_SUBTRACT",		EKey::KEY_KP_SUBTRACT},
		{"KP_ADD",			EKey::KEY_KP_ADD},
		{"KP_ENTER",		EKey::KEY_KP_ENTER},
		{"KP_EQUAL",		EKey::KEY_KP_EQUAL},
		{"LEFT_SHIFT",		EKey::KEY_LEFT_SHIFT},
		{"LEFT_CONTROL",	EKey::KEY_LEFT_CONTROL},
		{"LEFT_ALT",		EKey::KEY_LEFT_ALT},
		{"LEFT_SUPER",		EKey::KEY_LEFT_SUPER},
		{"RIGHT_SHIFT",		EKey::KEY_RIGHT_SHIFT},
		{"RIGHT_CONTROL",	EKey::KEY_RIGHT_CONTROL},
		{"RIGHT_ALT",		EKey::KEY_RIGHT_ALT},
		{"RIGHT_SUPER",		EKey::KEY_RIGHT_SUPER},
		{"MENU",			EKey::KEY_MENU}
	});

	p_luaState.new_enum<EMouseButton>("MouseButton", {
		{"BUTTON_1",		EMouseButton::MOUSE_BUTTON_1},
		{"BUTTON_2",		EMouseButton::MOUSE_BUTTON_2},
		{"BUTTON_3",		EMouseButton::MOUSE_BUTTON_3},
		{"BUTTON_4",		EMouseButton::MOUSE_BUTTON_4},
		{"BUTTON_5",		EMouseButton::MOUSE_BUTTON_5},
		{"BUTTON_6",		EMouseButton::MOUSE_BUTTON_6},
		{"BUTTON_7",		EMouseButton::MOUSE_BUTTON_7},
		{"BUTTON_8",		EMouseButton::MOUSE_BUTTON_8},
		{"BUTTON_LEFT",		EMouseButton::MOUSE_BUTTON_LEFT},
		{"BUTTON_RIGHT",	EMouseButton::MOUSE_BUTTON_RIGHT},
		{"BUTTON_MIDDLE",	EMouseButton::MOUSE_BUTTON_MIDDLE},
	});

	p_luaState.create_named_table("Debug",
		"Log", [](const std::string& p_message) { OVLOG(p_message); },
		"LogInfo", [](const std::string& p_message) { OVLOG_INFO(p_message); },
		"LogWarning", [](const std::string& p_message) { OVLOG_WARNING(p_message); },
		"LogError", [](const std::string& p_message) { OVLOG_ERROR(p_message); }
	);

	p_luaState.create_named_table("Inputs",
		"GetKeyDown", [](EKey p_key) { return OVSERVICE(InputManager).IsKeyPressed(p_key); },
		"GetKeyUp", [](EKey p_key) { return OVSERVICE(InputManager).IsKeyReleased(p_key); },
		"GetKey", [](EKey p_key) { return OVSERVICE(InputManager).GetKeyState(p_key) == EKeyState::KEY_DOWN; },
		"GetMouseButtonDown", [](EMouseButton p_button) { return OVSERVICE(InputManager).IsMouseButtonPressed(p_button); },
		"GetMouseButtonUp", [](EMouseButton p_button) { return OVSERVICE(InputManager).IsMouseButtonReleased(p_button); },
		"GetMouseButton", [](EMouseButton p_button) { return OVSERVICE(InputManager).GetMouseButtonState(p_button) == EMouseButtonState::MOUSE_DOWN; },
		"GetMousePos", []() {
			const auto mousePos = OVSERVICE(InputManager).GetMousePosition();
			return FVector3(static_cast<float>(mousePos.first), static_cast<float>(mousePos.second));
		},
		"GetMouseScroll", []() {
			const auto scroll = OVSERVICE(InputManager).GetMouseScroll();
			return FVector2(static_cast<float>(scroll.first), static_cast<float>(scroll.second));
		},
		"LockMouse", []() { return OVSERVICE(Window).SetCursorMode(Cursor::ECursorMode::DISABLED); },
		"UnlockMouse", []() { return OVSERVICE(Window).SetCursorMode(Cursor::ECursorMode::NORMAL); }
	);

	p_luaState.create_named_table("Scenes",
		"GetCurrentScene", []() -> Scene& { return *OVSERVICE(SceneManager).GetCurrentScene(); },
		"Load", [](const std::string& p_path) { OVSERVICE(SceneManager).LoadAndPlayDelayed(p_path); }
	);

	p_luaState.create_named_table("Resources",
		"GetModel", [](const std::string& p_resPath) { return OVSERVICE(ModelManager).GetResource(p_resPath); },
		"GetShader", [](const std::string& p_resPath) { return OVSERVICE(ShaderManager).GetResource(p_resPath); },
		"GetTexture", [](const std::string& p_resPath) { return OVSERVICE(TextureManager).GetResource(p_resPath); },
		"GetMaterial", [](const std::string& p_resPath) { return OVSERVICE(MaterialManager).GetResource(p_resPath); },
		"GetSound", [](const std::string& p_resPath) { return OVSERVICE(SoundManager).GetResource(p_resPath); }
	);

	p_luaState.create_named_table("Math",
		"RandomInt", [](int p_min, int p_max) { return OvTools::Utils::Random::Generate(p_min, p_max); },
		"RandomFloat", [](float p_min, float p_max) { return OvTools::Utils::Random::Generate(p_min, p_max); },
		"CheckPercentage", [](float p_percentage) { return OvTools::Utils::Random::CheckPercentage(p_percentage); },
		"Lerp", [](float a, float b, float t) -> float { return a + t * (b - a); }
	);

	p_luaState.new_usertype<PhysicsWrapper::RaycastHit>("RaycastHit",
		"FirstResultObject", &PhysicsWrapper::RaycastHit::FirstResultObject,
		"ResultObjects", &PhysicsWrapper::RaycastHit::ResultObjects
	);

	p_luaState.create_named_table("Physics",
		"Raycast", [](const OvMaths::FVector3& p_origin, const OvMaths::FVector3& p_direction, float p_distance) { return PhysicsWrapper::Raycast(p_origin, p_direction, p_distance); }
	);
}
