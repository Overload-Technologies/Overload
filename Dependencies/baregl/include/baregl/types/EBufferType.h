/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace baregl::types
{
	/**
	* Enumeration of buffer types
	*/
	enum class EBufferType : uint8_t
	{
		VERTEX,
		INDEX,
		UNIFORM,
		SHADER_STORAGE,
		UNKNOWN
	};
}
