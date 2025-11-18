/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvEditor/Panels/AView.h"
#include "OvEditor/Core/CameraController.h"

namespace OvEditor::Panels
{
	class AViewControllable : public OvEditor::Panels::AView
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AViewControllable(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Update the controllable view (Handle inputs)
		* @param p_deltaTime
		*/
		virtual void Update(float p_deltaTime) override;

		/**
		* Prepare the renderer for rendering
		*/
		virtual void InitFrame() override;

		/**
		* Reset the camera transform to its initial value
		*/
		virtual void ResetCameraTransform();

		/**
		* Returns the camera controller of the controllable view
		*/
		OvEditor::Core::CameraController& GetCameraController();

		/**
		* Returns the camera used by the camera controller
		*/
		virtual OvRendering::Entities::Camera* GetCamera();

		/**
		* Returns the grid color of the view
		*/
		const OvMaths::FVector3& GetGridColor() const;

		/**
		* Defines the grid color of the view
		* @param p_color
		*/
		void SetGridColor(const OvMaths::FVector3& p_color);

		/**
		* Reset the grid color to its initial value
		*/
		void ResetGridColor();

		/**
		* Set the camera clear color
		*/
		void ResetClearColor();

	protected:
		OvMaths::FVector3 m_gridColor;
		OvRendering::Entities::Camera m_camera;
		OvEditor::Core::CameraController m_cameraController;
	};
}