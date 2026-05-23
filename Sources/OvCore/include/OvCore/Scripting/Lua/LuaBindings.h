/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

namespace sol
{
	class state;
}

namespace OvCore::Scripting::Lua
{
	/**
	* Registers the standard Lua libraries and every symbol exposed by Overload.
	* @param p_luaState
	*/
	void BindLuaApi(sol::state& p_luaState);

	/**
	* Registers actor bindings exposed to Lua scripts.
	* @param p_luaState
	*/
	void BindLuaActor(sol::state& p_luaState);

	/**
	* Registers component bindings exposed to Lua scripts.
	* @param p_luaState
	*/
	void BindLuaComponents(sol::state& p_luaState);

	/**
	* Registers global engine bindings exposed to Lua scripts.
	* @param p_luaState
	*/
	void BindLuaGlobal(sol::state& p_luaState);

	/**
	* Registers math bindings exposed to Lua scripts.
	* @param p_luaState
	*/
	void BindLuaMath(sol::state& p_luaState);

	/**
	* Registers profiler bindings exposed to Lua scripts.
	* @param p_luaState
	*/
	void BindLuaProfiler(sol::state& p_luaState);
}
