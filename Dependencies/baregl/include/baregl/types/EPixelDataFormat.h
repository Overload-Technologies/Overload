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
	* Enumeration of pixel data formats
	*/
	enum class EPixelDataFormat : uint8_t
	{
		COLOR_INDEX,
		STENCIL_INDEX,
		DEPTH_COMPONENT,
		RED,
		GREEN,
		BLUE,
		ALPHA,
		RGB,
		BGR,
		RGBA,
		BGRA,
		LUMINANCE,
		LUMINANCE_ALPHA,
	};
}
