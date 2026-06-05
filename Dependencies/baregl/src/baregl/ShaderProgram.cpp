/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/ShaderProgram.h>

#include <baregl/debug/Assert.h>
#include <baregl/debug/Log.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>
#include <baregl/Texture.h>

#include <algorithm>
#include <array>
#include <functional>

namespace baregl
{
	ShaderProgram::ShaderProgram() :
		detail::NativeObject(glCreateProgram())
	{
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_id);
	}

	void ShaderProgram::Bind() const
	{
		glUseProgram(m_id);
	}

	void ShaderProgram::Unbind() const
	{
		glUseProgram(0);
	}

	void ShaderProgram::Attach(const ShaderStage& p_shader)
	{
		glAttachShader(m_id, p_shader.GetID());
		m_attachedShaders.push_back(std::ref(p_shader));
	}

	void ShaderProgram::Detach(const ShaderStage& p_shader)
	{
		glDetachShader(m_id, p_shader.GetID());
		m_attachedShaders.erase(std::remove_if(
			m_attachedShaders.begin(),
			m_attachedShaders.end(),
			[&p_shader](const std::reference_wrapper<const ShaderStage> shader) {
				return shader.get().GetID() == p_shader.GetID();
			}
		));
	}

	void ShaderProgram::DetachAll()
	{
		for (auto& shader : m_attachedShaders)
		{
			glDetachShader(m_id, shader.get().GetID());
		}

		m_attachedShaders.clear();
	}

	baregl::data::ShaderLinkingResult ShaderProgram::Link()
	{
		glLinkProgram(m_id);

		GLint linkStatus;
		glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

			std::string errorLog(maxLength, ' ');
			glGetProgramInfoLog(m_id, maxLength, &maxLength, errorLog.data());

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
type ShaderProgram::GetUniform<type>(const std::string& p_name) \
{ \
	type result{}; \
	if (auto it = m_uniformsLocationCache.find(p_name); it != m_uniformsLocationCache.end()) \
	{ \
		func(m_id, it->second, reinterpret_cast<glType*>(&result)); \
	} \
	return result; \
}

	DECLARE_GET_UNIFORM_FUNCTION(int, GLint, glGetUniformiv);
	DECLARE_GET_UNIFORM_FUNCTION(unsigned int, GLuint, glGetUniformuiv);
	DECLARE_GET_UNIFORM_FUNCTION(float, GLfloat, glGetUniformfv);
	DECLARE_GET_UNIFORM_FUNCTION(math::Vec2, GLfloat, glGetUniformfv);
	DECLARE_GET_UNIFORM_FUNCTION(math::Vec3, GLfloat, glGetUniformfv);
	DECLARE_GET_UNIFORM_FUNCTION(math::Vec4, GLfloat, glGetUniformfv);
	DECLARE_GET_UNIFORM_FUNCTION(math::Mat3, GLfloat, glGetUniformfv);
	DECLARE_GET_UNIFORM_FUNCTION(math::Mat4, GLfloat, glGetUniformfv);

#define DECLARE_SET_UNIFORM_FUNCTION(type, func, ...) \
template<> \
void ShaderProgram::SetUniform<type>(const std::string& p_name, const type& value) \
{ \
	if (auto it = m_uniformsLocationCache.find(p_name); it != m_uniformsLocationCache.end()) \
	{ \
		func(it->second, __VA_ARGS__); \
	} \
}

	DECLARE_SET_UNIFORM_FUNCTION(int, glUniform1i, value);
	DECLARE_SET_UNIFORM_FUNCTION(unsigned int, glUniform1ui, value);
	DECLARE_SET_UNIFORM_FUNCTION(float, glUniform1f, value);
	DECLARE_SET_UNIFORM_FUNCTION(math::Vec2, glUniform2f, value.x, value.y);
	DECLARE_SET_UNIFORM_FUNCTION(math::Vec3, glUniform3f, value.x, value.y, value.z);
	DECLARE_SET_UNIFORM_FUNCTION(math::Vec4, glUniform4f, value.x, value.y, value.z, value.w);
	DECLARE_SET_UNIFORM_FUNCTION(math::Mat3, glUniformMatrix3fv, 1, GL_FALSE, &value[0][0]);
	DECLARE_SET_UNIFORM_FUNCTION(math::Mat4, glUniformMatrix4fv, 1, GL_FALSE, &value[0][0]);

	std::optional<std::reference_wrapper<const baregl::data::UniformInfo>> ShaderProgram::GetUniformInfo(const std::string& p_name) const
	{
		if (m_uniforms.contains(p_name))
		{
			return m_uniforms.at(p_name);
		}

		return std::nullopt;
	}

	const std::unordered_map<std::string, baregl::data::UniformInfo>& ShaderProgram::GetUniforms() const
	{
		return m_uniforms;
	}

	void ShaderProgram::QueryUniforms()
	{
		m_uniforms.clear();

		std::array<GLchar, 256> nameBuffer;

		GLint activeUniformCount = 0;
		glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &activeUniformCount);

		uint32_t textureIndexCounter = 0;

		for (GLint i = 0; i < activeUniformCount; ++i)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;

			glGetActiveUniform(m_id, i, static_cast<GLsizei>(nameBuffer.size()), &actualLength, &arraySize, &type, nameBuffer.data());

			const auto name = std::string{ nameBuffer.data(), static_cast<size_t>(actualLength) };
			const auto uniformType = utils::ValueToEnum<types::EUniformType>(type);
			const auto location = glGetUniformLocation(m_id, name.c_str());

			if (location == -1)
			{
				continue; // Skip uniforms that don't have a valid location (e.g. uniform buffer members)
			}

			m_uniformsLocationCache.emplace(name, static_cast<uint32_t>(location));

			const std::any uniformValue = [&]() -> std::any {
				switch (uniformType)
				{
					using enum types::EUniformType;
				case BOOL: return static_cast<bool>(GetUniform<int>(name));
				case INT: return GetUniform<int>(name);
				case UNSIGNED_INT: return GetUniform<unsigned int>(name);
				case FLOAT: return GetUniform<float>(name);
				case FLOAT_VEC2: return GetUniform<math::Vec2>(name);
				case FLOAT_VEC3: return GetUniform<math::Vec3>(name);
				case FLOAT_VEC4: return GetUniform<math::Vec4>(name);
				case FLOAT_MAT3: return GetUniform<math::Mat3>(name);
				case FLOAT_MAT4: return GetUniform<math::Mat4>(name);
				case SAMPLER_2D: return std::make_any<Texture*>(nullptr);
				case SAMPLER_CUBE: return std::make_any<Texture*>(nullptr);
				default: return std::nullopt;
				}
			}();

			// Only add the uniform if it has a value (unsupported uniform types will be ignored)
			if (uniformValue.has_value())
			{
				const bool isTexture =
					uniformType == types::EUniformType::SAMPLER_2D ||
					uniformType == types::EUniformType::SAMPLER_CUBE;

				m_uniforms.emplace(name, baregl::data::UniformInfo{
					.type = uniformType,
					.name = name,
					.defaultValue = uniformValue,
					.textureIndex = isTexture ? textureIndexCounter++ : 0
				});
			}
		}
	}
}
