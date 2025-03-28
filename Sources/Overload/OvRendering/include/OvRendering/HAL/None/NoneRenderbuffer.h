/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TRenderbuffer.h>

namespace OvRendering::HAL
{
	struct NoneRenderbufferContext
	{
		uint16_t width = 0;
		uint16_t height = 0;
		Settings::EInternalFormat format = Settings::EInternalFormat::RGBA;
		bool allocated = false;
	};

	using NoneRenderbuffer = TRenderbuffer<Settings::EGraphicsBackend::NONE, NoneRenderbufferContext>;
}
