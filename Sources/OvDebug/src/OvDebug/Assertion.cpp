/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvDebug/Assertion.h"

#include <cassert>

void OvDebug::Assertion::Assert(bool p_condition, const std::string& p_message)
{
	assert(p_condition && p_message.c_str());
}
