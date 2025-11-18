/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TUniformBuffer.h>
#include <OvRendering/HAL/None/NoneBuffer.h>

namespace OvRendering::HAL
{
	struct NoneUniformBufferContext {};
	using NoneUniformBuffer = TUniformBuffer<Settings::EGraphicsBackend::NONE, NoneUniformBufferContext, NoneBufferContext>;
}
