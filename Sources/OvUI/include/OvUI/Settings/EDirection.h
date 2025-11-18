/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvUI::Settings
{
	enum class EDirection : uint8_t
	{
		NONE = 0,
		LEFT = 1,
		RIGHT = 2,
		UP = 3,
		DOWN = 4,
	};
}
