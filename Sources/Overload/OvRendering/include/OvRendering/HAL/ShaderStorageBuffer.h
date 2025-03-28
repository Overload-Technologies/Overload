/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLShaderStorageBuffer.h>
#else
#include <OvRendering/HAL/None/NoneShaderStorageBuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using ShaderStorageBuffer = GLShaderStorageBuffer;
#else
	using ShaderStorageBuffer = NoneShaderStorageBuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
