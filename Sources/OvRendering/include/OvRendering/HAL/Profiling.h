/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#if defined(GRAPHICS_API_OPENGL)
#include <glad.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>
#else
#undef TRACY_ENABLE // Disable tracy GPU profiling if not using OpenGL
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>
#endif // defined(GRAPHICS_API_OPENGL)
