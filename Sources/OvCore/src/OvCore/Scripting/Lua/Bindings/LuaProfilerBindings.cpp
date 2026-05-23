/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <sol/sol.hpp>
#include <tracy/TracyLua.hpp>

#include <OvCore/Scripting/Lua/LuaBindings.h>

void OvCore::Scripting::Lua::BindLuaProfiler(sol::state& p_luaState)
{
	tracy::LuaRegister(p_luaState.lua_state());
}
