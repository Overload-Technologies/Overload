/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Scripting/Lua/LuaScriptEngine.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <sol/sol.hpp>
#include <tracy/Tracy.hpp>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>
#include <OvCore/Scripting/ScriptEngine.h>
#include <OvCore/ECS/Components/Behaviour.h>
#include <OvCore/ECS/Actor.h>

void BindLuaActor(sol::state& p_state);
void BindLuaComponents(sol::state& p_state);
void BindLuaGlobal(sol::state& p_state);
void BindLuaMath(sol::state& p_state);
void BindLuaProfiler(sol::state& p_state);

namespace
{
	constexpr auto luaBindings = std::array{
		BindLuaActor,
		BindLuaComponents,
		BindLuaGlobal,
		BindLuaMath,
		BindLuaProfiler
	};

	template<typename... Args>
	void ExecuteLuaFunction(OvCore::ECS::Components::Behaviour& p_behaviour, const std::string& p_functionName, Args&& ...p_args)
	{
		auto context = p_behaviour.GetScript();

		OVASSERT(context.has_value(), "The given context is null");
		OVASSERT(context->IsValid(), "The given context is invalid");

		const sol::table& table = *static_cast<OvCore::Scripting::LuaScript&>(context.value()).GetContext().table;

		if (table[p_functionName].valid())
		{
			sol::protected_function pfr = table[p_functionName];
			auto pfrResult = pfr.call(table, std::forward<Args>(p_args)...);
			if (!pfrResult.valid())
			{
				sol::error err = pfrResult;
				OVLOG_ERROR(err.what());
			}
		}
	}

	sol::table LoadScript(sol::state& p_luaState, const std::string& p_scriptName)
	{
		using namespace OvCore::Scripting;

		const auto result = p_luaState.safe_script_file(p_scriptName, &sol::script_pass_on_error);

		if (!result.valid())
		{
			sol::error err = result;
			OVLOG_ERROR(err.what());
			return {};
		}
		else
		{
			if (result.return_count() == 1 && result[0].is<sol::table>())
			{
				return result[0];
			}
			else
			{
				OVLOG_ERROR("'" + p_scriptName + "' missing return expression");
				return {};
			}
		}
	}

	bool RegisterBehaviour(sol::state& p_luaState, OvCore::ECS::Components::Behaviour& p_behaviour, const std::string& p_scriptName)
	{
		auto table = LoadScript(p_luaState, p_scriptName);

		p_behaviour.SetScript(std::make_unique<OvCore::Scripting::LuaScript>(table));

		// Update the script context to add the owner reference
		if (auto context = p_behaviour.GetScript(); context.has_value() && context->IsValid())
		{
			auto& luaScript = static_cast<OvCore::Scripting::LuaScript&>(context.value());
			luaScript.SetOwner(p_behaviour.owner);
			return true;
		}

		return false;
	}

	std::string GetLuarcFileContent(
		const std::filesystem::path& p_engineResourcesFolder
	)
	{
		const auto absolutePath = std::filesystem::absolute(p_engineResourcesFolder);

		return std::format(
			"{{\n"
			"  \"workspace.library\": [\"{}\"],\n"
			"  \"runtime.version\": \"Lua {}.{}\",\n"
			"  \"runtime.builtin\": {{\n"
			"    \"basic\": \"enable\",\n"
			"    \"math\": \"enable\",\n"
			"    \"string\": \"disable\",\n"
			"    \"table\": \"disable\",\n"
			"    \"io\": \"disable\",\n"
			"    \"os\": \"disable\",\n"
			"    \"package\": \"disable\",\n"
			"    \"coroutine\": \"disable\"\n"
			"  }}\n"
			"}}\n",
			absolutePath.string(),
			LUA_VERSION_MAJOR,
			LUA_VERSION_MINOR
		);
	}
}

template<>
OvCore::Scripting::LuaScriptEngineBase::TScriptEngine(
	const std::filesystem::path& p_projectAssetsPath,
	const std::filesystem::path& p_engineAssetsPath
)
{
	m_context.projectAssetsPath = p_projectAssetsPath;
	m_context.engineAssetsPath = p_engineAssetsPath;
}

template<>
OvCore::Scripting::LuaScriptEngineBase::~TScriptEngine() {}

template<>
bool OvCore::Scripting::LuaScriptEngineBase::CreateProjectFiles(const std::filesystem::path& p_projectFolder, bool p_force)
{
	// Create a .luarc.json file at the root of the user's project.
	// This file will allow Lua LSPs to properly discover Lua symbols exposed by Overload.
	const std::filesystem::path luarcPath = p_projectFolder / ".luarc.json";

	// Prevent the .luarc.json from being overrided UNLESS p_force is used
	if (!p_force && std::filesystem::exists(luarcPath))
	{
		return true;
	}

	std::ofstream luarc(luarcPath);
	luarc << GetLuarcFileContent(m_context.engineAssetsPath);
	return true;
}

template<>
std::string OvCore::Scripting::LuaScriptEngineBase::GetDefaultExtension()
{
	return ".lua";
}

template<>
std::vector<std::string> OvCore::Scripting::LuaScriptEngineBase::GetValidExtensions()
{
	return { GetDefaultExtension() };
}

template<>
std::string OvCore::Scripting::LuaScriptEngineBase::GetDefaultScriptContent(const std::string& p_name)
{
	return
		"---@class " + p_name + " : Behaviour\n"
		"local " + p_name + " =\n"
		"{\n"
		"}\n"
		"\n"
		"function " + p_name + ":OnStart()\n"
		"end\n"
		"\n"
		"function " + p_name + ":OnUpdate(deltaTime)\n"
		"end\n"
		"\n"
		"return " + p_name;
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::AddBehaviour(OvCore::ECS::Components::Behaviour& p_toAdd)
{
	OVASSERT(m_context.luaState != nullptr, "No valid Lua context");

	m_context.behaviours.push_back(std::ref(p_toAdd));

	const auto scriptPath = m_context.projectAssetsPath / p_toAdd.name;

	if (!RegisterBehaviour(*m_context.luaState, p_toAdd, scriptPath.string()))
	{
		++m_context.errorCount;
	}
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::Reload()
{
	static_cast<LuaScriptEngine&>(*this).DestroyContext();
	static_cast<LuaScriptEngine&>(*this).CreateContext();
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::RemoveBehaviour(OvCore::ECS::Components::Behaviour& p_toRemove)
{
	if (m_context.luaState)
	{
		p_toRemove.RemoveScript();
	}

	m_context.behaviours.erase(
		std::remove_if(m_context.behaviours.begin(), m_context.behaviours.end(),
			[&p_toRemove](std::reference_wrapper< OvCore::ECS::Components::Behaviour> behaviour) {
				return &p_toRemove == &behaviour.get();
			}
		)
	);

	// Unconsidering a script is impossible with Lua, we have to reparse every behaviours
	// @note this might be costly, we should look into it more seriously.
	Reload(); 
}

template<>
bool OvCore::Scripting::LuaScriptEngineBase::IsOk() const
{
	return m_context.luaState && m_context.errorCount == 0;
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnAwake(OvCore::ECS::Components::Behaviour& p_target)
{
	ExecuteLuaFunction(p_target, "OnAwake");
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnStart(OvCore::ECS::Components::Behaviour& p_target)
{
	ExecuteLuaFunction(p_target, "OnStart");
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnEnable(OvCore::ECS::Components::Behaviour& p_target)
{
	ExecuteLuaFunction(p_target, "OnEnable");
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnDisable(OvCore::ECS::Components::Behaviour& p_target)
{
	ExecuteLuaFunction(p_target, "OnDisable");
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnDestroy(OvCore::ECS::Components::Behaviour& p_target)
{
	ExecuteLuaFunction(p_target, "OnDestroy");
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
	ExecuteLuaFunction(p_target, "OnUpdate", p_deltaTime);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnFixedUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
	ExecuteLuaFunction(p_target, "OnFixedUpdate", p_deltaTime);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnLateUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
	ExecuteLuaFunction(p_target, "OnLateUpdate", p_deltaTime);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnCollisionEnter(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnCollisionEnter", p_otherObject);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnCollisionStay(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnCollisionStay", p_otherObject);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnCollisionExit(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnCollisionExit", p_otherObject);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnTriggerEnter(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnTriggerEnter", p_otherObject);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnTriggerStay(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnTriggerStay", p_otherObject);
}

template<>
void OvCore::Scripting::LuaScriptEngineBase::OnTriggerExit(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
	ExecuteLuaFunction(p_target, "OnTriggerExit", p_otherObject);
}

OvCore::Scripting::LuaScriptEngine::LuaScriptEngine(
	const std::filesystem::path& p_projectAssetsPath,
	const std::filesystem::path& p_engineAssetsPath
) : OvCore::Scripting::LuaScriptEngineBase(
	p_projectAssetsPath,
	p_engineAssetsPath
)
{
	CreateContext();
}

OvCore::Scripting::LuaScriptEngine::~LuaScriptEngine()
{
	DestroyContext();
}

void OvCore::Scripting::LuaScriptEngine::CreateContext()
{
	OVASSERT(m_context.luaState == nullptr, "A Lua context already exists!");

	m_context.luaState = std::make_unique<sol::state>();
	m_context.luaState->open_libraries(sol::lib::base, sol::lib::math);

	for (auto& callback : luaBindings)
	{
		callback(*m_context.luaState);
	}

	m_context.errorCount = 0;

	std::for_each(m_context.behaviours.begin(), m_context.behaviours.end(),
		[this](std::reference_wrapper<OvCore::ECS::Components::Behaviour> behaviour) {
			const auto scriptPath = m_context.projectAssetsPath / behaviour.get().name;
			if (!RegisterBehaviour(*m_context.luaState, behaviour.get(), scriptPath.string()))
			{
				++m_context.errorCount;
			}
		}
	);

	if (m_context.errorCount > 0)
	{
		const std::string message = std::to_string(m_context.errorCount) + " script(s) failed to register";
		OVLOG_ERROR(message);
	}
}

void OvCore::Scripting::LuaScriptEngine::DestroyContext()
{
	OVASSERT(m_context.luaState != nullptr, "No valid Lua context");

	std::for_each(m_context.behaviours.begin(), m_context.behaviours.end(),
		[this](std::reference_wrapper<OvCore::ECS::Components::Behaviour> behaviour)
		{
			behaviour.get().RemoveScript();
		}
	);

	m_context.luaState.reset();
}
