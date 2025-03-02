/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>

#include <GL/glew.h>

#include <OvDebug/Logger.h>
#include <OvRendering/HAL/OpenGL/GLShaderProgram.h>
#include <OvRendering/HAL/OpenGL/GLTexture.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvRendering/Resources/Texture.h>

namespace
{
	constexpr bool IsEngineUBOMember(std::string_view p_name)
	{
		return p_name.starts_with("ubo_");
	}
}

template<>
OvRendering::HAL::GLShaderProgram::TShaderProgram() : m_context{
	.id = glCreateProgram()
}
{
}

template<>
OvRendering::HAL::GLShaderProgram::~TShaderProgram()
{
	glDeleteProgram(m_context.id);
}

template<>
void OvRendering::HAL::GLShaderProgram::Bind() const
{
	glUseProgram(m_context.id);
}

template<>
void OvRendering::HAL::GLShaderProgram::Unbind() const
{
	glUseProgram(0);
}

template<>
uint32_t OvRendering::HAL::GLShaderProgram::GetID() const
{
	return m_context.id;
}

template<>
void OvRendering::HAL::GLShaderProgram::Attach(const GLShaderStage& p_shader)
{
	glAttachShader(m_context.id, p_shader.GetID());
	m_context.attachedShaders.push_back(std::ref(p_shader));
}

template<>
void OvRendering::HAL::GLShaderProgram::Detach(const GLShaderStage& p_shader)
{
	glDetachShader(m_context.id, p_shader.GetID());
	m_context.attachedShaders.erase(std::remove_if(
		m_context.attachedShaders.begin(),
		m_context.attachedShaders.end(),
		[&p_shader](const std::reference_wrapper<const GLShaderStage> shader) {
			return shader.get().GetID() == p_shader.GetID();
		}
	));
}

template<>
void OvRendering::HAL::GLShaderProgram::DetachAll()
{
	for (auto& shader : m_context.attachedShaders)
	{
		glDetachShader(m_context.id, shader.get().GetID());
	}

	m_context.attachedShaders.clear();
}

template<>
OvRendering::Settings::ShaderLinkingResult OvRendering::HAL::GLShaderProgram::Link()
{
	glLinkProgram(m_context.id);

	GLint linkStatus;
	glGetProgramiv(m_context.id, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE)
	{
		GLint maxLength;
		glGetProgramiv(m_context.id, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorLog(maxLength, ' ');
		glGetProgramInfoLog(m_context.id, maxLength, &maxLength, errorLog.data());

		return {
			.success = false,
			.message = errorLog
		};
	}

	QueryUniforms();

	return {
		.success = true
	};
}


#define DECLARE_GET_UNIFORM_FUNCTION(type, glType, func) \
template<> \
template<> \
type OvRendering::HAL::GLShaderProgram::GetUniform<type>(std::string_view p_name) \
{ \
	type result; \
	if (const uint32_t location = m_context.GetUniformLocation(p_name); location != -1) \
	{ \
		func(m_context.id, location, reinterpret_cast<glType*>(&result)); \
	} \
	return result; \
}

DECLARE_GET_UNIFORM_FUNCTION(int, GLint, glGetUniformiv);
DECLARE_GET_UNIFORM_FUNCTION(float, GLfloat, glGetUniformfv);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector2, GLfloat, glGetUniformfv);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector3, GLfloat, glGetUniformfv);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FVector4, GLfloat, glGetUniformfv);
DECLARE_GET_UNIFORM_FUNCTION(OvMaths::FMatrix4, GLfloat, glGetUniformfv);

#define DECLARE_SET_UNIFORM_FUNCTION(type, func, ...) \
template<> \
template<> \
void OvRendering::HAL::GLShaderProgram::SetUniform<type>(std::string_view p_name, const type& value) \
{ \
	if (const uint32_t location = m_context.GetUniformLocation(p_name); location != -1) \
	{ \
		func(location, __VA_ARGS__); \
	} \
}

DECLARE_SET_UNIFORM_FUNCTION(int, glUniform1i, value);
DECLARE_SET_UNIFORM_FUNCTION(float, glUniform1f, value);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector2, glUniform2f, value.x, value.y);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector3, glUniform3f, value.x, value.y, value.z);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector4, glUniform4f, value.x, value.y, value.z, value.w);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FMatrix4, glUniformMatrix4fv, 1, GL_TRUE, &value.data[0]);

uint32_t OvRendering::HAL::GLShaderProgramContext::GetUniformLocation(std::string_view p_name)
{
	if (uniformLocationCache.find(p_name.data()) != uniformLocationCache.end())
	{
		return uniformLocationCache.at(p_name.data());
	}

	const GLint location = glGetUniformLocation(id, p_name.data());

	if (location == -1)
	{
		OVLOG_WARNING("Uniform: '" + std::string{ p_name } + "' doesn't exist");
	}

	uniformLocationCache[p_name.data()] = location;

	return location;
}

template<>
void OvRendering::HAL::GLShaderProgram::QueryUniforms()
{
	m_context.uniforms.clear();

	std::array<GLchar, 256> nameBuffer;

	GLint activeUniformCount = 0;
	glGetProgramiv(m_context.id, GL_ACTIVE_UNIFORMS, &activeUniformCount);

	for (GLint i = 0; i < activeUniformCount; ++i)
	{
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;

		glGetActiveUniform(m_context.id, i, static_cast<GLsizei>(nameBuffer.size()), &actualLength, &arraySize, &type, nameBuffer.data());

		const auto name = std::string{ nameBuffer.data(), static_cast<size_t>(actualLength) };
		const auto uniformType = ValueToEnum<Settings::EUniformType>(type);

		if (!IsEngineUBOMember(name))
		{
			std::any defaultValue;

			switch (uniformType)
			{
			case Settings::EUniformType::BOOL: defaultValue = GetUniform<int>(name); break;
			case Settings::EUniformType::INT: defaultValue = GetUniform<int>(name); break;
			case Settings::EUniformType::FLOAT: defaultValue = GetUniform<float>(name); break;
			case Settings::EUniformType::FLOAT_VEC2: defaultValue = GetUniform<OvMaths::FVector2>(name); break;
			case Settings::EUniformType::FLOAT_VEC3: defaultValue = GetUniform<OvMaths::FVector3>(name); break;
			case Settings::EUniformType::FLOAT_VEC4: defaultValue = GetUniform<OvMaths::FVector4>(name); break;
			case Settings::EUniformType::FLOAT_MAT4: defaultValue = GetUniform<OvMaths::FMatrix4>(name); break;
			case Settings::EUniformType::SAMPLER_2D: defaultValue = std::make_any<Resources::Texture*>(nullptr); break;
			}

			if (defaultValue.has_value())
			{
				m_context.uniforms.push_back(Settings::UniformInfo{
					.type = uniformType,
					.name = name,
					.location = m_context.GetUniformLocation(name),
					.defaultValue = defaultValue
				});
			}
		}
	}
}

template<>
OvTools::Utils::OptRef<const OvRendering::Settings::UniformInfo> OvRendering::HAL::GLShaderProgram::GetUniformInfo(std::string_view p_name) const
{
	if (const auto found = std::ranges::find_if(m_context.uniforms, [p_name](const auto& e) {
		return p_name.compare(e.name) == 0;
	}); found != m_context.uniforms.end())
	{
		return *found;
	}

	return std::nullopt;
}

template<>
std::span<const OvRendering::Settings::UniformInfo> OvRendering::HAL::GLShaderProgram::GetUniforms() const
{
	return m_context.uniforms;
}
