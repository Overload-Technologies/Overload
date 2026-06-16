/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/Texture.h>

#include <baregl/debug/Assert.h>
#include <baregl/debug/Event.h>
#include <baregl/debug/Log.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>

namespace
{
	constexpr uint32_t CalculateMipMapLevels(uint32_t p_width, uint32_t p_height)
	{
		uint32_t maxDim = p_width > p_height ? p_width : p_height;
		uint32_t levels = 0;

		while (maxDim > 1)
		{
			maxDim >>= 1;
			++levels;
		}
		
		return levels ? levels + 1 : 1u;
	}

	constexpr bool IsValidMipMapFilter(baregl::types::ETextureFilteringMode p_mode)
	{
		return
			p_mode == baregl::types::ETextureFilteringMode::NEAREST_MIPMAP_NEAREST ||
			p_mode == baregl::types::ETextureFilteringMode::NEAREST_MIPMAP_LINEAR ||
			p_mode == baregl::types::ETextureFilteringMode::LINEAR_MIPMAP_NEAREST ||
			p_mode == baregl::types::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR;
	}
}

namespace baregl
{
	Texture::Texture(types::ETextureType p_type, std::string_view p_debugName) :
		m_type{ utils::EnumToValue<GLenum>(p_type) }
	{
		glCreateTextures(m_type, 1, &m_id);
		m_debugName = p_debugName;
		NOTIFY_TEXTURE_CREATED;
	}

	void Texture::Bind(std::optional<uint32_t> p_slot) const
	{
		if (p_slot.has_value())
		{
			glBindTextureUnit(p_slot.value(), m_id);
		}
		else
		{
			glBindTexture(m_type, m_id);
		}
	}

	void Texture::Bind(uint32_t p_slot, types::EImageAccessSpecifier p_access, types::EInternalFormat p_format, uint32_t p_level, std::optional<uint32_t> p_layer) const
	{
		const bool layered = !p_layer.has_value();
		const uint32_t layer = layered ? 0 : p_layer.value();

		glBindImageTexture(p_slot, m_id, p_level, layered, layer, utils::EnumToValue<GLenum>(p_access), utils::EnumToValue<GLenum>(p_format));
	}

	void Texture::Unbind() const
	{
		glBindTexture(m_type, 0);
	}

	types::ETextureType Texture::GetType() const
	{
		return utils::ValueToEnum<types::ETextureType>(m_type);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
		NOTIFY_TEXTURE_DESTROYED;
	}

	void Texture::Allocate(const data::TextureDesc& p_desc)
	{
		auto& desc = m_desc;

		desc = p_desc;
		desc.width = std::max(1u, desc.width);
		desc.height = std::max(1u, desc.height);

		if (desc.mutableDesc.has_value())
		{
			const auto& mutableDesc = desc.mutableDesc.value();

			BAREGL_ASSERT(m_type == GL_TEXTURE_2D, "Mutable textures are only supported for 2D textures");

			// No DSA version for glTexImage2D (mutable texture),
			// so we need to Bind/Unbind the texture.
			Bind();
			glTexImage2D(
				m_type,
				0,
				utils::EnumToValue<GLenum>(desc.internalFormat),
				desc.width,
				desc.height,
				0,
				utils::EnumToValue<GLenum>(mutableDesc.format),
				utils::EnumToValue<GLenum>(mutableDesc.type),
				mutableDesc.data
			);
			Unbind();
		}
		else
		{
			// If the underlying texture is a cube map, this will allocate all 6 sides.
			// No need to iterate over each side.
			glTextureStorage2D(
				m_id,
				desc.useMipMaps ? CalculateMipMapLevels(desc.width, desc.height) : 1,
				utils::EnumToValue<GLenum>(desc.internalFormat),
				desc.width,
				desc.height
			);
		}

		// Once the texture is allocated, we don't need to set the parameters again
		if (!m_allocated)
		{
			glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, utils::EnumToValue<GLenum>(p_desc.horizontalWrap));
			glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, utils::EnumToValue<GLenum>(p_desc.verticalWrap));
			glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, utils::EnumToValue<GLenum>(p_desc.minFilter));
			glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, utils::EnumToValue<GLenum>(p_desc.magFilter));
		}

		m_allocated = true;
	}

	bool Texture::IsValid() const
	{
		return m_allocated;
	}

	bool Texture::IsMutable() const
	{
		BAREGL_ASSERT(IsValid(), "Cannot check if a texture is mutable before it has been allocated");
		return m_desc.mutableDesc.has_value();
	}

	void Texture::Upload(const void* p_data, types::EFormat p_format, types::EPixelDataType p_type)
	{
		BAREGL_ASSERT(IsValid(), "Cannot upload data to a texture before it has been allocated");
		BAREGL_ASSERT(p_data, "Cannot upload texture data from a null pointer");

		if (IsMutable())
		{
			m_desc.mutableDesc.value().data = p_data;
			Allocate(m_desc);
		}
		else
		{
			if (m_type == GL_TEXTURE_CUBE_MAP)
			{
				for (uint32_t i = 0; i < 6; ++i)
				{
					glTextureSubImage3D(
						m_id,
						0,
						0,
						0,
						i,
						m_desc.width,
						m_desc.height,
						1,
						utils::EnumToValue<GLenum>(p_format),
						utils::EnumToValue<GLenum>(p_type),
						p_data
					);
				}
			}
			else
			{
				glTextureSubImage2D(
					m_id,
					0,
					0,
					0,
					m_desc.width,
					m_desc.height,
					utils::EnumToValue<GLenum>(p_format),
					utils::EnumToValue<GLenum>(p_type),
					p_data
				);
			}
		}
	}

	void Texture::Resize(uint32_t p_width, uint32_t p_height)
	{
		BAREGL_ASSERT(IsValid(), "Cannot resize a texture before it has been allocated");
		BAREGL_ASSERT(IsMutable(), "Cannot resize an immutable texture");

		auto& desc = m_desc;

		if (p_width != desc.width || p_height != desc.height)
		{
			desc.width = p_width;
			desc.height = p_height;

			Allocate(desc);
		}
	}

	const baregl::data::TextureDesc& Texture::GetDesc() const
	{
		BAREGL_ASSERT(IsValid(), "Cannot get the descriptor of a texture before it has been allocated");
		return m_desc;
	}

	void Texture::GenerateMipmaps() const
	{
		BAREGL_ASSERT(IsValid(), "Cannot generate mipmaps for a texture before it has been allocated");
		BAREGL_ASSERT(m_desc.useMipMaps, "Cannot generate mipmaps for a texture that doesn't use them");

		if (IsValidMipMapFilter(m_desc.minFilter))
		{
			glGenerateTextureMipmap(m_id);
		}
		else
		{
			// In the event a user tries to generate mipmaps for a texture that doesn't use a valid mipmap filter
			BAREGL_LOG_ERROR("Cannot generate mipmaps for a texture that doesn't use a valid mipmap filter");
		}
	}

	void Texture::SetBorderColor(const math::Vec4& p_color)
	{
		BAREGL_ASSERT(IsValid(), "Cannot set border color for a texture before it has been allocated");
		glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, &p_color.x);
	}

	const std::string& Texture::GetDebugName() const
	{
		return m_debugName;
	}
}
