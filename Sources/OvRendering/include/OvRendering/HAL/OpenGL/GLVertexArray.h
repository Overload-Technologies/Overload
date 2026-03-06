/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TVertexArray.h>
#include <OvRendering/HAL/OpenGL/GLIndexBuffer.h>
#include <OvRendering/HAL/OpenGL/GLVertexBuffer.h>
#include <OvTools/Utils/OptRef.h>

namespace OvRendering::HAL
{
	struct GLVertexArrayContext
	{
		uint32_t id = 0;
		uint32_t attributeCount = 0;
	};

	using GLVertexArray = TVertexArray<
		Settings::EGraphicsBackend::OPENGL,
		GLVertexArrayContext,
		GLVertexBufferContext,
		GLIndexBufferContext,
		GLBufferContext
	>;
}
