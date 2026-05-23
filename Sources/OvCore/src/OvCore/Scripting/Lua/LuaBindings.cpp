/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Scripting/Lua/LuaBindings.h>

#include <sol/sol.hpp>

void OvCore::Scripting::Lua::BindLuaApi(sol::state& p_luaState)
{
	p_luaState.open_libraries(sol::lib::base, sol::lib::math);

	BindLuaActor(p_luaState);
	BindLuaComponents(p_luaState);
	BindLuaGlobal(p_luaState);
	BindLuaMath(p_luaState);
	BindLuaProfiler(p_luaState);
}
