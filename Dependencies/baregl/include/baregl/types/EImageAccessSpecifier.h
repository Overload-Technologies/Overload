/**
* @project: baregl
* @author: Vaani
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace baregl::types
{
	/**
	* Defines access hints that images can use
	*/
	enum class EImageAccessSpecifier : uint8_t
	{
		READ_ONLY,
		WRITE_ONLY,
		READ_WRITE
	};
}
