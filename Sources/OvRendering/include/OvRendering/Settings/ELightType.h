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
	* Light types
	*/
	enum class ELightType : uint8_t
	{
		POINT,
		DIRECTIONAL,
		SPOT,
		AMBIENT_BOX,
		AMBIENT_SPHERE
	};
}