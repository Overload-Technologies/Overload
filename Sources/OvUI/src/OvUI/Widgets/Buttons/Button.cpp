/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>
#include <imgui_internal.h>

#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Internal/Converter.h>

OvUI::Widgets::Buttons::Button::Button(const std::string& p_label, const OvMaths::FVector2& p_size, bool p_disabled) :
	label(p_label), size(p_size), disabled(p_disabled)
{
}

void OvUI::Widgets::Buttons::Button::_Draw_Impl()
{
	using namespace OvUI::Internal;

	ImGui::PushStyleColor(ImGuiCol_Button,        Converter::ToImVec4(backgroundColor.Resolve()));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Converter::ToImVec4(hoveredBackgroundColor.Resolve()));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Converter::ToImVec4(clickedBackgroundColor.Resolve()));
	ImGui::PushStyleColor(ImGuiCol_Text,          Converter::ToImVec4(textColor.Resolve()));

	const bool isDisabled = disabled;

	if (isDisabled)
	{
		ImGui::BeginDisabled();
	}

	if (ImGui::Button((label + m_widgetID).c_str(), Internal::Converter::ToImVec2(size)))
	{
		ClickedEvent.Invoke();
	}

	if (isDisabled)
	{
		ImGui::EndDisabled();
	}

	ImGui::PopStyleColor(4);
}

