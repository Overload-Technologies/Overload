/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLFramebuffer.h>
#else
#include <OvRendering/HAL/None/NoneFramebuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using Framebuffer = GLFramebuffer;
#else
	using Framebuffer = NoneFramebuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
