/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FTransform.h>
#include <OvMaths/FVector2.h>

#include <OvCore/ECS/Actor.h>

namespace OvEditor::Core
{
	enum class EGizmoOperation
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	/* Handle gizmo behaviours */
	class GizmoBehaviour
	{
	public:
		enum class EDirection
		{
			X,
			Y,
			Z
		};
		
		/**
		* Returns true if the snapping behaviour is enabled
		*/
		bool IsSnappedBehaviourEnabled() const;

		/**
		* Starts the gizmo picking behaviour for the given target in the given direction
		* @param p_actor
		* @param p_cameraPosition
		* @param p_operation
		* @param p_direction
		*/
		void StartPicking(OvCore::ECS::Actor& p_target, const OvMaths::FVector3& p_cameraPosition, EGizmoOperation p_operation, EDirection p_direction);

		/**
		* Stops the gizmo picking behaviour
		*/
		void StopPicking();

		/**
		* Handle the current behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_cameraPosition
		* @param p_viewSize
		*/
		void ApplyOperation(const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector3& p_cameraPosition, const OvMaths::FVector2& p_viewSize);

		/**
		* Set the given mouse position as the current mouse position and update the previous mouse position
		* @param p_mousePosition
		*/
		void SetCurrentMouse(const OvMaths::FVector2& p_mousePosition);

		/**
		* Returns true if the gizmo is currently picked
		*/
		bool IsPicking() const;

		/**
		* Returns the current operation direction
		*/
		EDirection GetDirection() const;

		/**
		* Returns the mouse origin and direction in World Space
                * @param p_mousePos
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		OvMaths::FVector3 GetMouseRay(const OvMaths::FVector2& p_mousePos, const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector2& p_viewSize);

	private:
		/**
		* Returns the global direction matching with the current m_direction
		*/
		OvMaths::FVector3 GetFakeDirection() const;

		/**
		* Returns the actual direction of the target matching with the current m_direction
		* @param p_relative (If true, the direction depends on hierarchy)
		*/
		OvMaths::FVector3 GetRealDirection(bool p_relative = false) const;

		/**
		* Returns the 3D vector of the arrow projected to the screen
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		OvMaths::FVector2 GetScreenDirection(const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector2& p_viewSize) const;

		OvMaths::FVector3 IntersectRayWithAxis(const OvMaths::FVector3& rayOrigin, const OvMaths::FVector3& rayDirection, const OvMaths::FVector3& axisOrigin, const OvMaths::FVector3& axisDirection) const;
		OvMaths::FVector3 RaycastToAxis(const OvMaths::FMatrix4& viewMatrix,
			const OvMaths::FMatrix4& projectionMatrix, const OvMaths::FVector2& viewSize,
			const OvMaths::FVector3& axisDirection, const OvMaths::FVector2& mousePosition) const;
		/**
		* Handle the translation behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_cameraPosition
		* @param p_viewSize
		*/
		void ApplyTranslation(const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector3& p_cameraPosition, const OvMaths::FVector2& p_viewSize);

		/**
		* Handle the rotation behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_viewSize
		*/
		void ApplyRotation(const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector2& p_viewSize) const;

		/**
		* Handle the scale behaviour
		* @param p_viewMatrix
		* @param p_projectionMatrix
		* @param p_cameraPosition
		* @param p_viewSize
		*/
		void ApplyScale(const OvMaths::FMatrix4& p_viewMatrix, const OvMaths::FMatrix4& p_projectionMatrix, const OvMaths::FVector3& p_cameraPosition, const OvMaths::FVector2& p_viewSize);

	private:
		bool m_firstMouse = true;
		bool m_firstPick = true;
		float m_distanceToActor = 0.0f;
		OvCore::ECS::Actor* m_target = nullptr;
		EGizmoOperation m_currentOperation;
		EDirection m_direction;
		OvMaths::FTransform m_originalTransform;
		OvMaths::FVector3 m_initialOffset;
		OvMaths::FVector2 m_originMouse;
		OvMaths::FVector2 m_currentMouse;
		OvMaths::FVector2 m_screenDirection;
	};
}
