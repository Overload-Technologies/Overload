/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvMaths/FVector4.h>
#include <OvRendering/HAL/Common/TTextureHandle.h>
#include <OvRendering/Settings/EGraphicsBackend.h>
#include <OvRendering/Settings/TextureDesc.h>
#include <OvTools/Eventing/Event.h>

namespace OvRendering::HAL
{
	/**
	* Represents a texture, used to store image data for the graphics backend to use.
	*/
	template<Settings::EGraphicsBackend Backend, class TextureContext, class TextureHandleContext>
	class TTexture final : public TTextureHandle<Backend, TextureHandleContext>
	{
	public:
		/**
		* Creates a texture.
		* @param p_debugName A name used to identify the texture for debugging purposes
		*/
		TTexture(std::string_view p_debugName = std::string_view{});

		/**
		* Destroys the texture.
		*/
		~TTexture();

		/**
		* Allocates memory for the texture.
		* @param p_desc
		*/
		void Allocate(const Settings::TextureDesc& p_desc);

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
		void Upload(const void* p_data, Settings::EFormat p_format, Settings::EPixelDataType p_type);

		/**
		* Resizes the texture.
		* @param p_width
		* @param p_height
		*/
		void Resize(uint32_t p_width, uint32_t p_height);

		/**
		* Returns the texture descriptor structure.
		*/
		const Settings::TextureDesc& GetDesc() const;

		/**
		* Generates mipmaps for the texture.
		*/
		void GenerateMipmaps() const;

		/**
		* Sets the border color for the texture.
		* @param p_color
		*/
		void SetBorderColor(const OvMaths::FVector4& p_color);

		/**
		* Returns the debug name of the texture.
		*/
		const std::string& GetDebugName() const;

	public:
		static OvTools::Eventing::Event<TTexture&> CreationEvent;
		static OvTools::Eventing::Event<TTexture&> DestructionEvent;

	private:
		TextureContext m_textureContext;
	};
}
