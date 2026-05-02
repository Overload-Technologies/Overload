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
		std::unordered_map<std::string, Settings::UniformInfo> uniforms;
		std::unordered_map<std::string, uint32_t> uniformsLocationCache;
		std::vector<std::reference_wrapper<const GLShaderStage>> attachedShaders;

		uint32_t GetUniformLocation(std::string_view p_name);
	};

	using GLShaderProgram = TShaderProgram<Settings::EGraphicsBackend::OPENGL, GLShaderProgramContext, GLShaderStageContext>;

	/**
	* Invalidate the shader program binding cache.
	* Useful when external code binds programs directly through OpenGL calls.
	*/
	void InvalidateGLShaderProgramBindingCache();
}
