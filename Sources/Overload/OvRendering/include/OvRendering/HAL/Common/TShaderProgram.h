/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <span>

#include <OvMaths/FMatrix4.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FVector4.h>

#include <OvRendering/HAL/ShaderStage.h>
#include <OvRendering/Settings/EGraphicsBackend.h>
#include <OvRendering/Settings/UniformInfo.h>
#include <OvRendering/Settings/ShaderLinkingResult.h>

#include <OvTools/Utils/OptRef.h>

namespace OvRendering::HAL
{
	template<typename T>
	concept SupportedUniformType =
		std::same_as<T, float> ||
		std::same_as<T, int> ||
		std::same_as<T, OvMaths::FVector2> ||
		std::same_as<T, OvMaths::FVector3> ||
		std::same_as<T, OvMaths::FVector4> ||
		std::same_as<T, OvMaths::FMatrix4>;

	/**
	* Represents a shader program, used to link shader stages together.
	*/
	template<Settings::EGraphicsBackend Backend, class ProgramContext, class StageContext>
	class TShaderProgram final
	{
	public:
		/**
		* Creates a shader program.
		*/
		TShaderProgram();

		/**
		* Destroys a shader program.
		*/
		~TShaderProgram();

		/**
		* Attaches a shader stage to the program.
		* @param p_shader
		*/
		void Attach(const TShaderStage<Backend, StageContext>& p_shader);

		/**
		* Detaches a shader stage to the program.
		* @param p_shader
		*/
		void Detach(const TShaderStage<Backend, StageContext>& p_shader);

		/**
		* Detaches all shader stages from the program.
		*/
		void DetachAll();

		/**
		* Links the shader stages together.
		* @return The linking result
		*/
		Settings::ShaderLinkingResult Link();

		/**
		* Binds the program.
		*/
		void Bind() const;

		/**
		* Unbinds the program.
		*/
		void Unbind() const;

		/**
		* Returns the ID of the program.
		*/
		uint32_t GetID() const;

		/**
		* Sends a uniform value associated with the given name to the GPU.
		* @param p_name
		* @param p_value
		*/
		template<SupportedUniformType T>
		void SetUniform(std::string_view p_name, const T& p_value);

		/**
		* Returns the value of a uniform associated with the given name.
		* @param p_name
		* @param p_value
		*/
		template<SupportedUniformType T>
		T GetUniform(std::string_view p_name);

		/**
		* Returns information about the uniform identified by the given name or std::nullopt if not found.
		* @param p_name
		*/
		OvTools::Utils::OptRef<const Settings::UniformInfo> GetUniformInfo(std::string_view p_name) const;

		/**
		* Queries the uniforms from the program and caches them in memory.
		*/
		void QueryUniforms();

		/**
		* Returns the uniforms associated with this program.
		*/
		std::span<const Settings::UniformInfo> GetUniforms() const;

	private:
		ProgramContext m_context;
	};
}
