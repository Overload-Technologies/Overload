/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TShaderStorageBuffer.h>
#include <OvRendering/HAL/None/NoneBuffer.h>

namespace OvRendering::HAL
{
	struct NoneShaderStorageBufferContext {};
	using NoneShaderStorageBuffer = TShaderStorageBuffer<Settings::EGraphicsBackend::NONE, NoneShaderStorageBufferContext, NoneBufferContext>;
}
