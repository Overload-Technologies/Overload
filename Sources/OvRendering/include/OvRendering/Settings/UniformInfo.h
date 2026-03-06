/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <any>
#include <string>

#include <OvRendering/Settings/EUniformType.h>

namespace OvRendering::Settings
{
	/**
	* Structure that holds information about a uniform
	*/
	struct UniformInfo
	{
		EUniformType type;
		std::string name;
		std::any defaultValue;
	};
}
