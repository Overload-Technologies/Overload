/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TTextureHandle.h>

namespace OvRendering::HAL
{
	struct GLTextureHandleContext
	{
		uint32_t id;
	};

	using GLTextureHandle = TTextureHandle<Settings::EGraphicsBackend::OPENGL, GLTextureHandleContext>;
}
