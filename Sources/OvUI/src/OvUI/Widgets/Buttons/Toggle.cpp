/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include "OvUI/Widgets/Buttons/Toggle.h"

OvUI::Widgets::Buttons::Toggle::Toggle(const std::string& p_labelA, const std::string& p_labelB, bool p_state)
	: labelA(p_labelA), labelB(p_labelB), state(p_state)
	, activeColor          { 0.70f, 0.50f, 0.00f, 1.0f }
	, activeHoveredColor   { 0.80f, 0.60f, 0.00f, 1.0f }
	, activePressedColor   { 0.55f, 0.38f, 0.00f, 1.0f }
	, inactiveColor        { 0.18f, 0.18f, 0.18f, 1.0f }
	, inactiveHoveredColor { 0.26f, 0.26f, 0.26f, 1.0f }
	, inactivePressedColor { 0.12f, 0.12f, 0.12f, 1.0f }
{}

void OvUI::Widgets::Buttons::Toggle::_Draw_Impl()
{
	const float rounding = ImGui::GetStyle().FrameRounding;
	const float padX     = ImGui::GetStyle().FramePadding.x;
	const float padY     = ImGui::GetStyle().FramePadding.y;
	const float height   = ImGui::GetFrameHeight();
	const float widthA   = ImGui::CalcTextSize(labelA.c_str()).x + padX * 2.0f;
	const float widthB   = ImGui::CalcTextSize(labelB.c_str()).x + padX * 2.0f;
	constexpr float kSep = 1.0f;

	const ImVec2 origin = ImGui::GetCursorScreenPos();
	const ImVec2 posB   = { origin.x + widthA + kSep, origin.y };

	// Single interaction area covering the entire pill
	const bool clicked = ImGui::InvisibleButton(m_widgetID.c_str(), { widthA + kSep + widthB, height });
	const bool hovered = ImGui::IsItemHovered();
	const bool pressed = ImGui::IsItemActive();

	auto pickColor = [&](bool isActive) -> ImU32 {
		const Types::Color& c = pressed ? (isActive ? activePressedColor   : inactivePressedColor)
		                      : hovered ? (isActive ? activeHoveredColor   : inactiveHoveredColor)
		                      :           (isActive ? activeColor          : inactiveColor);
		return ImGui::ColorConvertFloat4ToU32({ c.r, c.g, c.b, c.a });
	};

	ImDrawList* dl = ImGui::GetWindowDrawList();

	// Left half — rounded left corners only
	dl->AddRectFilled(
		origin,
		{ origin.x + widthA, origin.y + height },
		pickColor(!state),
		rounding,
		ImDrawFlags_RoundCornersLeft);

	// Right half — rounded right corners only
	dl->AddRectFilled(
		posB,
		{ posB.x + widthB, posB.y + height },
		pickColor(state),
		rounding,
		ImDrawFlags_RoundCornersRight);

	const ImU32 textColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
	dl->AddText({ origin.x + padX, origin.y + padY }, textColor, labelA.c_str());
	dl->AddText({ posB.x   + padX, posB.y   + padY }, textColor, labelB.c_str());

	if (clicked)
	{
		state = !state;
		StateChangedEvent.Invoke(state);
	}
}
