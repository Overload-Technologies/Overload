/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLVertexArray.h>
#else
#include <OvRendering/HAL/None/NoneVertexArray.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using VertexArray = GLVertexArray;
#else
	using VertexArray = NoneVertexArray;
#endif // defined(GRAPHICS_API_OPENGL)
}
