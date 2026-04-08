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

template<>
std::map<std::string, OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::LuaScriptBase::GetDefaultProperties() const
{
	std::map<std::string, ScriptPropertyValue> properties;

	if (!IsValid()) return properties;

	m_context.table->for_each([&](const sol::object& key, const sol::object& value) {
		if (!key.is<std::string>()) return;
		const std::string keyStr = key.as<std::string>();
		if (keyStr.starts_with('_')) return;

		switch (value.get_type())
		{
			case sol::type::boolean: properties[keyStr] = value.as<bool>();        break;
			case sol::type::number:  properties[keyStr] = value.as<double>();      break;
			case sol::type::string:  properties[keyStr] = value.as<std::string>(); break;
			default: break;
		}
	});

	return properties;
}

template<>
std::optional<OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::LuaScriptBase::GetProperty(const std::string& p_key) const
{
	if (!IsValid()) return std::nullopt;

	const sol::object obj = (*m_context.table)[p_key];

	switch (obj.get_type())
	{
		case sol::type::boolean: return obj.as<bool>();
		case sol::type::number:  return obj.as<double>();
		case sol::type::string:  return obj.as<std::string>();
		default:                 return std::nullopt;
	}
}

template<>
void OvCore::Scripting::LuaScriptBase::SetProperty(const std::string& p_key, const OvCore::Scripting::ScriptPropertyValue& p_value)
{
	if (!IsValid()) return;

	std::visit([&](auto&& v) {
		(*m_context.table)[p_key] = v;
	}, p_value);
}
