/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Rendering/SceneRenderer.h>
#include <OvRendering/HAL/UniformBuffer.h>
#include <OvRendering/Entities/Camera.h>
#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/HAL/Framebuffer.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Visual/Image.h>

namespace OvEditor::Panels
{
	/**
	* Base class for any view
	*/
	class AView : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AView(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Update the view
		* @param p_deltaTime
		*/
		virtual void Update(float p_deltaTime);

		/**
		* Custom implementation of the draw method
		*/
		void _Draw_Impl() override;

		/**
		* Prepare the renderer for rendering
		*/
		virtual void InitFrame();

		/**
		* Render the view
		*/
		void Render();

		/**
		* Draw the frame (m_renderer->Draw() if not overriden)
		* @note You don't need to begin/end frame inside of this method, as this is called after begin, and after end
		*/
		virtual void DrawFrame();

		/**
		* Returns the camera used by this view
		*/
		virtual OvRendering::Entities::Camera* GetCamera() = 0;

		/**
		* Returns the scene used by this view
		*/
		virtual OvCore::SceneSystem::Scene* GetScene() = 0;

		/**
		* Returns the size of the panel ignoring its titlebar height
		*/
		std::pair<uint16_t, uint16_t> GetSafeSize() const;

		/**
		* Returns the renderer used by this view
		*/
		const OvCore::Rendering::SceneRenderer& GetRenderer() const;

	protected:
		virtual OvCore::Rendering::SceneRenderer::SceneDescriptor CreateSceneDescriptor();

	protected:
		OvUI::Widgets::Visual::Image* m_image;

		OvMaths::FVector3 m_gridColor = OvMaths::FVector3 { 0.176f, 0.176f, 0.176f };

		OvRendering::HAL::Framebuffer m_framebuffer;
		std::unique_ptr<OvCore::Rendering::SceneRenderer> m_renderer;
	};
}