/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string_view>

#if defined(DEBUG) || defined(_DEBUG)
	#define BAREGL_ASSERT(condition, message) baregl::debug::Assert(condition, message)
#else
	#define BAREGL_ASSERT(condition, message) ((void)(condition), (void)(message))
#endif

namespace baregl::debug
{
	void Assert(bool condition, const std::string_view p_message = {});
}
