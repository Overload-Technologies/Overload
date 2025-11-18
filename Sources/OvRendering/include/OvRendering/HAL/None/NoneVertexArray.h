/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TVertexArray.h>
#include <OvRendering/HAL/None/NoneIndexBuffer.h>
#include <OvRendering/HAL/None/NoneVertexBuffer.h>

namespace OvRendering::HAL
{
	struct NoneVertexArrayContext
	{
		uint32_t attributeCount = 0;
	};

	using NoneVertexArray = TVertexArray<
		Settings::EGraphicsBackend::NONE,
		NoneVertexArrayContext,
		NoneVertexBufferContext,
		NoneIndexBufferContext,
		NoneBufferContext
	>;
}
