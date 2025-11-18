/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvAudio::Settings
{
	/**
	* Spatial sound attenuation models
	*/
	enum class EAttenuationModel : uint8_t
	{
		NONE,
		INVERSE_DISTANCE,
		LINEAR_DISTANCE,
		EXPONENTIAL_DISTANCE
	};
}
