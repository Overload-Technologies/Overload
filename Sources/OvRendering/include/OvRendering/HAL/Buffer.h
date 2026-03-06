/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLBuffer.h>
#else
#include <OvRendering/HAL/None/NoneBuffer.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using Buffer = GLBuffer;
#else
	using Buffer = NoneBuffer;
#endif // defined(GRAPHICS_API_OPENGL)
}
