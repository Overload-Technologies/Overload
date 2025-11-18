/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <GLFW/glfw3.h>

#include "OvWindowing/Inputs/InputManager.h"

OvWindowing::Inputs::InputManager::InputManager(Window& p_window) : m_window(p_window)
{
	m_keyPressedListener = m_window.KeyPressedEvent.AddListener(std::bind(&InputManager::OnKeyPressed, this, std::placeholders::_1));
	m_keyReleasedListener = m_window.KeyReleasedEvent.AddListener(std::bind(&InputManager::OnKeyReleased, this, std::placeholders::_1));
	m_mouseButtonPressedListener = m_window.MouseButtonPressedEvent.AddListener(std::bind(&InputManager::OnMouseButtonPressed, this, std::placeholders::_1));
	m_mouseButtonReleasedListener = m_window.MouseButtonReleasedEvent.AddListener(std::bind(&InputManager::OnMouseButtonReleased, this, std::placeholders::_1));
	m_mouseScrollListener = m_window.MouseScrollEvent.AddListener(std::bind(&InputManager::OnMouseScroll, this, std::placeholders::_1, std::placeholders::_2));
}

OvWindowing::Inputs::InputManager::~InputManager()
{
	m_window.KeyPressedEvent.RemoveListener(m_keyPressedListener);
	m_window.KeyReleasedEvent.RemoveListener(m_keyReleasedListener);
	m_window.MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListener);
	m_window.MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListener);
	m_window.MouseScrollEvent.RemoveListener(m_mouseScrollListener);
}

OvWindowing::Inputs::EKeyState OvWindowing::Inputs::InputManager::GetKeyState(EKey p_key) const
{
	switch (glfwGetKey(m_window.GetGlfwWindow(), static_cast<int>(p_key)))
	{
		case GLFW_PRESS:	return EKeyState::KEY_DOWN;
		case GLFW_RELEASE:	return EKeyState::KEY_UP;
	}

	return EKeyState::KEY_UP;
}

OvWindowing::Inputs::EMouseButtonState OvWindowing::Inputs::InputManager::GetMouseButtonState(EMouseButton p_button) const
{
	switch (glfwGetMouseButton(m_window.GetGlfwWindow(), static_cast<int>(p_button)))
	{
		case GLFW_PRESS:	return EMouseButtonState::MOUSE_DOWN;
		case GLFW_RELEASE:	return EMouseButtonState::MOUSE_UP;
	}

	return EMouseButtonState::MOUSE_UP;
}

bool OvWindowing::Inputs::InputManager::IsKeyPressed(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_DOWN;
}

bool OvWindowing::Inputs::InputManager::IsKeyReleased(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_UP;
}

bool OvWindowing::Inputs::InputManager::IsMouseButtonPressed(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_DOWN;
}

bool OvWindowing::Inputs::InputManager::IsMouseButtonReleased(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_UP;
}

std::pair<double, double> OvWindowing::Inputs::InputManager::GetMousePosition() const
{
	std::pair<double, double> result;
	glfwGetCursorPos(m_window.GetGlfwWindow(), &result.first, &result.second);
	return result;
}

std::pair<double, double> OvWindowing::Inputs::InputManager::GetMouseScroll() const
{
	return m_scrollData;
}

void OvWindowing::Inputs::InputManager::ClearEvents()
{
	m_keyEvents.clear();
	m_mouseButtonEvents.clear();
	m_scrollData = { 0.0, 0.0 };
}

void OvWindowing::Inputs::InputManager::OnKeyPressed(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_DOWN;
}

void OvWindowing::Inputs::InputManager::OnKeyReleased(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_UP;
}

void OvWindowing::Inputs::InputManager::OnMouseButtonPressed(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_DOWN;
}

void OvWindowing::Inputs::InputManager::OnMouseButtonReleased(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_UP;
}

void OvWindowing::Inputs::InputManager::OnMouseScroll(double p_xOffset, double p_yOffset)
{
	m_scrollData = { p_xOffset, p_yOffset };
}
