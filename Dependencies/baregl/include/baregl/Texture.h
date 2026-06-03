/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/detail/NativeObject.h>
#include <baregl/math/Vec4.h>
#include <baregl/data/TextureDesc.h>
#include <baregl/types/EImageAccessSpecifier.h>
#include <baregl/types/EInternalFormat.h>
#include <baregl/types/ETextureType.h>

#include <optional>
#include <string>

namespace baregl
{
	/**
	* Represents a texture, used to store image data for the graphics context to use.
	*/
	class Texture final : public detail::NativeObject
	{
	public:
		/**
		* Creates a texture.
		* @param p_type The type of the texture (2D, Cube, etc.)
		* @param p_debugName A name used to identify the texture for debugging purposes
		*/
		Texture(types::ETextureType p_type, std::string_view p_debugName = std::string_view{});

		/**
		* Destroys the texture.
		*/
		~Texture();

		/**
		* Binds the texture to the given slot.
		* @param p_slot Optional slot to bind the texture to.
		*/
		void Bind(std::optional<uint32_t> p_slot = std::nullopt) const;

		/**
		* Binds the texture to the given slot as an image texture.
		* @param p_slot Slot to bind the texture to.
		* @param p_access The level of access the shader has to the image.
		* @param p_format The format that the shader will treat the image as.
		* @param p_level Mipmap level of the texture that will be bound.
		* @param p_layer Optional layer of the texture to bind.
		*/
		void Bind(uint32_t p_slot, types::EImageAccessSpecifier p_access, types::EInternalFormat p_format, uint32_t p_level = 0, std::optional<uint32_t> p_layer = std::nullopt) const;

		/**
		* Unbinds the texture.
		*/
		void Unbind() const;

		/**
		* Returns the texture type.
		*/
		types::ETextureType GetType() const;

		/**
		* Allocates memory for the texture.
		* @param p_desc
		*/
		void Allocate(const data::TextureDesc& p_desc);

		/**
		* Returns true if the texture has been properly allocated.
		*/
		bool IsValid() const;

		/**
		* Returns true if the texture is mutable.
		*/
		bool IsMutable() const;

		/**
		* Uploads data to the texture.
		* @param p_data Pointer to the data to upload.
		* @param p_format Format of the data.
		* @param p_type Type of the pixel data.
		*/
		void Upload(const void* p_data, types::EFormat p_format, types::EPixelDataType p_type);

		/**
		* Resizes the texture.
		* @param p_width
		* @param p_height
		*/
		void Resize(uint32_t p_width, uint32_t p_height);

		/**
		* Returns the texture descriptor structure.
		*/
		const data::TextureDesc& GetDesc() const;

		/**
		* Generates mipmaps for the texture.
		*/
		void GenerateMipmaps() const;

		/**
		* Sets the border color for the texture.
		* @param p_color
		*/
		void SetBorderColor(const math::Vec4& p_color);

		/**
		* Returns the debug name of the texture.
		*/
		const std::string& GetDebugName() const;

	private:
		const uint32_t m_type;
		data::TextureDesc m_desc;
		bool m_allocated = false;
		std::string m_debugName;
	};
}
