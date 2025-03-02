/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <memory>

#include <OvRendering/HAL/ShaderProgram.h>

namespace OvRendering::Resources
{
	namespace Loaders { class ShaderLoader; }

	/**
	* Represents a shader resource, which wraps a shader program and adds a path to it.
	* Can be seen as a "Shader Asset".
	*/
	class Shader
	{
		friend class Loaders::ShaderLoader;

	public:
		/**
		* Returns the associated shader program
		*/
		HAL::ShaderProgram& GetProgram() const;

	private:
		Shader(const std::string p_path, std::unique_ptr<HAL::ShaderProgram>&& p_program);
		~Shader() = default;
		void SetProgram(std::unique_ptr<HAL::ShaderProgram>&& p_program);

	public:
		const std::string path;

	private:
		std::unique_ptr<HAL::ShaderProgram> m_program;
	};
}