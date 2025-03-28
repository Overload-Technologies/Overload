/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <unordered_map>

#include <OvRendering/HAL/Common/TShaderProgram.h>
#include <OvRendering/HAL/OpenGL/GLShaderStage.h>

namespace OvRendering::HAL
{
	struct GLShaderProgramContext
	{
		const uint32_t id;
		std::vector<Settings::UniformInfo> uniforms;
		std::vector<std::reference_wrapper<const GLShaderStage>> attachedShaders;
		std::unordered_map<std::string, int> uniformLocationCache;

		uint32_t GetUniformLocation(std::string_view p_name);
	};

	using GLShaderProgram = TShaderProgram<Settings::EGraphicsBackend::OPENGL, GLShaderProgramContext, GLShaderStageContext>;
}
