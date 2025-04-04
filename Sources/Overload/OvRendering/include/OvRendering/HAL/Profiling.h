/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <tracy/TracyOpenGL.hpp>
#define OvGpuZone(...) TracyGpuZone(__VA_ARGS__)
#else
#define OvGpuZone(...) (void)0
#endif // defined(GRAPHICS_API_OPENGL)
