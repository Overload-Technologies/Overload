/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/HardwareInfo.h>

using namespace OvUI::Panels;
using namespace OvUI::Widgets;

OvEditor::Panels::HardwareInfo::HardwareInfo
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	auto& driver = *EDITOR_CONTEXT(driver);

	// Display some static information about the graphics driver
	CreateWidget<OvUI::Widgets::Texts::Text>("Vendor: " + std::string(driver.GetVendor()));
	CreateWidget<OvUI::Widgets::Texts::Text>("Hardware: " + std::string(driver.GetHardware()));
	CreateWidget<OvUI::Widgets::Texts::Text>("OpenGL Version: " + std::string(driver.GetVersion()));
	CreateWidget<OvUI::Widgets::Texts::Text>("GLSL Version: " + std::string(driver.GetShadingLanguageVersion()));
}
