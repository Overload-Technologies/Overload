/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>

#include <OvRendering/HAL/None//NoneTexture.h>

OvTools::Eventing::Event<OvRendering::HAL::NoneTexture&> OvRendering::HAL::NoneTexture::CreationEvent;
OvTools::Eventing::Event<OvRendering::HAL::NoneTexture&> OvRendering::HAL::NoneTexture::DestructionEvent;

template<>
OvRendering::HAL::NoneTexture::TTexture(std::string_view p_debugName) : TTextureHandle{ 0 }
{
	m_textureContext.debugName = p_debugName;
	CreationEvent.Invoke(*this);
}

template<>
OvRendering::HAL::NoneTexture::~TTexture()
{
	DestructionEvent.Invoke(*this);
}

template<>
void OvRendering::HAL::NoneTexture::Allocate(const Settings::TextureDesc& p_desc)
{
	m_textureContext.desc = p_desc;
	m_textureContext.allocated = true;
}

template<>
bool OvRendering::HAL::NoneTexture::IsValid() const
{
	return m_textureContext.allocated;
}

template<>
bool OvRendering::HAL::NoneTexture::IsMutable() const
{
	OVASSERT(IsValid(), "Cannot check if a texture is mutable before it has been allocated");
	return m_textureContext.desc.mutableDesc.has_value();
}

template<>
void OvRendering::HAL::NoneTexture::Upload(const void* p_data, Settings::EFormat p_format, Settings::EPixelDataType p_type)
{
	OVASSERT(IsValid(), "Cannot upload data to a texture before it has been allocated");

	if (IsMutable())
	{
		auto& mutableDesc = m_textureContext.desc.mutableDesc.value();
		mutableDesc.format = p_format;
		mutableDesc.type = p_type;
	}
}

template<>
void OvRendering::HAL::NoneTexture::Resize(uint32_t p_width, uint32_t p_height)
{
	OVASSERT(IsValid(), "Cannot resize a texture before it has been allocated");
	OVASSERT(IsMutable(), "Cannot resize an immutable texture");
	
	auto& desc = m_textureContext.desc;

	if (p_width != desc.width || p_height != desc.width)
	{
		desc.width = p_width;
		desc.height = p_height;

		Allocate(desc);
	}
}

template<>
const OvRendering::Settings::TextureDesc& OvRendering::HAL::NoneTexture::GetDesc() const
{
	OVASSERT(IsValid(), "Cannot get the descriptor of a texture before it has been allocated");
	return m_textureContext.desc;
}

template<>
void OvRendering::HAL::NoneTexture::GenerateMipmaps() const
{
	OVASSERT(IsValid(), "Cannot generate mipmaps for a texture before it has been allocated");
	OVASSERT(m_textureContext.desc.useMipMaps, "Cannot generate mipmaps for a texture that doesn't use them");
}

template<>
void OvRendering::HAL::NoneTexture::SetBorderColor(const OvMaths::FVector4& p_color)
{
	OVASSERT(IsValid(), "Cannot set border color for a texture before it has been allocated");
}
