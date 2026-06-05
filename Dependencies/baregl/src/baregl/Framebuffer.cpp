/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/Framebuffer.h>

#include <baregl/debug/Assert.h>
#include <baregl/debug/Log.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>
#include <baregl/Renderbuffer.h>

namespace
{
	constexpr uint32_t k_maxColorAttachmentCount = 32;
}

namespace baregl
{
	template<>
	void Framebuffer::Attach(
		std::shared_ptr<Renderbuffer> p_toAttach,
		types::EFramebufferAttachment p_attachment,
		uint32_t p_index,
		std::optional<uint32_t> p_layer
	)
	{
		BAREGL_ASSERT(p_toAttach != nullptr, "Cannot attach a null renderbuffer");
		BAREGL_ASSERT(!p_layer.has_value(), "Renderbuffer cannot use layers");
		BAREGL_ASSERT(p_index == 0 || p_attachment == types::EFramebufferAttachment::COLOR, "Only color attachments support indexing");
		BAREGL_ASSERT(p_index < k_maxColorAttachmentCount, "Color attachment index must be in range [0, 31]");

		const auto attachmentIndex = utils::EnumToValue<GLenum>(p_attachment) + static_cast<GLenum>(p_index);
		glNamedFramebufferRenderbuffer(m_id, attachmentIndex, GL_RENDERBUFFER, p_toAttach->GetID());
		m_attachments[attachmentIndex] = p_toAttach;
	}

	template<>
	void Framebuffer::Attach(
		std::shared_ptr<Texture> p_toAttach,
		types::EFramebufferAttachment p_attachment,
		uint32_t p_index,
		std::optional<uint32_t> p_layer
	)
	{
		BAREGL_ASSERT(p_toAttach != nullptr, "Cannot attach a null texture");
		BAREGL_ASSERT(p_index == 0 || p_attachment == types::EFramebufferAttachment::COLOR, "Only color attachments support indexing");
		BAREGL_ASSERT(p_index < k_maxColorAttachmentCount, "Color attachment index must be in range [0, 31]");

		const auto attachmentIndex = utils::EnumToValue<GLenum>(p_attachment) + static_cast<GLenum>(p_index);
		constexpr uint32_t k_mipMapLevel = 0;

		if (p_layer.has_value())
		{
			glNamedFramebufferTextureLayer(m_id, attachmentIndex, p_toAttach->GetID(), k_mipMapLevel, p_layer.value());
		}
		else
		{
			glNamedFramebufferTexture(m_id, attachmentIndex, p_toAttach->GetID(), k_mipMapLevel);
		}

		m_attachments[attachmentIndex] = p_toAttach;
	}

	Framebuffer::Framebuffer(std::string_view p_debugName) :
		m_debugName{ p_debugName }
	{
		glCreateFramebuffers(1, &m_id);
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_id);
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool Framebuffer::Validate()
	{
		const GLenum status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			BAREGL_LOG_ERROR("Framebuffer validation failed with status: " + std::to_string(status));
			return m_valid = false;
		}

		return m_valid = true;
	}

	bool Framebuffer::IsValid() const
	{
		return m_valid;
	}

	template<>
	std::optional<std::reference_wrapper<Texture>> Framebuffer::GetAttachment(
		baregl::types::EFramebufferAttachment p_attachment,
		uint32_t p_index
	) const
	{
		const auto attachmentIndex = utils::EnumToValue<GLenum>(p_attachment) + static_cast<GLenum>(p_index);

		if (m_attachments.contains(attachmentIndex))
		{
			auto attachment = m_attachments.at(attachmentIndex);

			if (auto pval = std::get_if<std::shared_ptr<Texture>>(&attachment); pval && *pval)
			{
				return **pval;
			}
		}

		return std::nullopt;
	}

	template<>
	std::optional<std::reference_wrapper<Renderbuffer>> Framebuffer::GetAttachment(
		baregl::types::EFramebufferAttachment p_attachment,
		uint32_t p_index
	) const
	{
		const auto attachmentIndex = utils::EnumToValue<GLenum>(p_attachment) + static_cast<GLenum>(p_index);

		if (m_attachments.contains(attachmentIndex))
		{
			auto attachment = m_attachments.at(attachmentIndex);

			if (auto pval = std::get_if<std::shared_ptr<Renderbuffer>>(&attachment); pval && *pval)
			{
				return **pval;
			}
		}

		return std::nullopt;
	}

	void Framebuffer::Resize(uint16_t p_width, uint16_t p_height)
	{
		BAREGL_ASSERT(IsValid(), "Cannot resize an invalid framebuffer");

		for (auto& attachment : m_attachments)
		{
			if (const auto pval = std::get_if<std::shared_ptr<Texture>>(&attachment.second); pval && *pval)
			{
				(*pval)->Resize(p_width, p_height);
			}
			else if (const auto* pval = std::get_if<std::shared_ptr<Renderbuffer>>(&attachment.second); pval && *pval)
			{
				(*pval)->Resize(p_width, p_height);
			}
		}
	}

	void Framebuffer::SetTargetDrawBuffer(std::optional<uint32_t> p_index)
	{
		BAREGL_ASSERT(IsValid(), "Invalid framebuffer");
		BAREGL_ASSERT(!p_index.has_value() || p_index < k_maxColorAttachmentCount, "Color attachment index must be in range [0, 31]");

		if (p_index.has_value())
		{
			glNamedFramebufferDrawBuffer(m_id, GL_COLOR_ATTACHMENT0 + p_index.value());
		}
		else
		{
			glNamedFramebufferDrawBuffer(m_id, GL_NONE);
		}
	}

	void Framebuffer::SetTargetReadBuffer(std::optional<uint32_t> p_index)
	{
		BAREGL_ASSERT(IsValid(), "Invalid framebuffer");
		BAREGL_ASSERT(!p_index.has_value() || p_index < k_maxColorAttachmentCount, "Color attachment index must be in range [0, 31]");

		if (p_index.has_value())
		{
			glNamedFramebufferReadBuffer(m_id, GL_COLOR_ATTACHMENT0 + p_index.value());
		}
		else
		{
			glNamedFramebufferReadBuffer(m_id, GL_NONE);
		}
	}

	std::pair<uint16_t, uint16_t> Framebuffer::GetSize() const
	{
		BAREGL_ASSERT(IsValid(), "Cannot get width of an invalid framebuffer");

		// Attachments are expected to be of the same size, other Validate() would fail.
		// We only need to return the size of the first attachment found, whether it's a texture of render buffer.
		for (auto& attachment : m_attachments)
		{
			if (const auto pval = std::get_if<std::shared_ptr<Texture>>(&attachment.second); pval && *pval)
			{
				return {
					(*pval)->GetDesc().width,
					(*pval)->GetDesc().height
				};
			}
			else if (const auto* pval = std::get_if<std::shared_ptr<Renderbuffer>>(&attachment.second); pval && *pval)
			{
				return {
					(*pval)->GetWidth(),
					(*pval)->GetHeight()
				};
			}
		}

		return { {}, {} }; // <-- not an emoji
	}

	void Framebuffer::BlitToBackBuffer(uint16_t p_backBufferWidth, uint16_t p_backBufferHeight) const
	{
		BAREGL_ASSERT(IsValid(), "Cannot blit an invalid framebuffer");

		auto [width, height] = GetSize();

		glBlitNamedFramebuffer(
			m_id, 0,
			0, 0, width, height,
			0, 0, p_backBufferWidth, p_backBufferHeight,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);
	}

	void Framebuffer::ReadPixels(
		uint32_t p_x,
		uint32_t p_y,
		uint32_t p_width,
		uint32_t p_height,
		types::EPixelDataFormat p_format,
		types::EPixelDataType p_type,
		void* p_data) const
	{
		BAREGL_ASSERT(IsValid(), "Cannot read pixels from an invalid framebuffer");
		BAREGL_ASSERT(p_width > 0 && p_height > 0, "Invalid read size");

		Bind();
		glReadPixels(
			p_x, p_y,
			p_width,
			p_height,
			utils::EnumToValue<GLenum>(p_format),
			utils::EnumToValue<GLenum>(p_type),
			p_data
		);
		Unbind();
	}

	const std::string& Framebuffer::GetDebugName() const
	{
		return m_debugName;
	}
}
