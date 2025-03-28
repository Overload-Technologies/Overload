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
	* Enumeration of uniform types
	*/
	enum class EUniformType : uint8_t
	{
		BOOL,
		INT,
		FLOAT,
		FLOAT_VEC2,
		FLOAT_VEC3,
		FLOAT_VEC4,
		FLOAT_MAT4,
		DOUBLE_MAT4,
		SAMPLER_2D,
		SAMPLER_CUBE
	};
}
