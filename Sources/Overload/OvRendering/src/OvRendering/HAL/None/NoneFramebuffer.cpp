/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/None/NoneFramebuffer.h>
#include <OvRendering/HAL/None/NoneRenderbuffer.h>

template<>
template<>
void OvRendering::HAL::NoneFramebuffer::Attach(std::shared_ptr<NoneRenderbuffer> p_toAttach, Settings::EFramebufferAttachment p_attachment, uint32_t p_index)
{
	OVASSERT(p_toAttach != nullptr, "Cannot attach a null renderbuffer");
	const auto index = std::underlying_type_t<Settings::EFramebufferAttachment>(p_attachment);
	m_context.attachments[index] = p_toAttach;
}

template<>
template<>
void OvRendering::HAL::NoneFramebuffer::Attach(std::shared_ptr<NoneTexture> p_toAttach, Settings::EFramebufferAttachment p_attachment, uint32_t p_index)
{
	OVASSERT(p_toAttach != nullptr, "Cannot attach a null texture");
	const auto index = std::underlying_type_t<Settings::EFramebufferAttachment>(p_attachment);
	m_context.attachments[index] = p_toAttach;
}

template<>
OvRendering::HAL::NoneFramebuffer::TFramebuffer(std::string_view p_debugName) :
	m_context{ .debugName = std::string{p_debugName} }
{
}

template<>
OvRendering::HAL::NoneFramebuffer::~TFramebuffer()
{
}

template<>
void OvRendering::HAL::NoneFramebuffer::Bind() const
{
}

template<>
void OvRendering::HAL::NoneFramebuffer::Unbind() const
{
}

bool OvRendering::HAL::NoneFramebuffer::Validate()
{
	return m_context.valid = true;
}

template<>
bool OvRendering::HAL::NoneFramebuffer::IsValid() const
{
	return m_context.valid;
}

template<>
template<>
OvTools::Utils::OptRef<OvRendering::HAL::NoneTexture> OvRendering::HAL::NoneFramebuffer::GetAttachment(OvRendering::Settings::EFramebufferAttachment p_attachment, uint32_t p_index) const
{
	const auto index = std::underlying_type_t<Settings::EFramebufferAttachment>(p_attachment);

	if (m_context.attachments.contains(index))
	{
		auto attachment = m_context.attachments.at(index);

		if (auto pval = std::get_if<std::shared_ptr<NoneTexture>>(&attachment); pval && *pval)
		{
			return **pval;
		}
	}

	return std::nullopt;
}

template<>
template<>
OvTools::Utils::OptRef<OvRendering::HAL::NoneRenderbuffer> OvRendering::HAL::NoneFramebuffer::GetAttachment(OvRendering::Settings::EFramebufferAttachment p_attachment, uint32_t p_index) const
{
	const auto index = std::underlying_type_t<Settings::EFramebufferAttachment>(p_attachment);

	if (m_context.attachments.contains(index))
	{
		auto attachment = m_context.attachments.at(index);

		if (auto pval = std::get_if<std::shared_ptr<NoneRenderbuffer>>(&attachment); pval && *pval)
		{
			return **pval;
		}
	}

	return std::nullopt;
}

template<>
void OvRendering::HAL::NoneFramebuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	OVASSERT(IsValid(), "Cannot resize an invalid framebuffer");

	for (auto& attachment : m_context.attachments)
	{
		if (const auto pval = std::get_if<std::shared_ptr<NoneTexture>>(&attachment.second); pval && *pval)
		{
			(*pval)->Resize(p_width, p_height);
		}
		else if (const auto* pval = std::get_if<std::shared_ptr<NoneRenderbuffer>>(&attachment.second); pval && *pval)
		{
			(*pval)->Resize(p_width, p_height);
		}
	}
}

template<>
void OvRendering::HAL::NoneFramebuffer::SetTargetDrawBuffer(std::optional<uint32_t> p_index)
{
	OVASSERT(IsValid(), "Cannot set target draw buffer on an invalid framebuffer");
}

template<>
void OvRendering::HAL::NoneFramebuffer::SetTargetReadBuffer(std::optional<uint32_t> p_index)
{
	OVASSERT(IsValid(), "Cannot set target read buffer on an invalid framebuffer");
}

template<>
uint32_t OvRendering::HAL::NoneFramebuffer::GetID() const
{
	return 0;
}

template<>
std::pair<uint16_t, uint16_t> OvRendering::HAL::NoneFramebuffer::GetSize(Settings::EFramebufferAttachment p_attachment) const
{
	return { {}, {} }; // <-- I swear it's not an emoji
}

template<>
void OvRendering::HAL::NoneFramebuffer::BlitToBackBuffer(uint16_t p_backBufferWidth, uint16_t p_backBufferHeight) const
{
	OVASSERT(IsValid(), "Cannot blit an invalid framebuffer to the back buffer");
}

template<>
void OvRendering::HAL::NoneFramebuffer::ReadPixels(
	uint32_t p_x,
	uint32_t p_y,
	uint32_t p_width,
	uint32_t p_height,
	Settings::EPixelDataFormat p_format,
	Settings::EPixelDataType p_type,
	void* p_data) const
{
	OVASSERT(IsValid(), "Cannot read pixels from an invalid framebuffer");
}
