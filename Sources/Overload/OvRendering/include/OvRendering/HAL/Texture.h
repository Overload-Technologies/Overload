/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLTexture.h>
#else
#include <OvRendering/HAL/None/NoneTexture.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using Texture = GLTexture;
#else
	using Texture = NoneTexture;
#endif // defined(GRAPHICS_API_OPENGL)
}
