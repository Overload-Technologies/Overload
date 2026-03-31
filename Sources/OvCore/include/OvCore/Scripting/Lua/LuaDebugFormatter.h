/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <sol/sol.hpp>

namespace OvCore::Scripting::Lua
{
	std::string BuildDebugMessage(const std::string& p_format, sol::variadic_args p_args);
}