/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLVertexBuffer.h>
#else
#include <OvRendering/HAL/None/NoneVertexBuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using VertexBuffer = GLVertexBuffer;
#else
	using VertexBuffer = NoneVertexBuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
