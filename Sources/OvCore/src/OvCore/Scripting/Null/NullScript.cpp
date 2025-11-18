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
