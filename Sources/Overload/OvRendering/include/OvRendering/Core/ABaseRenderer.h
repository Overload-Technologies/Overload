/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <atomic>

#include "OvRendering/Core/IRenderer.h"
#include "OvRendering/Data/FrameInfo.h"
#include "OvRendering/Resources/IMesh.h"
#include "OvRendering/Resources/Texture.h"
#include "OvRendering/Entities/Drawable.h"
#include "OvRendering/Settings/EBlitFlags.h"
#include "OvRendering/Context/Driver.h"

namespace OvRendering::Core
{
	/**
	* A simple base renderer that doesn't handle any object binding, but provide a strong base for other renderers
	* to implement their own logic.
	*/
	class ABaseRenderer : public IRenderer
	{
	public:
		/**
		* Constructor of the base renderer
		* @param p_driver
		*/
		ABaseRenderer(Context::Driver& p_driver);

		/**
		* Destructor of the base renderer
		*/
		virtual ~ABaseRenderer();

		/**
		* Begin Frame
		* @param p_frameDescriptor
		*/
		virtual void BeginFrame(const Data::FrameDescriptor& p_frameDescriptor);

		/**
		* End Frame
		*/
		virtual void EndFrame();

		/**
		* Returns a reference to the current frame descriptor
		* @note Cannot be called outside of a frame drawing operation
		*/
		const Data::FrameDescriptor& GetFrameDescriptor() const;

		/**
		* Create a pipeline state object.
		* The settings are set with default settings provided by the renderer
		*/
		Data::PipelineState CreatePipelineState() const;

		/**
		* Returns true if the renderer is currently drawing a frame
		*/
		bool IsDrawing() const;

		/**
		* Set the viewport
		* @param p_x
		* @param p_y
		* @param p_width
		* @param p_height
		*/
		void SetViewport(uint32_t p_x, uint32_t p_y, uint32_t p_width, uint32_t p_height);

		/**
		* Clear the screen
		* @param p_colorBuffer
		* @param p_depthBuffer
		* @param p_stencilBuffer
		* @param p_color
		*/
		virtual void Clear(
			bool p_colorBuffer,
			bool p_depthBuffer,
			bool p_stencilBuffer,
			const OvMaths::FVector4& p_color = OvMaths::FVector4::Zero
		);

		/**
		* Draw a fullscreen quad with the given material
		* @param p_pso
		* @param p_src
		* @param p_dst
		* @param p_material
		* @param p_flags
		*/
		virtual void Blit(
			OvRendering::Data::PipelineState p_pso,
			OvRendering::HAL::Framebuffer& p_src,
			OvRendering::HAL::Framebuffer& p_dst,
			OvRendering::Data::Material& p_material,
			OvRendering::Settings::EBlitFlags p_flags = OvRendering::Settings::EBlitFlags::DEFAULT
		);

		/**
		* Draw a drawable entity
		* @param p_pso
		* @param p_drawable
		*/
		virtual void DrawEntity(
			OvRendering::Data::PipelineState p_pso,
			const Entities::Drawable& p_drawable
		);

	protected:
		Data::FrameDescriptor m_frameDescriptor;
		Context::Driver& m_driver;
		OvRendering::Resources::Texture* m_emptyTexture;
		std::unique_ptr<OvRendering::Resources::IMesh> m_unitQuad;
		OvRendering::Data::PipelineState m_basePipelineState;
		bool m_isDrawing;

	private:
		static std::atomic_bool s_isDrawing;
	};
}
