/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/data/ShaderLinkingResult.h>
#include <baregl/data/UniformInfo.h>
#include <baregl/detail/NativeObject.h>
#include <baregl/math/Mat3.h>
#include <baregl/math/Mat4.h>
#include <baregl/math/Vec2.h>
#include <baregl/math/Vec3.h>
#include <baregl/math/Vec4.h>
#include <baregl/ShaderStage.h>

#include <optional>
#include <unordered_map>
#include <vector>

namespace baregl
{
	template<typename T>
	concept SupportedUniformType =
		std::same_as<T, float> ||
		std::same_as<T, int> ||
		std::same_as<T, unsigned int> ||
		std::same_as<T, math::Vec2> ||
		std::same_as<T, math::Vec3> ||
		std::same_as<T, math::Vec4> ||
		std::same_as<T, math::Mat3> ||
		std::same_as<T, math::Mat4>;

	/**
	* Represents a shader program, used to link shader stages together.
	*/
	class ShaderProgram final : public detail::NativeObject
	{
	public:
		/**
		* Creates a shader program.
		*/
		ShaderProgram();

		/**
		* Destroys a shader program.
		*/
		~ShaderProgram();

		/**
		* Attaches a shader stage to the program.
		* @param p_shader
		*/
		void Attach(const ShaderStage& p_shader);

		/**
		* Detaches a shader stage to the program.
		* @param p_shader
		*/
		void Detach(const ShaderStage& p_shader);

		/**
		* Detaches all shader stages from the program.
		*/
		void DetachAll();

		/**
		* Links the shader stages together.
		* @return The linking result
		*/
		baregl::data::ShaderLinkingResult Link();

		/**
		* Binds the program.
		*/
		void Bind() const;

		/**
		* Unbinds the program.
		*/
		void Unbind() const;

		/**
		* Sends a uniform value associated with the given name to the GPU.
		* @note The shader program must be bound before calling SetUniform
		* @param p_name
		* @param p_value
		*/
		template<SupportedUniformType T>
		void SetUniform(const std::string& p_name, const T& p_value);

		/**
		* Returns the value of a uniform associated with the given name.
		* @note The shader program must be bound before calling GetUniform
		* @param p_name
		* @param p_value
		*/
		template<SupportedUniformType T>
		T GetUniform(const std::string& p_name);

		/**
		* Returns information about the uniform identified by the given name or std::nullopt if not found.
		* @param p_name
		*/
		std::optional<std::reference_wrapper<const data::UniformInfo>> GetUniformInfo(const std::string& p_name) const;

		/**
		* Returns the uniforms associated with this program.
		*/
		const std::unordered_map<std::string, data::UniformInfo>& GetUniforms() const;

	private:
		void QueryUniforms();

	private:
		std::unordered_map<std::string, data::UniformInfo> m_uniforms;
		std::unordered_map<std::string, uint32_t> m_uniformsLocationCache;
		std::vector<std::reference_wrapper<const ShaderStage>> m_attachedShaders;
	};
}
