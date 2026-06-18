/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/types/EUniformType.h>

#include <any>
#include <string>
#include <optional>

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
		std::optional<uint32_t> textureIndex;
	};
}
