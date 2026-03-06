/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>

#include <cassert>
#include <format>

#if defined(_MSC_VER)
#   define OV_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#   define OV_DEBUG_BREAK() __builtin_trap()
#else
#   define OV_DEBUG_BREAK() std::abort()
#endif

void OvDebug::_Assert(const char* p_expression, const std::string_view p_message, const std::source_location& p_location)
{
	OVLOG_ERROR(std::format(
		"Assertion failed: {}\n"
		"  Expression: {}\n"
		"  Function: {}\n"
		"  Location: {}:{}", 
		p_message, p_expression, p_location.function_name(), 
		p_location.file_name(), p_location.line()));
	OV_DEBUG_BREAK();
}