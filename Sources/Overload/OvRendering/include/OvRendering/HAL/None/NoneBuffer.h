/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TBuffer.h>

namespace OvRendering::HAL
{
	struct NoneBufferContext
	{
		uint64_t allocatedBytes = 0;
		Settings::EBufferType type = Settings::EBufferType::UNKNOWN;
	};

	using NoneBuffer = TBuffer<Settings::EGraphicsBackend::NONE, NoneBufferContext>;
}
