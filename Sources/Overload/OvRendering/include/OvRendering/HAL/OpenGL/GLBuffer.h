/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TBuffer.h>

namespace OvRendering::HAL
{
	struct GLBufferContext
	{
		uint32_t id = 0;
		Settings::EBufferType type = Settings::EBufferType::UNKNOWN;
		uint64_t allocatedBytes = 0;
	};
	
	using GLBuffer = TBuffer<Settings::EGraphicsBackend::OPENGL, GLBufferContext>;
}
