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
	* Blending equations used by the blending operations
	*/
	enum class EBlendingEquation : uint8_t
	{
		FUNC_ADD,
		FUNC_SUBTRACT,
		FUNC_REVERSE_SUBTRACT,
		MIN,
		MAX
	};
}
