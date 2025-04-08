/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <tracy/TracyOpenGL.hpp>
#define GPUZone(...) TracyGpuZone(__VA_ARGS__)
#else
#define GPUZone(...) (void)0
#endif // defined(GRAPHICS_API_OPENGL)
