/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

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

	auto deriveColor = [](const Types::Color& base, float factor) -> Types::Color {
		return Types::Color{
			std::clamp(base.r * factor, 0.0f, 1.0f),
			std::clamp(base.g * factor, 0.0f, 1.0f),
			std::clamp(base.b * factor, 0.0f, 1.0f),
			base.a
		};
	};

	if (backgroundColor.HasSource())
	{
		const auto base    = backgroundColor.Resolve();
		const auto hovered = hoveredBackgroundColor ? hoveredBackgroundColor->Resolve() : deriveColor(base, 1.3f);
		const auto clicked = clickedBackgroundColor ? clickedBackgroundColor->Resolve() : deriveColor(base, 0.75f);

		ImGui::PushStyleColor(ImGuiCol_Button,        Converter::ToImVec4(base));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Converter::ToImVec4(hovered));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Converter::ToImVec4(clicked));
		styleOverrides += 3;
	}
	else
	{
		if (hoveredBackgroundColor)
		{
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Converter::ToImVec4(hoveredBackgroundColor->Resolve()));
			++styleOverrides;
		}

		if (clickedBackgroundColor)
		{
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Converter::ToImVec4(clickedBackgroundColor->Resolve()));
			++styleOverrides;
		}
	}

	if (textColor.HasSource())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, Converter::ToImVec4(textColor.Resolve()));
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

