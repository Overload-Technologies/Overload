/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <memory>

#include <OvCore/Scripting/Common/TScriptEngine.h>

#include <sol/sol.hpp>

namespace OvCore::ECS::Components
{
	class Behaviour;
}

namespace OvCore::Scripting
{
	/**
	* Lua script engine context
	*/
	struct LuaScriptEngineContext
	{
		std::unique_ptr<sol::state> luaState;
		std::string scriptRootFolder;
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
		~LuaScriptEngine();

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
