/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include<format>

#include <OvCore/Rendering/FramebufferUtil.h>
#include <baregl/Framebuffer.h>
#include <baregl/Renderbuffer.h>
#include <baregl/Texture.h>

namespace OvCore::Rendering::FramebufferUtil
{
	void SetupFramebuffer(
		baregl::Framebuffer& p_framebuffer,
		const baregl::data::TextureDesc& p_textureDesc,
		bool p_useDepth,
		bool p_useStencil
	)
	{
		using namespace baregl::types;

		const auto renderTexture = std::make_shared<baregl::Texture>(
			ETextureType::TEXTURE_2D,
			std::format(
				"{}/Color",
				p_framebuffer.GetDebugName()
			)
		);

		renderTexture->Allocate(p_textureDesc);
		p_framebuffer.Attach(renderTexture, EFramebufferAttachment::COLOR);

		if (p_useDepth || p_useStencil)
		{
			const auto renderbuffer = std::make_shared<baregl::Renderbuffer>();
			const auto internalFormat = p_useStencil ? EInternalFormat::DEPTH_STENCIL : EInternalFormat::DEPTH_COMPONENT;
			renderbuffer->Allocate(p_textureDesc.width, p_textureDesc.height, internalFormat);
			if (p_useStencil)
			{
				p_framebuffer.Attach(renderbuffer, EFramebufferAttachment::STENCIL);
			}
			if (p_useDepth)
			{
				p_framebuffer.Attach(renderbuffer, EFramebufferAttachment::DEPTH);
			}
		}

		p_framebuffer.Validate();
	}

	void SetupFramebuffer(
		baregl::Framebuffer& p_framebuffer,
		uint32_t p_width,
		uint32_t p_height,
		bool p_useDepth,
		bool p_useStencil,
		bool p_useMipMaps
	)
	{
		using namespace baregl::types;
		using namespace baregl::data;

		p_width = static_cast<uint16_t>(std::max(1u, p_width));
		p_height = static_cast<uint16_t>(std::max(1u, p_height));

		TextureDesc renderTextureDesc{
			.width = p_width,
			.height = p_height,
			.minFilter = p_useMipMaps ? ETextureFilteringMode::LINEAR_MIPMAP_LINEAR : ETextureFilteringMode::LINEAR,
			.magFilter = ETextureFilteringMode::LINEAR,
			.horizontalWrap = ETextureWrapMode::CLAMP_TO_BORDER,
			.verticalWrap = ETextureWrapMode::CLAMP_TO_BORDER,
			.internalFormat = EInternalFormat::RGBA32F,
			.useMipMaps = p_useMipMaps,
			.mutableDesc = MutableTextureDesc{
				.format = EFormat::RGBA,
				.type = EPixelDataType::FLOAT
			}
		};

		SetupFramebuffer(
			p_framebuffer,
			renderTextureDesc,
			p_useDepth,
			p_useStencil
		);
	}
}
