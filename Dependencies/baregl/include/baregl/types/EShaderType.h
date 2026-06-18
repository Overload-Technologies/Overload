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
	* Enumeration of shader types
	*/
	enum class EShaderType : uint8_t
	{
		NONE,
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		COMPUTE,
		TESSELLATION_CONTROL,
		TESSELLATION_EVALUATION
	};
}
