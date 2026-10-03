/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Widgets/AWidget.h>

namespace OvRendering::Entities { class Camera; }
namespace OvEditor::Core { class CameraController; }

namespace OvEditor::Widgets
{
	/**
	* draw a navigation gizmo  in the top-right corner of a view.
	* Clicking a bubble moves the camera to look along that axis.

	*/
	class NavigationGizmo : public OvUI::Widgets::AWidget
	{
	public:
		/**
		* Constructor
		* @param p_camera
		* @param p_cameraController
		*/
		NavigationGizmo(
			OvRendering::Entities::Camera& p_camera,
			OvEditor::Core::CameraController& p_cameraController
		);

		/**
		* returns true if the mouse is over the gizmo
		*/
		bool IsHovered() const;

	protected:
		virtual void _Draw_Impl() override;

	private:
		OvRendering::Entities::Camera& m_camera;
		OvEditor::Core::CameraController& m_cameraController;
		bool m_hovered = false;
		bool m_dragging = false;
	};
}
