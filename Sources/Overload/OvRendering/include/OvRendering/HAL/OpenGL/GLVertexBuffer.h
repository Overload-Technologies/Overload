/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TVertexBuffer.h>
#include <OvRendering/HAL/OpenGL/GLBuffer.h>

namespace OvRendering::HAL
{
	struct GLVertexBufferContext{};
	using GLVertexBuffer = TVertexBuffer<Settings::EGraphicsBackend::OPENGL, GLVertexBufferContext, GLBufferContext>;
}
