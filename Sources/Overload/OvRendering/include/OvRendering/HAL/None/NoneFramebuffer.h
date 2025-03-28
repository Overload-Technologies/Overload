/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <unordered_map>

#include <OvRendering/HAL/Common/TFramebuffer.h>
#include <OvRendering/HAL/None/NoneTexture.h>
#include <OvRendering/HAL/None/NoneRenderbuffer.h>

namespace OvRendering::HAL
{
	template<Settings::EGraphicsBackend Backend, class GLTextureContext, class GLTextureHandleContext, class GLRenderbufferContext>
	struct TNoneFramebufferContext
	{
		using Attachment = TFramebufferAttachment<Backend, GLTextureContext, GLTextureHandleContext, GLRenderbufferContext>;

		bool valid = false;
		std::unordered_map<std::underlying_type_t<Settings::EFramebufferAttachment>, Attachment> attachments;
	};

	using NoneFramebufferContext = TNoneFramebufferContext<
		Settings::EGraphicsBackend::NONE,
		NoneTextureContext,
		NoneTextureHandleContext,
		NoneRenderbufferContext
	>;

	using NoneFramebuffer = TFramebuffer<
		Settings::EGraphicsBackend::NONE,
		NoneFramebufferContext,
		NoneTextureContext,
		NoneTextureHandleContext,
		NoneRenderbufferContext
	>;
}
