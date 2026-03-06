/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLRenderbuffer.h>
#else
#include <OvRendering/HAL/None/NoneRenderbuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using Renderbuffer = GLRenderbuffer;
#else
	using Renderbuffer = NoneRenderbuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
