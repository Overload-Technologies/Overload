/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TIndexBuffer.h>
#include <OvRendering/HAL/OpenGL/GLBuffer.h>

namespace OvRendering::HAL
{
	struct GLIndexBufferContext {};
	using GLIndexBuffer = TIndexBuffer<Settings::EGraphicsBackend::OPENGL, GLIndexBufferContext, GLBufferContext>;
}
