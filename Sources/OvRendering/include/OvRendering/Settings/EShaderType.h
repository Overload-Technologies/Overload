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
	* OpenGL rasterization mode enum wrapper
	*/
	enum class EShaderType : uint8_t
	{
		NONE,
		VERTEX,
		FRAGMENT,
	};
}
