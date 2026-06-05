/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/utils/BitmaskOperators.h>

#include <cstdint>
#include <limits>

namespace baregl::types
{
	/**
	* Enumeration of memory barrier flags
	*/
	enum class EMemoryBarrierFlags : uint16_t
	{
		VERTEX_ATTRIB_ARRAY = 0x1,
		ELEMENT_ARRAY = 0x2,
		UNIFORM = 0x4,
		TEXTURE_FETCH = 0x8,
		SHADER_IMAGE_ACCESS = 0x10,
		COMMAND = 0x20,
		PIXEL_BUFFER = 0x40,
		TEXTURE_UPDATE = 0x80,
		BUFFER_UPDATE = 0x100,
		CLIENT_MAPPED_BUFFER = 0x200,
		FRAMEBUFFER = 0x400,
		TRANSFORM_FEEDBACK = 0x800,
		ATOMIC_COUNTER = 0x1000,
		SHADER_STORAGE = 0x2000,
		QUERY_BUFFER = 0x4000,
		ALL = std::numeric_limits<uint16_t>::max()
	};
}

ENABLE_BITMASK_OPERATORS(baregl::types::EMemoryBarrierFlags);
