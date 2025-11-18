/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <stdint.h>

namespace OvUI::Internal
{
	/**
	* Simple union necessary for imgui textureID
	*/
	union TextureID
	{
		uint32_t id;
		void* raw;
	};
}