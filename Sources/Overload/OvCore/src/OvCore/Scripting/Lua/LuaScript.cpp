/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <sol/sol.hpp>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/Scripting/Lua/LuaScript.h>

template<>
OvCore::Scripting::LuaScriptBase::TScript() = default;

template<>
OvCore::Scripting::LuaScriptBase::~TScript() = default;

template<>
bool OvCore::Scripting::LuaScriptBase::IsValid() const
{
	return m_context.table && m_context.table->valid();
}

OvCore::Scripting::LuaScript::LuaScript(sol::table table)
{
	m_context.table = std::make_unique<sol::table>(table);
}

void OvCore::Scripting::LuaScript::SetOwner(OvCore::ECS::Actor& p_owner)
{
	(*m_context.table)["owner"] = &p_owner;
}
