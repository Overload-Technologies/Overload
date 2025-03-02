/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TTexture.h>
#include <OvRendering/HAL/OpenGL/GLTextureHandle.h>

namespace OvRendering::HAL
{
	struct GLTextureContext
	{
		Settings::TextureDesc desc;
		bool allocated = false;
	};

	using GLTexture = TTexture<Settings::EGraphicsBackend::OPENGL, GLTextureContext, GLTextureHandleContext>;
}
