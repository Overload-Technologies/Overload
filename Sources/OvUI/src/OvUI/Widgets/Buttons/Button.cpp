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

	uint32_t styleOverrides = 0;

	if (idleBackgroundColor.has_value())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, Converter::ToImVec4(idleBackgroundColor.value()));
		++styleOverrides;
	}

	if (hoveredBackgroundColor.has_value())
	{
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Converter::ToImVec4(hoveredBackgroundColor.value()));
		++styleOverrides;
	}

	if (clickedBackgroundColor.has_value())
	{
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Converter::ToImVec4(clickedBackgroundColor.value()));
		++styleOverrides;
	}

	if (textColor.has_value())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, Converter::ToImVec4(textColor.value()));
		++styleOverrides;
	}

	// Instead of using disabled directly, as its value can change if some
	// callback is bound to the ClickedEvent.
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

	ImGui::PopStyleColor(styleOverrides);
}

