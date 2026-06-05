/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/ShaderStage.h>

#include <baregl/debug/Log.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>

#include <algorithm>

namespace baregl
{
	ShaderStage::ShaderStage(types::EShaderType p_type) :
		detail::NativeObject(glCreateShader(utils::EnumToValue<GLenum>(p_type))),
		m_type{ p_type }
	{
	}

	ShaderStage::~ShaderStage()
	{
		glDeleteShader(m_id);
	}

	void ShaderStage::Upload(const std::string& p_source) const
	{
		const char* source = p_source.c_str();
		glShaderSource(m_id, 1, &source, nullptr);
	}

	baregl::data::ShaderCompilationResult ShaderStage::Compile() const
	{
		glCompileShader(m_id);

		GLint compileStatus;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE)
		{
			GLint maxLength;
			glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

			std::string errorLog(maxLength, ' ');
			glGetShaderInfoLog(m_id, maxLength, &maxLength, errorLog.data());

			return {
				.success = false,
				.message = errorLog
			};
		}

		return {
			.success = true
		};
	}

	types::EShaderType ShaderStage::GetType() const
	{
		return m_type;
	}
}
