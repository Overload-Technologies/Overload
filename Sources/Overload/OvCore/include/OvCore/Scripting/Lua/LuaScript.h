/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvCore/Scripting/Common/TScript.h>

#include <sol/sol.hpp>

namespace OvCore::ECS
{
	class Actor;
}

namespace OvCore::Scripting
{
	/**
	* Lua script context
	*/
	struct LuaScriptContext
	{
		sol::table table;
	};

	using LuaScriptBase = TScript<EScriptingLanguage::LUA, LuaScriptContext>;

	/**
	* Lua script implementation
	*/
	class LuaScript : public LuaScriptBase
	{
	public:
		/**
		* Constructor of the Lua script
		* @param p_table
		*/
		LuaScript(sol::table p_table);

		/**
		* Sets the owner of the script
		* @param p_owner
		*/
		void SetOwner(OvCore::ECS::Actor& p_owner);
	};
}
