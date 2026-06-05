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
	* Enumeration of uniform types
	*/
	enum class EUniformType : uint8_t
	{
		BOOL,
		INT,
		UNSIGNED_INT,
		FLOAT,
		FLOAT_VEC2,
		FLOAT_VEC3,
		FLOAT_VEC4,
		FLOAT_MAT3,
		FLOAT_MAT4,
		DOUBLE_MAT4,
		SAMPLER_2D,
		SAMPLER_CUBE,
		IMAGE_2D,
		IMAGE_CUBE
	};
}
