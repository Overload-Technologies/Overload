/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <source_location>
#include <string>

#ifdef NDEBUG
#define OVASSERT(condition, message) static_cast<void>(0)
#else
#define OVASSERT(condition, message) \
	static_cast<bool>(condition) ? \
	static_cast<void>(0) : \
	OvDebug::_Assert(#condition, message, std::source_location::current());
#endif

namespace OvDebug
{
	void _Assert(
		const char* p_expression,
		const std::string_view p_message,
		const std::source_location& p_location
	);
}