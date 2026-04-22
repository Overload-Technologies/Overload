/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#ifdef _DEBUG

#include "OvGame/Debug/DriverInfo.h"

OvGame::Debug::DriverInfo::DriverInfo(OvRendering::Context::Driver& p_driver, OvWindowing::Window& p_window)
{
	m_defaultHorizontalAlignment = OvUI::Settings::EHorizontalAlignment::RIGHT;
	m_defaultVerticalAlignment = OvUI::Settings::EVerticalAlignment::BOTTOM;
	m_defaultPosition.x = static_cast<float>(p_window.GetSize().first) - 10.f;
	m_defaultPosition.y = static_cast<float>(p_window.GetSize().second) - 10.f;

	const std::string vendor(p_driver.GetVendor());
	const std::string hardware(p_driver.GetHardware());
	const std::string version(p_driver.GetVersion());
	const std::string shadingVersion(p_driver.GetShadingLanguageVersion());

	CreateWidget<OvUI::Widgets::Texts::TextColored>("Vendor: "	+ vendor, OvUI::Types::Color::Yellow);
	CreateWidget<OvUI::Widgets::Texts::TextColored>("Hardware: " + hardware, OvUI::Types::Color::Yellow);
	CreateWidget<OvUI::Widgets::Texts::TextColored>("OpenGL Version: " + version, OvUI::Types::Color::Yellow);
	CreateWidget<OvUI::Widgets::Texts::TextColored>("GLSL Version: " + shadingVersion, OvUI::Types::Color::Yellow);
}

#endif
