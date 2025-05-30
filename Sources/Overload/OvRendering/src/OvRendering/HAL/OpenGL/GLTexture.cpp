/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <glad.h>

#include <OvRendering/HAL/OpenGL/GLTexture.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>

OvTools::Eventing::Event<OvRendering::HAL::GLTexture&> OvRendering::HAL::GLTexture::CreationEvent;
OvTools::Eventing::Event<OvRendering::HAL::GLTexture&> OvRendering::HAL::GLTexture::DestructionEvent;

namespace
{
	constexpr uint32_t CalculateMipMapLevels(uint32_t p_width, uint32_t p_height)
	{
		uint32_t maxDim = p_width > p_height ? p_width : p_height;
		return maxDim ? 32u - __lzcnt(maxDim) : 1u;
	}

	constexpr bool IsValidMipMapFilter(OvRendering::Settings::ETextureFilteringMode p_mode)
	{
		return
			p_mode == OvRendering::Settings::ETextureFilteringMode::NEAREST_MIPMAP_NEAREST ||
			p_mode == OvRendering::Settings::ETextureFilteringMode::NEAREST_MIPMAP_LINEAR ||
			p_mode == OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_NEAREST ||
			p_mode == OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR;
	}
}

template<>
OvRendering::HAL::GLTexture::TTexture(Settings::ETextureType p_type, std::string_view p_debugName) : GLTextureHandle(p_type)
{
	glCreateTextures(m_context.type, 1, &m_context.id);
	m_textureContext.debugName = p_debugName;
	CreationEvent.Invoke(*this);
}

template<>
OvRendering::HAL::GLTexture::~TTexture()
{
	glDeleteTextures(1, &m_context.id);
	DestructionEvent.Invoke(*this);
}

template<>
void OvRendering::HAL::GLTexture::Allocate(const Settings::TextureDesc& p_desc)
{
	auto& desc = m_textureContext.desc;

	desc = p_desc;
	desc.width = std::max(1u, desc.width);
	desc.height = std::max(1u, desc.height);

	if (desc.mutableDesc.has_value())
	{
		const auto& mutableDesc = desc.mutableDesc.value();

		OVASSERT(m_context.type == GL_TEXTURE_2D, "Mutable textures are only supported for 2D textures");

		// No DSA version for glTexImage2D (mutable texture),
		// so we need to Bind/Unbind the texture.
		Bind(); 
		glTexImage2D(
			m_context.type,
			0,
			EnumToValue<GLenum>(desc.internalFormat),
			desc.width,
			desc.height,
			0,
			EnumToValue<GLenum>(mutableDesc.format),
			EnumToValue<GLenum>(mutableDesc.type),
			mutableDesc.data
		);
		Unbind();
	}
	else
	{
		// If the underlying texture is a cube map, this will allocate all 6 sides.
		// No need to iterate over each side.
		glTextureStorage2D(
			m_context.id,
			desc.useMipMaps ? CalculateMipMapLevels(desc.width, desc.height) : 1,
			EnumToValue<GLenum>(desc.internalFormat),
			desc.width,
			desc.height
		);
	}

	// Once the texture is allocated, we don't need to set the parameters again
	if (!m_textureContext.allocated)
	{
		glTextureParameteri(m_context.id, GL_TEXTURE_WRAP_S, EnumToValue<GLenum>(p_desc.horizontalWrap));
		glTextureParameteri(m_context.id, GL_TEXTURE_WRAP_T, EnumToValue<GLenum>(p_desc.verticalWrap));
		glTextureParameteri(m_context.id, GL_TEXTURE_MIN_FILTER, EnumToValue<GLenum>(p_desc.minFilter));
		glTextureParameteri(m_context.id, GL_TEXTURE_MAG_FILTER, EnumToValue<GLenum>(p_desc.magFilter));
	}

	m_textureContext.allocated = true;
}

template<>
bool OvRendering::HAL::GLTexture::IsValid() const
{
	return m_textureContext.allocated;
}

template<>
bool OvRendering::HAL::GLTexture::IsMutable() const
{
	OVASSERT(IsValid(), "Cannot check if a texture is mutable before it has been allocated");
	return m_textureContext.desc.mutableDesc.has_value();
}

template<>
void OvRendering::HAL::GLTexture::Upload(const void* p_data, Settings::EFormat p_format, Settings::EPixelDataType p_type)
{
	OVASSERT(IsValid(), "Cannot upload data to a texture before it has been allocated");
	OVASSERT(p_data, "Cannot upload texture data from a null pointer");

	if (IsMutable())
	{
		m_textureContext.desc.mutableDesc.value().data = p_data;
		Allocate(m_textureContext.desc);
	}
	else
	{
		if (m_context.type == GL_TEXTURE_CUBE_MAP)
		{
			for (uint32_t i = 0; i < 6; ++i)
			{
				glTextureSubImage3D(
					m_context.id,
					0,
					0,
					0,
					0,
					m_textureContext.desc.width,
					m_textureContext.desc.height,
					i,
					EnumToValue<GLenum>(p_format),
					EnumToValue<GLenum>(p_type),
					p_data
				);
			}
		}
		else
		{
			glTextureSubImage2D(
				m_context.id,
				0,
				0,
				0,
				m_textureContext.desc.width,
				m_textureContext.desc.height,
				EnumToValue<GLenum>(p_format),
				EnumToValue<GLenum>(p_type),
				p_data
			);
		}
	}
}

template<>
void OvRendering::HAL::GLTexture::Resize(uint32_t p_width, uint32_t p_height)
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
const OvRendering::Settings::TextureDesc& OvRendering::HAL::GLTexture::GetDesc() const
{
	OVASSERT(IsValid(), "Cannot get the descriptor of a texture before it has been allocated");
	return m_textureContext.desc;
}

template<>
void OvRendering::HAL::GLTexture::GenerateMipmaps() const
{
	OVASSERT(IsValid(), "Cannot generate mipmaps for a texture before it has been allocated");
	OVASSERT(m_textureContext.desc.useMipMaps, "Cannot generate mipmaps for a texture that doesn't use them");
	
	if (IsValidMipMapFilter(m_textureContext.desc.minFilter))
	{
		glGenerateTextureMipmap(m_context.id);
	}
	else
	{
		// In the event a user tries to generate mipmaps for a texture that doesn't use a valid mipmap filter
		OVLOG_ERROR("Cannot generate mipmaps for a texture that doesn't use a valid mipmap filter");
	}
}

template<>
void OvRendering::HAL::GLTexture::SetBorderColor(const OvMaths::FVector4& p_color)
{
	OVASSERT(IsValid(), "Cannot set border color for a texture before it has been allocated");
	glTextureParameterfv(m_context.id, GL_TEXTURE_BORDER_COLOR, &p_color.x);
}

template<>
const std::string& OvRendering::HAL::GLTexture::GetDebugName() const
{
	return m_textureContext.debugName;
}
