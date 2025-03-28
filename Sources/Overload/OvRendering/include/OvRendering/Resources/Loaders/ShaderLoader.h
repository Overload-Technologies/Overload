/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>

#include "OvRendering/Resources/Shader.h"

namespace OvRendering::Resources::Loaders
{
	/**
	* Handle the Shader creation and destruction
	*/
	class ShaderLoader
	{
	public:
		using FilePathParserCallback = std::function<std::string(const std::string&)>;

		/**
		* Disabled constructor
		*/
		ShaderLoader() = delete;

		/**
		* Create a shader
		* @param p_filePath
		* @param p_pathParser
		*/
		static Shader* Create(const std::string& p_filePath, FilePathParserCallback p_pathParser = nullptr);

		/**
		* Create a shader from source
		* @param p_vertexShader
		* @param p_fragmentShader
		* @note Doesn't support path parsing/resolving
		*/
		static Shader* CreateFromSource(const std::string& p_vertexShader, const std::string& p_fragmentShader);

		/**
		* Recompile a shader
		* @param p_shader
		* @param p_filePath
		*/
		static void	Recompile(Shader& p_shader, const std::string& p_filePath, FilePathParserCallback p_pathParser = nullptr);

		/**
		* Destroy a shader
		* @param p_shader
		*/
		static bool Destroy(Shader*& p_shader);
	};
}