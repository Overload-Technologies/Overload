/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <OvRendering/HAL/OpenGL/GLTextureHandle.h>
#else
#include <OvRendering/HAL/None/NoneTextureHandle.h>
#endif // defined(GRAPHICS_API_OPENGL)

namespace OvRendering::HAL
{
#if defined(GRAPHICS_API_OPENGL)
	using TextureHandle = GLTextureHandle;
#else
	using TextureHandle = NoneTextureHandle;
#endif // defined(GRAPHICS_API_OPENGL)
}
