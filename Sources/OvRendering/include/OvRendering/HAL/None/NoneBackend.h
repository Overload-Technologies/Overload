/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TBackend.h>

namespace OvRendering::HAL
{
	struct NoneBackendContext {};
	using NoneBackend = TBackend<Settings::EGraphicsBackend::NONE, NoneBackendContext>;
}
