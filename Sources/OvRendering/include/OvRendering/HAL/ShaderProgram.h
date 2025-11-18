/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLShaderProgram.h>
#else
#include <OvRendering/HAL/None/NoneShaderProgram.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using ShaderProgram = GLShaderProgram;
#else
	using ShaderProgram = NoneShaderProgram;
#endif // defined(GRAPHICS_API_OPENGL)
}
