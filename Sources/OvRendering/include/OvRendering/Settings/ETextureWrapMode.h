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
	* Enumeration of texture wrap modes
	*/
	enum class ETextureWrapMode : uint8_t
	{
		REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRRORED_REPEAT,
		MIRROR_CLAMP_TO_EDGE
	};
}