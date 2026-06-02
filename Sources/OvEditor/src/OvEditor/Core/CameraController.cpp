/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <imgui.h>

#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/Rendering/UIRenderingUtils.h>

#include <OvEditor/Core/CameraController.h>
#include <OvEditor/Core/EditorActions.h>

OvEditor::Core::CameraController::CameraController(
	OvEditor::Panels::AView& p_view,
	OvRendering::Entities::Camera& p_camera
) :
	m_inputManager(*EDITOR_CONTEXT(inputManager)),
	m_window(*EDITOR_CONTEXT(window)),
	m_view(p_view),
	m_camera(p_camera)
{
	m_camera.SetFov(60.0f);
}

namespace
{
	struct ActorFocusTarget
	{
		OvMaths::FVector3 position = OvMaths::FVector3::Zero;
		float distance = 4.0f;
	};

	OvMaths::FVector3 TransformPoint(const OvMaths::FMatrix4& p_matrix, const OvMaths::FVector2& p_point)
	{
		const auto result = p_matrix * OvMaths::FVector4{ p_point.x, p_point.y, 0.0f, 1.0f };
		return { result.x, result.y, result.z };
	}

	OvCore::ECS::Actor* FindNearestCanvasOwner(OvCore::ECS::Actor& p_actor)
	{
		auto* current = &p_actor;

		while (current)
		{
			if (current->GetComponent<OvCore::ECS::Components::UI::CCanvas>())
			{
				return current;
			}

			current = current->GetParent();
		}

		return nullptr;
	}

	OvMaths::FMatrix4 CalculateUnscaledModelMatrix(OvCore::ECS::Actor& p_actor)
	{
		return
			OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition()) *
			OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
	}

	OvMaths::FMatrix4 GetCanvasMatrix(
		OvCore::ECS::Actor& p_actor,
		bool p_screenSpace
	)
	{
		if (p_screenSpace)
		{
			return OvMaths::FMatrix4::Identity;
		}

		if (auto* canvasOwner = FindNearestCanvasOwner(p_actor))
		{
			return CalculateUnscaledModelMatrix(*canvasOwner);
		}

		return OvMaths::FMatrix4::Identity;
	}

	float GetActor3DFocusDist(OvCore::ECS::Actor& p_actor)
	{
		float distance = 4.0f;

		if (p_actor.IsActive())
		{
			if (auto pb = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalBox>())
			{
				distance = std::max(distance, std::max
				(
					std::max
					(
						pb->GetSize().x * p_actor.transform.GetWorldScale().x,
						pb->GetSize().y * p_actor.transform.GetWorldScale().y
					),
					pb->GetSize().z * p_actor.transform.GetWorldScale().z
				) * 1.5f);
			}

			if (auto ps = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalSphere>())
			{
				distance = std::max(distance, std::max
				(
					std::max
					(
						ps->GetRadius() * p_actor.transform.GetWorldScale().x,
						ps->GetRadius() * p_actor.transform.GetWorldScale().y
					),
					ps->GetRadius() * p_actor.transform.GetWorldScale().z
				) * 1.5f);
			}

			if (auto pc = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalCapsule>())
			{
				distance = std::max(distance, std::max
				(
					std::max
					(
						pc->GetRadius() * p_actor.transform.GetWorldScale().x,
						pc->GetHeight() * p_actor.transform.GetWorldScale().y
					),
					pc->GetRadius() * p_actor.transform.GetWorldScale().z
				) * 1.5f);
			}

			if (auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>())
			{
				const bool hasCustomBoundingSphere = modelRenderer->GetFrustumBehaviour() == OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::CUSTOM_BOUNDS;
				const bool hasModel = modelRenderer->GetModel();
				const auto boundingSphere = hasCustomBoundingSphere ? &modelRenderer->GetCustomBoundingSphere() : hasModel ? &modelRenderer->GetModel()->GetBoundingSphere() : nullptr;
				const auto& actorScale = p_actor.transform.GetWorldScale();
				const auto scaleFactor = std::max(std::max(actorScale.x, actorScale.y), actorScale.z);

				distance = std::max(distance, boundingSphere ? (boundingSphere->radius + OvMaths::FVector3::Length(boundingSphere->position)) * scaleFactor * 2.0f : 10.0f);
			}

			for (auto child : p_actor.GetChildren())
				distance = std::max(distance, GetActor3DFocusDist(*child));
		}

		return distance;
	}

	bool TryGetUIFocusTarget(
		OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_screenSpace,
		ActorFocusTarget& p_outTarget
	)
	{
		auto& transform = p_actor.transform;
		if (!transform.HasActiveUIData())
		{
			return false;
		}

		const auto* canvas = OvCore::Rendering::UIRenderingUtils::FindCanvas(p_actor);
		if (!canvas)
		{
			return false;
		}

		OvMaths::FVector2 elementSize = OvMaths::FVector2::Zero;
		if (const auto* image = p_actor.GetComponent<OvCore::ECS::Components::UI::CImage>())
		{
			elementSize = image->GetSize();
		}
		else if (const auto* text = p_actor.GetComponent<OvCore::ECS::Components::UI::CText>())
		{
			elementSize = text->GetSize();
		}
		else if (const auto* canvasComponent = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			elementSize = OvCore::Rendering::UIRenderingUtils::GetCanvasSize(*canvasComponent, p_renderSize);
		}
		else
		{
			elementSize = transform.GetUISize();
		}

		const auto canvasSize = OvCore::Rendering::UIRenderingUtils::GetCanvasSize(p_actor, p_renderSize);
		const auto layoutOffset = OvCore::Rendering::UIRenderingUtils::GetLayoutOffset(p_actor);
		const auto canvasScale = OvCore::Rendering::UIRenderingUtils::GetCanvasScale(*canvas, p_renderSize);
		const auto worldScale = OvCore::Rendering::UIRenderingUtils::GetUIWorldScale(*canvas, p_screenSpace);
		const auto unitsScale = p_screenSpace ? canvasScale : canvasScale * worldScale;

		const bool isCanvas = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>() != nullptr;
		auto matrix = isCanvas ?
			OvMaths::FMatrix4::Identity :
			transform.GetUIMatrix(canvasSize, layoutOffset, elementSize);

		if (!isCanvas && elementSize.x > 0.0f && elementSize.y > 0.0f)
		{
			const auto resolvedSize = transform.GetUIEffectiveSize(elementSize);
			matrix = matrix * OvMaths::FMatrix4::Scaling({
				resolvedSize.x / elementSize.x,
				resolvedSize.y / elementSize.y,
				1.0f
			});
		}

		matrix =
			GetCanvasMatrix(p_actor, p_screenSpace) *
			OvMaths::FMatrix4::Scaling({ unitsScale, unitsScale, 1.0f }) *
			matrix;

		const auto effectiveSize = isCanvas ? elementSize : transform.GetUIEffectiveSize(elementSize);
		const auto focusedSize = effectiveSize * unitsScale;
		p_outTarget.position = TransformPoint(matrix, OvMaths::FVector2::Zero);
		p_outTarget.distance = p_screenSpace ? 4.0f : std::max(4.0f, std::max(focusedSize.x, focusedSize.y) * 2.0f);
		return true;
	}

	ActorFocusTarget GetActorFocusTarget(
		OvCore::ECS::Actor& p_actor,
		const OvMaths::FVector2& p_renderSize,
		bool p_screenSpace
	)
	{
		ActorFocusTarget target;
		if (TryGetUIFocusTarget(p_actor, p_renderSize, p_screenSpace, target))
		{
			return target;
		}

		return {
			p_actor.transform.GetWorldPosition(),
			GetActor3DFocusDist(p_actor)
		};
	}
}

void OvEditor::Core::CameraController::HandleInputs(float p_deltaTime)
{
	// Even if the mouse is not hovering the view, we need to handle the mouse released event.
	// This is to prevent the camera to be stuck in orbitting or panning mode when a mouse button
	// is released outside the view.
	HandleMouseReleased();

	if (m_view.IsHovered())
	{
		HandleMousePressed();

		if (!ImGui::IsAnyItemActive())
		{
			if (auto target = GetTargetActor())
			{
				auto [winWidth, winHeight] = m_view.GetSafeSize();
				const auto renderSize = OvMaths::FVector2{
					winWidth > 0 ? static_cast<float>(winWidth) : 1.0f,
					winHeight > 0 ? static_cast<float>(winHeight) : 1.0f
				};
				const auto focusTarget = GetActorFocusTarget(
					target.value().get(),
					renderSize,
					EDITOR_EXEC(IsSceneUIRenderingEnabled())
				);

				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_F))
				{
					MoveToTarget(target.value().get());
				}

				auto focusObjectFromAngle = [this, focusTarget]( const OvMaths::FVector3& offset)
				{
					auto camPos = focusTarget.position + offset * focusTarget.distance;
					auto direction = OvMaths::FVector3::Normalize(focusTarget.position - camPos);
					m_camera.SetRotation(OvMaths::FQuaternion::LookAt(direction, std::abs(direction.y) == 1.0f ? OvMaths::FVector3::Right : OvMaths::FVector3::Up));
					m_cameraDestinations.push({ camPos, m_camera.GetRotation() });
				};

				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_UP))			focusObjectFromAngle(OvMaths::FVector3::Up);
				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_DOWN))		focusObjectFromAngle(-OvMaths::FVector3::Up);
				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_RIGHT))		focusObjectFromAngle(OvMaths::FVector3::Right);
				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_LEFT))		focusObjectFromAngle(-OvMaths::FVector3::Right);
				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_PAGE_UP))	focusObjectFromAngle(OvMaths::FVector3::Forward);
				if (m_inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_PAGE_DOWN))	focusObjectFromAngle(-OvMaths::FVector3::Forward);
			}
		}
	}

	if (!m_cameraDestinations.empty())
	{
		m_currentMovementSpeed = 0.0f;

		while (m_cameraDestinations.size() != 1)
			m_cameraDestinations.pop();

		auto& [destPos, destRotation] = m_cameraDestinations.front();

		float t = m_focusLerpCoefficient * p_deltaTime;

		if (OvMaths::FVector3::Distance(m_camera.GetPosition(), destPos) <= 0.03f)
		{
			m_camera.SetPosition(destPos);
			m_camera.SetRotation(destRotation);
			m_cameraDestinations.pop();
		}
		else
		{
			m_camera.SetPosition(OvMaths::FVector3::Lerp(m_camera.GetPosition(), destPos, t));
			m_camera.SetRotation(OvMaths::FQuaternion::Lerp(m_camera.GetRotation(), destRotation, t));
		}
	} 
	else
	{
		if (m_rightMousePressed || m_middleMousePressed || m_leftMousePressed)
		{
			auto [xPos, yPos] = m_inputManager.GetMousePosition();

			bool wasFirstMouse = m_firstMouse;

			if (m_firstMouse)
			{
				m_lastMousePosX = xPos;
				m_lastMousePosY = yPos;
				m_firstMouse = false;
			}

			OvMaths::FVector2 mouseOffset
			{
				static_cast<float>(xPos - m_lastMousePosX),
				static_cast<float>(m_lastMousePosY - yPos)
			};

			m_lastMousePosX = xPos;
			m_lastMousePosY = yPos;

			if (m_rightMousePressed)
			{
				HandleCameraFPSMouse(mouseOffset, wasFirstMouse);
			}
			else
			{
				if (m_middleMousePressed)
				{
					if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_ALT) == OvWindowing::Inputs::EKeyState::KEY_DOWN ||
						m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_CONTROL) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
					{
						if (auto target = GetTargetActor())
						{
							HandleCameraOrbit(target.value().get(), mouseOffset, wasFirstMouse);
						}
					}
					else
					{
						HandleCameraPanning(mouseOffset, wasFirstMouse);
					}
				}
			}
		}

		if (m_view.IsHovered())
		{
			HandleCameraZoom();
		}

		HandleCameraFPSKeyboard(p_deltaTime);
	}
}

void OvEditor::Core::CameraController::MoveToTarget(OvCore::ECS::Actor& p_target)
{
	auto [winWidth, winHeight] = m_view.GetSafeSize();
	const auto renderSize = OvMaths::FVector2{
		winWidth > 0 ? static_cast<float>(winWidth) : 1.0f,
		winHeight > 0 ? static_cast<float>(winHeight) : 1.0f
	};
	const auto focusTarget = GetActorFocusTarget(
		p_target,
		renderSize,
		EDITOR_EXEC(IsSceneUIRenderingEnabled())
	);

	m_cameraDestinations.push({
		focusTarget.position -
		m_camera.GetRotation() *
		OvMaths::FVector3::Forward *
		focusTarget.distance,
		m_camera.GetRotation()
	});
}

void OvEditor::Core::CameraController::SetSpeed(float p_speed)
{
	m_cameraMoveSpeed = p_speed;
}

float OvEditor::Core::CameraController::GetSpeed() const
{
	return m_cameraMoveSpeed;
}

void OvEditor::Core::CameraController::SetPosition(const OvMaths::FVector3 & p_position)
{
	m_camera.SetPosition(p_position);
}

void OvEditor::Core::CameraController::SetRotation(const OvMaths::FQuaternion & p_rotation)
{
	m_camera.SetRotation(p_rotation);
}

const OvMaths::FVector3& OvEditor::Core::CameraController::GetPosition() const
{
	return m_camera.GetPosition();
}

const OvMaths::FQuaternion& OvEditor::Core::CameraController::GetRotation() const
{
	return m_camera.GetRotation();
}

bool OvEditor::Core::CameraController::IsRightMousePressed() const
{
	return m_rightMousePressed;
}

bool OvEditor::Core::CameraController::IsOperating() const
{
	return m_rightMousePressed || m_middleMousePressed;
}

void OvEditor::Core::CameraController::LockTargetActor(OvCore::ECS::Actor& p_actor)
{
	m_lockedActor = p_actor;
}

void OvEditor::Core::CameraController::UnlockTargetActor()
{
	m_lockedActor = std::nullopt;
}

std::optional<std::reference_wrapper<OvCore::ECS::Actor>> OvEditor::Core::CameraController::GetTargetActor() const
{
	if (m_lockedActor.has_value())
	{
		return m_lockedActor;
	}
	else if (EDITOR_EXEC(IsAnyActorSelected()))
	{
		return EDITOR_EXEC(GetSelectedActor());
	}

	return std::nullopt;
}

void OvEditor::Core::CameraController::HandleCameraPanning(const OvMaths::FVector2& p_mouseOffset, bool p_firstMouset)
{
	auto mouseOffset = p_mouseOffset * m_cameraDragSpeed;

	m_camera.SetPosition(m_camera.GetPosition() + m_camera.transform->GetWorldRight() * mouseOffset.x);
	m_camera.SetPosition(m_camera.GetPosition() - m_camera.transform->GetWorldUp() * mouseOffset.y);
}

OvMaths::FVector3 RemoveRoll(const OvMaths::FVector3& p_ypr)
{
	OvMaths::FVector3 result = p_ypr;

	if (result.z >= 179.0f || result.z <= -179.0f)
	{
		result.x += result.z;
		result.y = 180.0f - result.y;
		result.z = 0.0f;
	}

	if (result.x > 180.0f) result.x -= 360.0f;
	if (result.x < -180.0f) result.x += 360.0f;

	return result;
}

void OvEditor::Core::CameraController::HandleCameraOrbit(
	OvCore::ECS::Actor& p_target,
	const OvMaths::FVector2& p_mouseOffset,
	bool p_firstMouse
)
{
	auto mouseOffset = p_mouseOffset * m_cameraOrbitSpeed;

	if (p_firstMouse)
	{
		m_ypr = OvMaths::FQuaternion::EulerAngles(m_camera.GetRotation());
		m_ypr = RemoveRoll(m_ypr);
		m_orbitTarget = &p_target.transform.GetFTransform();
		m_orbitStartOffset = -OvMaths::FVector3::Forward * OvMaths::FVector3::Distance(m_orbitTarget->GetWorldPosition(), m_camera.GetPosition());
	}

	m_ypr.y += -mouseOffset.x;
	m_ypr.x += -mouseOffset.y;
	m_ypr.x = std::max(std::min(m_ypr.x, 90.0f), -90.0f);

	auto& target = p_target.transform.GetFTransform();
	OvMaths::FTransform pivotTransform(target.GetWorldPosition());
	OvMaths::FTransform cameraTransform(m_orbitStartOffset);
	cameraTransform.SetParent(pivotTransform);
	pivotTransform.RotateLocal(OvMaths::FQuaternion(m_ypr));
	m_camera.SetPosition(cameraTransform.GetWorldPosition());
	m_camera.SetRotation(cameraTransform.GetWorldRotation());
}

void OvEditor::Core::CameraController::HandleCameraZoom()
{
	constexpr float kUnitsPerScroll = 1.0f;

	const auto verticalScroll = static_cast<float>(
		m_inputManager.GetMouseScroll().second
	);

	m_camera.SetPosition(
		m_camera.GetPosition() +
		m_camera.transform->GetWorldForward() * kUnitsPerScroll * verticalScroll
	);
}

void OvEditor::Core::CameraController::HandleCameraFPSMouse(const OvMaths::FVector2& p_mouseOffset, bool p_firstMouse)
{
	auto mouseOffset = p_mouseOffset * m_mouseSensitivity;

	if (p_firstMouse)
	{
		m_ypr = OvMaths::FQuaternion::EulerAngles(m_camera.GetRotation());
		m_ypr = RemoveRoll(m_ypr);
	}

	m_ypr.y -= mouseOffset.x;
	m_ypr.x += -mouseOffset.y;
	m_ypr.x = std::max(std::min(m_ypr.x, 90.0f), -90.0f);

	m_camera.SetRotation(OvMaths::FQuaternion(m_ypr));
}

void OvEditor::Core::CameraController::HandleCameraFPSKeyboard(float p_deltaTime)
{
	m_targetSpeed = OvMaths::FVector3(0.f, 0.f, 0.f);

	if (m_rightMousePressed)
	{
		bool run = m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_SHIFT) == OvWindowing::Inputs::EKeyState::KEY_DOWN;
		float velocity = m_cameraMoveSpeed * p_deltaTime * (run ? 2.0f : 1.0f);

		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_W) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += m_camera.transform->GetWorldForward() * velocity;
		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_S) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += m_camera.transform->GetWorldForward() * -velocity;
		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_A) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += m_camera.transform->GetWorldRight() * velocity;
		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_D) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += m_camera.transform->GetWorldRight() * -velocity;
		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_E) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += {0.0f, velocity, 0.0f};
		if (m_inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_Q) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
			m_targetSpeed += {0.0f, -velocity, 0.0f};
	}

	m_currentMovementSpeed = OvMaths::FVector3::Lerp(m_currentMovementSpeed, m_targetSpeed, 10.0f * p_deltaTime);
	m_camera.SetPosition(m_camera.GetPosition() + m_currentMovementSpeed);
}

void OvEditor::Core::CameraController::HandleMousePressed()
{
	if (m_inputManager.IsMouseButtonPressed(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
	{
		m_leftMousePressed = true;
	}

	if (m_inputManager.IsMouseButtonPressed(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_window.SetCursorShape(OvWindowing::Cursor::ECursorShape::HAND);
		m_middleMousePressed = true;
	}

	if (m_inputManager.IsMouseButtonPressed(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_window.SetCursorMode(OvWindowing::Cursor::ECursorMode::DISABLED);
	}
}

void OvEditor::Core::CameraController::HandleMouseReleased()
{
	if (m_leftMousePressed && m_inputManager.IsMouseButtonReleased(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_LEFT))
	{
		m_leftMousePressed = false;
		m_firstMouse = true;
	}

	if (m_middleMousePressed && m_inputManager.IsMouseButtonReleased(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_middleMousePressed = false;
		m_firstMouse = true;
		m_window.SetCursorShape(OvWindowing::Cursor::ECursorShape::ARROW);
	}

	if (m_rightMousePressed && m_inputManager.IsMouseButtonReleased(OvWindowing::Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = false;
		m_firstMouse = true;
		m_window.SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
	}
}
