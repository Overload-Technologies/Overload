/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <queue>

#include <OvWindowing/Inputs/InputManager.h>
#include <OvWindowing/Window.h>
#include <OvRendering/Entities/Camera.h>

#include "OvEditor/Panels/Hierarchy.h"
#include "OvEditor/Panels/AView.h"

namespace OvEditor::Core
{
	/**
	* A simple camera controller used to navigate into views
	*/
	class CameraController
	{
	public:
		/**
		* Constructor
		* @param p_view
		* @param p_camera
		*/
		CameraController(
			OvEditor::Panels::AView& p_view,
			OvRendering::Entities::Camera& p_camera
		);

		/**
		* Handle mouse and keyboard inputs
		* @parma p_deltaTime
		*/
		void HandleInputs(float p_deltaTime);

		/**
		* Asks the camera to move to the target actor
		* @param p_target
		*/
		void MoveToTarget(OvCore::ECS::Actor& p_target);

		/**
		* Defines the speed of the camera
		* @param p_speed
		*/
		void SetSpeed(float p_speed);

		/**
		* Returns the camera speed
		*/
		float GetSpeed() const;

		/**
		* Defines the position of the camera
		* @param p_position
		*/
		void SetPosition(const OvMaths::FVector3& p_position);

		/**
		* Defines the rotation of the camera
		* @param p_rotation
		*/
		void SetRotation(const OvMaths::FQuaternion& p_rotation);

		/**
		* Returns the position of the camera
		*/
		const OvMaths::FVector3& GetPosition() const;

		/**
		* Returns the position of the camera
		*/
		const OvMaths::FQuaternion& GetRotation() const;

		/**
		* Returns true if the right mouse click is being pressed
		*/
		bool IsRightMousePressed() const;

		/**
		* Lock the target actor to the given actor.
		* @note Usefull to force orbital camera or camera focus to target a specific actor
		* @param p_actor
		*/
		void LockTargetActor(OvCore::ECS::Actor& p_actor);

		/**
		* Removes any locked actor
		*/
		void UnlockTargetActor();

	private:
		std::optional<std::reference_wrapper<OvCore::ECS::Actor>> GetTargetActor() const;
		void HandleCameraPanning(const OvMaths::FVector2& p_mouseOffset, bool p_firstMouse);
		void HandleCameraOrbit(OvCore::ECS::Actor& p_target, const OvMaths::FVector2& p_mouseOffset, bool p_firstMouse);
		void HandleCameraFPSMouse(const OvMaths::FVector2& p_mouseOffset, bool p_firstMouse);

		void HandleCameraZoom();
		void HandleCameraFPSKeyboard(float p_deltaTime);
		void HandleMousePressed();
		void HandleMouseReleased();

	private:
		OvWindowing::Inputs::InputManager& m_inputManager;
		OvWindowing::Window& m_window;
		OvEditor::Panels::AView& m_view;
		OvRendering::Entities::Camera& m_camera;

		std::queue<std::tuple<OvMaths::FVector3, OvMaths::FQuaternion>> m_cameraDestinations;

		bool m_leftMousePressed = false;
		bool m_middleMousePressed = false;
		bool m_rightMousePressed = false;

		OvMaths::FVector3 m_targetSpeed;
		OvMaths::FVector3 m_currentMovementSpeed;

		OvMaths::FTransform* m_orbitTarget = nullptr;
		OvMaths::FVector3 m_orbitStartOffset;
		bool m_firstMouse = true;
		double m_lastMousePosX = 0.0;
		double m_lastMousePosY = 0.0;
		OvMaths::FVector3 m_ypr;
		float m_mouseSensitivity = 0.12f;
		float m_cameraDragSpeed = 0.03f;
		float m_cameraOrbitSpeed = 0.5f;
		float m_cameraMoveSpeed = 15.0f;
		float m_focusDistance = 15.0f;
		float m_focusLerpCoefficient = 8.0f;

		std::optional<std::reference_wrapper<OvCore::ECS::Actor>> m_lockedActor = std::nullopt;
	};
}