/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLShaderStage.h>
#else
#include <OvRendering/HAL/None/NoneShaderStage.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using ShaderStage = GLShaderStage;
#else
	using ShaderStage = NoneShaderStage;
#endif // defined(GRAPHICS_API_OPENGL)
}
