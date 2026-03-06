/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

namespace OvRendering::Settings
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
