/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvGame/Utils/FPSCounter.h"

OvGame::Utils::FPSCounter::FPSCounter(OvWindowing::Window& p_window) : m_window(p_window)
{
	m_text.color = OvUI::Types::Color::Yellow;
	m_defaultHorizontalAlignment = OvUI::Settings::EHorizontalAlignment::RIGHT;
	m_defaultPosition = { static_cast<float>(m_window.GetSize().first) - 10.0f , 10.0f };
	m_text.content = "999 FPS";
	ConsiderWidget(m_text, false);
}

void OvGame::Utils::FPSCounter::Update(float p_deltaTime)
{
	m_elapsed += p_deltaTime;

	++m_frames;

	if (m_elapsed >= 0.1f)
	{
		m_text.content = std::to_string(static_cast<int>(1.f / (m_elapsed / static_cast<float>(m_frames)))) + " FPS";
		SetPosition({ static_cast<float>(m_window.GetSize().first) - 10.0f , 10.0f });
		SetAlignment(OvUI::Settings::EHorizontalAlignment::RIGHT, OvUI::Settings::EVerticalAlignment::TOP);

		m_elapsed = 0.f;
		m_frames = 0;
	}
}
