/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/Scripting/Null/NullScript.h>

template<>
OvCore::Scripting::NullScript::TScript() {}

template<>
OvCore::Scripting::NullScript::~TScript() {}

template<>
bool OvCore::Scripting::NullScript::IsValid() const { return true; }

template<>
std::map<std::string, OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::NullScript::GetDefaultProperties() const { return {}; }

template<>
std::optional<OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::NullScript::GetProperty(const std::string&) const { return std::nullopt; }

template<>
void OvCore::Scripting::NullScript::SetProperty(const std::string&, const OvCore::Scripting::ScriptPropertyValue&) {}
