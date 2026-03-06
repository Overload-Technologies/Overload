/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLUniformBuffer.h>
#else
#include <OvRendering/HAL/None/NoneUniformBuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using UniformBuffer = GLUniformBuffer;
#else
	using UniformBuffer = NoneUniformBuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
