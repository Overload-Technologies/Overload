/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvCore/Scripting/Common/TScript.h>

namespace OvCore::ECS
{
	class Actor;
}

namespace sol
{
	template <bool b>
	class basic_reference;
	using reference = basic_reference<false>;

	template <bool, typename>
	class basic_table_core;
	template <bool b>
	using table_core = basic_table_core<b, reference>;
	using table = table_core<false>;
}

namespace OvCore::Scripting
{
	/**
	* Lua script context
	*/
	struct LuaScriptContext
	{
		std::unique_ptr<sol::table> table;
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
