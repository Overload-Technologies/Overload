/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TShaderStage.h>

namespace OvRendering::HAL
{
	struct GLShaderStageContext
	{
		uint32_t id;
		Settings::EShaderType type;
	};

	using GLShaderStage = TShaderStage<Settings::EGraphicsBackend::OPENGL, GLShaderStageContext>;
}
