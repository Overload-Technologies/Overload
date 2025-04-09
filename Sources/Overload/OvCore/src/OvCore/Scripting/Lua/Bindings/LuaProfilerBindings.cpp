/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <sol.hpp>
#include <tracy/TracyLua.hpp>

void BindLuaProfiler(sol::state& p_luaState)
{
	tracy::LuaRegister(p_luaState.lua_state());
}
