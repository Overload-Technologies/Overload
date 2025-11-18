/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TRenderbuffer.h>

namespace OvRendering::HAL
{
	struct GLRenderbufferContext
	{
		uint32_t id = 0;
		uint16_t width = 0;
		uint16_t height = 0;
		Settings::EInternalFormat format = Settings::EInternalFormat::RGBA;
		bool allocated = false;
	};

	using GLRenderbuffer = TRenderbuffer<Settings::EGraphicsBackend::OPENGL, GLRenderbufferContext>;
}
