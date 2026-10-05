/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <OvCore/Scripting/Common/TScriptEngine.h>

namespace OvCore::ECS::Components
{
	class Behaviour;
}

namespace sol
{
	template <bool b>
	class basic_reference;
	using reference = basic_reference<false>;

	template <typename base_type>
	class basic_object;
	using object = basic_object<reference>;

	class state;
}

namespace OvCore::Scripting
{
	/**
	* Lua script engine context
	*/
	struct LuaScriptEngineContext
	{
		std::unique_ptr<sol::state> luaState;
		std::filesystem::path projectAssetsPath;
		std::filesystem::path engineAssetsPath;
		std::vector<std::reference_wrapper<OvCore::ECS::Components::Behaviour>> behaviours;
		uint32_t errorCount;
	};

	using LuaScriptEngineBase = TScriptEngine<EScriptingLanguage::LUA, LuaScriptEngineContext>;

	/**
	* Lua script engine implementation
	*/
	class LuaScriptEngine : public LuaScriptEngineBase
	{
	public:
		/**
		* Constructor of the lua script engine
		* @param p_projectAssetsPath
		* @param p_engineAssetsPath
		*/
		LuaScriptEngine(
			const std::filesystem::path& p_projectAssetsPath,
			const std::filesystem::path& p_engineAssetsPath
		);

		/**
		* Destructor of the Lua script engine
		*/
		virtual ~LuaScriptEngine();

		/**
		* Create the Lua state
		*/
		void CreateContext();

		/**
		* Destroy the lua state
		*/
		void DestroyContext();

		/**
		* Loads (if not already loaded) and returns the value returned by the script identified
		* by the given path. Returns nil on failure.
		* The returned value is shared by every script, and is discarded along with the lua state.
		* @param p_path
		*/
		sol::object GetScript(const std::string& p_path);
	};
}
