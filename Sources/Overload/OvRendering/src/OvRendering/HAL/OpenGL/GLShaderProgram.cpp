/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>

#include <glad.h>

#include <OvDebug/Assertion.h>
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
OvRendering::HAL::GLShaderProgram::TShaderProgram() : m_context{ .id = glCreateProgram() }
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
type OvRendering::HAL::GLShaderProgram::GetUniform<type>(const std::string& p_name) \
{ \
	type result{}; \
	if (m_context.uniformsLocationCache.contains(p_name)) \
	{ \
		if (const uint32_t location = m_context.uniformsLocationCache.at(p_name)) \
		{ \
			func(m_context.id, location, reinterpret_cast<glType*>(&result)); \
		} \
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
void OvRendering::HAL::GLShaderProgram::SetUniform<type>(const std::string& p_name, const type& value) \
{ \
	if (m_context.uniformsLocationCache.contains(p_name)) \
	{ \
		func(m_context.uniformsLocationCache.at(p_name), __VA_ARGS__); \
	} \
}

DECLARE_SET_UNIFORM_FUNCTION(int, glUniform1i, value);
DECLARE_SET_UNIFORM_FUNCTION(float, glUniform1f, value);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector2, glUniform2f, value.x, value.y);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector3, glUniform3f, value.x, value.y, value.z);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FVector4, glUniform4f, value.x, value.y, value.z, value.w);
DECLARE_SET_UNIFORM_FUNCTION(OvMaths::FMatrix4, glUniformMatrix4fv, 1, GL_TRUE, &value.data[0]);

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

		// Skip reserved uniforms (e.g., ubo_ prefix)
		if (IsEngineUBOMember(name))
		{
			continue;
		}

		const auto location = glGetUniformLocation(m_context.id, name.c_str());
		OVASSERT(location != -1, "Failed to get uniform location for: " + name);
		m_context.uniformsLocationCache.emplace(name, static_cast<uint32_t>(location));

		const std::any uniformValue = [&]() -> std::any {
			switch (uniformType)
			{
				using enum Settings::EUniformType;
				case BOOL: return static_cast<bool>(GetUniform<int>(name));
				case INT: return GetUniform<int>(name);
				case FLOAT: return GetUniform<float>(name);
				case FLOAT_VEC2: return GetUniform<OvMaths::FVector2>(name);
				case FLOAT_VEC3: return GetUniform<OvMaths::FVector3>(name);
				case FLOAT_VEC4: return GetUniform<OvMaths::FVector4>(name);
				case FLOAT_MAT4: return GetUniform<OvMaths::FMatrix4>(name);
				case SAMPLER_2D: return std::make_any<Resources::Texture*>(nullptr);
				case SAMPLER_CUBE: return std::make_any<Resources::Texture*>(nullptr);
				default: return std::nullopt;
			}
		}();

		// Only add the uniform if it has a value (unsupported uniform types will be ignored)
		if (uniformValue.has_value())
		{
			m_context.uniforms.emplace(name, Settings::UniformInfo{
				.type = uniformType,
				.name = name,
				.defaultValue = uniformValue
			});
		}
	}
}

template<>
OvTools::Utils::OptRef<const OvRendering::Settings::UniformInfo> OvRendering::HAL::GLShaderProgram::GetUniformInfo(const std::string& p_name) const
{
	if (m_context.uniforms.contains(p_name))
	{
		return m_context.uniforms.at(p_name);
	}

	return std::nullopt;
}

template<>
const std::unordered_map<std::string, OvRendering::Settings::UniformInfo>& OvRendering::HAL::GLShaderProgram::GetUniforms() const
{
	return m_context.uniforms;
}
