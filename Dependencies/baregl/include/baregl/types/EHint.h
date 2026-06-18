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
	* Hint behavior.
	*/
	enum class EHint : uint8_t
	{
		DONT_CARE,
		FASTEST,
		NICEST
	};
}
