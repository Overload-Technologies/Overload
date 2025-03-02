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
	* Structure that contains the result of a shader linking operation
	*/
	struct ShaderLinkingResult
	{
		const bool success;
		const std::string message;
	};
}
