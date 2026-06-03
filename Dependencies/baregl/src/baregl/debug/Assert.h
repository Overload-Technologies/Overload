/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string_view>

#define BAREGL_ASSERT(condition, message) baregl::debug::Assert(condition, message)

namespace baregl::debug
{
	void Assert(bool condition, const std::string_view p_message = {});
}
