/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TShaderStage.h>

namespace OvRendering::HAL
{
	struct NoneShaderStageContext {};
	using NoneShaderStage = TShaderStage<Settings::EGraphicsBackend::NONE, NoneShaderStageContext>;
}
