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
	* Enumeration of rasterization modes
	*/
	enum class ERasterizationMode : uint8_t
	{
		POINT,
		LINE,
		FILL
	};
}
