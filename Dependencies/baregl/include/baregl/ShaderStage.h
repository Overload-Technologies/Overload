/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/data/ShaderCompilationResult.h>
#include <baregl/detail/NativeObject.h>
#include <baregl/types/EShaderType.h>

namespace baregl
{
	/**
	* Represents a part of a shader program that is responsible for a specific stage (vertex, fragment, geometry, etc.).
	*/
	class ShaderStage final : public detail::NativeObject
	{
	public:
		/**
		* Creates a shader stage of the given type.
		* @param p_type
		*/
		ShaderStage(types::EShaderType p_type);

		/**
		* Destructor of the shader stage.
		*/
		~ShaderStage();

		/**
		* Uploads the shader source to the graphics context memory.
		* @param p_source
		*/
		void Upload(const std::string& p_source) const;

		/**
		* Compiles the uploaded shader source.
		* @note Use this method after uploading the shader source.
		* @return The compilation result.
		*/
		data::ShaderCompilationResult Compile() const;

		/**
		* Returns the type of shader stage.
		*/
		types::EShaderType GetType() const;

	private:
		types::EShaderType m_type;
	};
}
