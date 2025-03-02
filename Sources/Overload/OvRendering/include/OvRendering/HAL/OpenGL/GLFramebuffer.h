/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <unordered_map>

#include <OvRendering/HAL/Common/TFramebuffer.h>
#include <OvRendering/HAL/OpenGL/GLTexture.h>
#include <OvRendering/HAL/OpenGL/GLRenderbuffer.h>

namespace OvRendering::HAL
{
	template<Settings::EGraphicsBackend Backend, class GLTextureContext, class GLTextureHandleContext, class GLRenderbufferContext>
	struct TGLFramebufferContext
	{
		using Attachment = TFramebufferAttachment<Backend, GLTextureContext, GLTextureHandleContext, GLRenderbufferContext>;

		uint32_t id = 0;
		bool valid = false;
		std::string debugName;
		std::unordered_map<uint32_t, Attachment> attachments;
	};

	using GLFramebufferContext = TGLFramebufferContext<
		Settings::EGraphicsBackend::OPENGL,
		GLTextureContext,
		GLTextureHandleContext,
		GLRenderbufferContext
	>;

	using GLFramebuffer = TFramebuffer<
		Settings::EGraphicsBackend::OPENGL,
		GLFramebufferContext,
		GLTextureContext,
		GLTextureHandleContext,
		GLRenderbufferContext
	>;
}
