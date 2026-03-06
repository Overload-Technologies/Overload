/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <OvCore/Scripting/Common/TScriptEngine.h>

namespace OvCore::ECS::Components
{
	class Behaviour;
}

namespace sol
{
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
		std::filesystem::path scriptRootFolder;
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
		* Constructor of the Lua script engine
		*/
		LuaScriptEngine();

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
	};
}
