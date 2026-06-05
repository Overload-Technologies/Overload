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
	* Enumeration of texture types
	*/
	enum class ETextureType : uint8_t
	{
		TEXTURE_2D,
		TEXTURE_CUBE
	};
}
