/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLIndexBuffer.h>
#else
#include <OvRendering/HAL/None/NoneIndexBuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using IndexBuffer = GLIndexBuffer;
#else
	using IndexBuffer = NoneIndexBuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
