/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>

#include <OvRendering/Settings/DriverSettings.h>
#include <OvRendering/Settings/ERenderingCapability.h>
#include <OvRendering/Settings/EPrimitiveMode.h>
#include <OvRendering/Settings/ERasterizationMode.h>
#include <OvRendering/Settings/EComparaisonAlgorithm.h>
#include <OvRendering/Settings/EOperation.h>
#include <OvRendering/Settings/ECullFace.h>
#include <OvRendering/Settings/ECullingOptions.h>
#include <OvRendering/Settings/EPixelDataFormat.h>
#include <OvRendering/Settings/EPixelDataType.h>
#include <OvRendering/Settings/EGraphicsBackend.h>
#include <OvRendering/Settings/EFramebufferAttachment.h>
#include <OvRendering/Data/PipelineState.h>
#include <OvRendering/Resources/Texture.h>
#include <OvRendering/HAL/Common/TTexture.h>
#include <OvRendering/HAL/Common/TRenderbuffer.h>
#include <OvTools/Utils/OptRef.h>

namespace OvRendering::HAL
{
	template<Settings::EGraphicsBackend Backend, class TextureContext, class TextureHandleContext, class RenderBufferContext>
	using TFramebufferAttachment = std::variant<
		std::shared_ptr<TTexture<Backend, TextureContext, TextureHandleContext>>,
		std::shared_ptr<TRenderbuffer<Backend, RenderBufferContext>>
	>;

	/**
	* Represents a framebuffer, used to store render data for the graphics backend to use.
	*/
	template<Settings::EGraphicsBackend Backend, class FramebufferContext, class TextureContext, class TextureHandleContext, class RenderBufferContext>
	class TFramebuffer
	{
	public:
		using Attachment = TFramebufferAttachment<Backend, TextureContext, TextureHandleContext, RenderBufferContext>;

		template<typename T>
		static constexpr bool IsSupportedAttachmentType =
			std::same_as<T, TTexture<Backend, TextureContext, TextureHandleContext>> ||
			std::same_as<T, TRenderbuffer<Backend, RenderBufferContext>>;

		/**
		* Creates a framebuffer.
		*/
		TFramebuffer();

		/**
		* Destroys the framebuffer.
		*/
		~TFramebuffer();

		/**
		* Binds the framebuffer.
		*/
		void Bind() const;

		/**
		* Unbinds the framebuffer.
		*/
		void Unbind() const;

		/**
		* Validate the framebuffer. Must be executed at least once after the framebuffer creation.
		* @note It's recommended to call this method after each attachment change.
		* @return Returns true if the framebuffer has been validated successfully.
		*/
		bool Validate();

		/**
		* Returns true if the framebuffer is valid.
		*/
		bool IsValid() const;

		/**
		* Resizes all attachments to match the given width and height.
		* @param p_width
		* @param p_height
		*/
		void Resize(uint16_t p_width, uint16_t p_height);

		/**
		* Attaches the given texture or render buffer to the framebuffer, at the given attachment point
		* @param p_toAttach must be a texture or a render buffer.
		* @param p_attachment
		* @param p_index (optional) useful when specifying multiple color attachments.
		*/
		template<class T>
			requires IsSupportedAttachmentType<T>
		void Attach(std::shared_ptr<T> p_toAttach,
			Settings::EFramebufferAttachment p_attachment,
			uint32_t p_index = 0
		);

		/**
		* Returns the texture or render buffer associated with the given attachment point.
		* @param p_attachment
		* @param p_index (optional) useful when specifying multiple color attachments.
		*/
		template<class T>
			requires IsSupportedAttachmentType<T>
		OvTools::Utils::OptRef<T> GetAttachment(
			OvRendering::Settings::EFramebufferAttachment p_attachment,
			uint32_t p_index = 0
		) const;

		/**
		* Selects which color attachment to draw to.
		* @param p_index index of the color attachment, if set to std::nullopt, no color will be drawn.
		*/
		void SetTargetDrawBuffer(std::optional<uint32_t> p_index);

		/**
		* Selects which color attachment to read from.
		* @param p_index index of the color attachment, if set to std::nullopt, no color attachment will be available for read.
		*/
		void SetTargetReadBuffer(std::optional<uint32_t> p_index);

		/**
		* Returns the ID the buffer.
		*/
		uint32_t GetID() const;

		/**
		* Returns the size of the given attachment.
		* @param p_attachment
		*/
		std::pair<uint16_t, uint16_t> GetSize(
			Settings::EFramebufferAttachment p_attachment = Settings::EFramebufferAttachment::COLOR
		) const;

		/**
		* Blits the framebuffer to the back buffer.
		* @param p_backBufferWidth
		* @param p_backBufferHeight
		*/
		void BlitToBackBuffer(uint16_t p_backBufferWidth, uint16_t p_backBufferHeight) const;

		/**
		* Reads pixels from the framebuffer.
		* @param p_x The x-coordinate of the lower-left corner.
		* @param p_y The y-coordinate of the lower-left corner.
		* @param p_width The width of the pixel rectangle.
		* @param p_height The height of the pixel rectangle.
		* @param p_format The format of the pixel data.
		* @param p_type The data type of the pixel data.
		* @param p_data The destination buffer to store pixel data.
		*/
		void ReadPixels(
			uint32_t p_x,
			uint32_t p_y,
			uint32_t p_width,
			uint32_t p_height,
			Settings::EPixelDataFormat p_format,
			Settings::EPixelDataType p_type,
			void* p_data
		) const;

	protected:
		FramebufferContext m_context;
	};
}
