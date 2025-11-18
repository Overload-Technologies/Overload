/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(LUA_SCRIPTING)
#include <OvCore/Scripting/Lua/LuaScript.h>
#include <OvCore/Scripting/Lua/LuaScriptEngine.h>
#else
#include <OvCore/Scripting/Null/NullScript.h>
#include <OvCore/Scripting/Null/NullScriptEngine.h>
#endif

namespace OvCore::Scripting
{
#if defined(LUA_SCRIPTING)
	using Script = LuaScript;
	using ScriptEngine = LuaScriptEngine;
#else
	using Script = NullScript;
	using ScriptEngine = NullScriptEngine;
#endif
}
