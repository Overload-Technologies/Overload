/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvRendering::Settings
{
	/**
	* Enumeration of formats
	*/
	enum class EFormat : uint8_t
	{
		RED,
		RG,
		RGB,
		BGR,
		RGBA,
		BGRA,
		RED_INTEGER,
		RG_INTEGER,
		RGB_INTEGER,
		BGR_INTEGER,
		RGBA_INTEGER,
		BGRA_INTEGER,
		STENCIL_INDEX,
		DEPTH_COMPONENT,
		DEPTH_STENCIL
	};
}