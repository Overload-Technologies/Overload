/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvRendering::Settings
{
	/**
	* Enumeration of buffer types
	*/
	enum class EBufferType : uint8_t
	{
		VERTEX,
		INDEX,
		UNIFORM,
		SHADER_STORAGE,
		UNKNOWN
	};
}
