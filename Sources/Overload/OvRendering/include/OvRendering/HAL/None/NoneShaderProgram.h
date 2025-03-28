/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TShaderProgram.h>
#include <OvRendering/HAL/None/NoneShaderStage.h>

namespace OvRendering::HAL
{
	struct NoneShaderProgramContext {};
	using NoneShaderProgram = TShaderProgram<Settings::EGraphicsBackend::NONE, NoneShaderProgramContext, NoneShaderStageContext>;
}
