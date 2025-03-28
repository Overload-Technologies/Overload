/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLBackend.h>
#else
#include <OvRendering/HAL/None/NoneBackend.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using Backend = GLBackend;
#else
	using Backend = NoneBackend;
#endif // defined(GRAPHICS_API_OPENGL)
}
