/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/types/EUniformType.h>

#include <any>
#include <string>

namespace baregl::data
{
	/**
	* Structure that holds information about a uniform
	*/
	struct UniformInfo
	{
		types::EUniformType type;
		std::string name;
		std::any defaultValue;
		uint32_t textureIndex;
	};
}
