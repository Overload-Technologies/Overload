/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <GL/glew.h>

#include <OvDebug/Logger.h>
#include <OvRendering/HAL/OpenGL/GLShaderStage.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvRendering/Utils/ShaderUtil.h>

template<>
OvRendering::HAL::GLShaderStage::TShaderStage(Settings::EShaderType p_type) : m_context{
	.id = static_cast<uint32_t>(glCreateShader(EnumToValue<GLenum>(p_type))),
	.type = p_type,
}
{
}

template<>
OvRendering::HAL::GLShaderStage::~TShaderStage()
{
	glDeleteShader(m_context.id);
}

template<>
void OvRendering::HAL::GLShaderStage::Upload(const std::string& p_source) const
{
	const char* source = p_source.c_str();
	glShaderSource(m_context.id, 1, &source, nullptr);
}

template<>
OvRendering::Settings::ShaderCompilationResult OvRendering::HAL::GLShaderStage::Compile() const
{
	glCompileShader(m_context.id);

	GLint compileStatus;
	glGetShaderiv(m_context.id, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		GLint maxLength;
		glGetShaderiv(m_context.id, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorLog(maxLength, ' ');
		glGetShaderInfoLog(m_context.id, maxLength, &maxLength, errorLog.data());

		std::string shaderTypeStr = Utils::GetShaderTypeName(m_context.type);
		std::transform(shaderTypeStr.begin(), shaderTypeStr.end(), shaderTypeStr.begin(), std::toupper);
		std::string errorHeader = "[" + shaderTypeStr + " SHADER] \"";

		return {
			.success = false,
			.message = errorLog
		};
	}

	return {
		.success = true
	};
}

template<>
uint32_t OvRendering::HAL::GLShaderStage::GetID() const
{
	return m_context.id;
}
