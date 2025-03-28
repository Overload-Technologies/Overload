/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TShaderStorageBuffer.h>
#include <OvRendering/HAL/OpenGL/GLBuffer.h>

namespace OvRendering::HAL
{
	struct GLShaderStorageBufferContext
	{
		uint32_t id = 0;
	};

	using GLShaderStorageBuffer = TShaderStorageBuffer<Settings::EGraphicsBackend::OPENGL, GLShaderStorageBufferContext, GLBufferContext>;
}
