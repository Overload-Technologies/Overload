/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string>

namespace baregl::data
{
	/**
	* Structure that holds the result of a shader compilation
	*/
	struct ShaderCompilationResult
	{
		bool success;
		std::string message;
	};
}
