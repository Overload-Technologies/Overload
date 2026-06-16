/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/utils/BitmaskOperators.h>

#include <cstdint>

namespace baregl::types
{
	/**
	* OpenGL context creation flags.
	*/
	enum class EContextFlags : uint32_t
	{
		NONE = 0,
		FORWARD_COMPATIBLE = 0x00000001,
		DEBUG_BIT = 0x00000002,
		ROBUST_ACCESS = 0x00000004,
		NO_ERROR = 0x00000008
	};
}

ENABLE_BITMASK_OPERATORS(baregl::types::EContextFlags);
