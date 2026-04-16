/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>
#include <imgui_internal.h>

#include "OvUI/Widgets/Layout/GroupCollapsable.h"

OvUI::Widgets::Layout::GroupCollapsable::GroupCollapsable(const std::string & p_name) :
	name(p_name)
{
}

void OvUI::Widgets::Layout::GroupCollapsable::_Draw_Impl()
{
	bool previouslyOpened = opened;

	bool isOpen = ImGui::CollapsingHeader(name.c_str(), closable ? &opened : nullptr, ImGuiTreeNodeFlags_DefaultOpen);

	EndDisableOverride(); // Early end disable override group so that children are not affected

	if (reorderable)
	{
		const ImVec2 headerMin = ImGui::GetItemRectMin();
		const ImVec2 headerMax = ImGui::GetItemRectMax();
		const float bSize = headerMax.y - headerMin.y;
		const float rightEdge = closable ? (headerMax.x - bSize) : headerMax.x;
		const ImVec2 savedCursor = ImGui::GetCursorPos();

		// Keep original vertical padding so arrow buttons fill the header height exactly
		const float padY = ImGui::GetStyle().FramePadding.y;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, padY});
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0, 0});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));

		ImGui::SetCursorScreenPos({rightEdge - bSize * 2.0f, headerMin.y});
		if (!canMoveUp) ImGui::BeginDisabled();
		if (ImGui::ArrowButton(("##mu" + m_widgetID).c_str(), ImGuiDir_Up))
			MoveUpEvent.Invoke();
		if (!canMoveUp) ImGui::EndDisabled();

		ImGui::SetCursorScreenPos({rightEdge - bSize, headerMin.y});
		if (!canMoveDown) ImGui::BeginDisabled();
		if (ImGui::ArrowButton(("##md" + m_widgetID).c_str(), ImGuiDir_Down))
			MoveDownEvent.Invoke();
		if (!canMoveDown) ImGui::EndDisabled();

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		ImGui::SetCursorPos(savedCursor);
	}

	if (isOpen)
		Group::_Draw_Impl();

	if (opened != previouslyOpened)
	{
		if (opened)
			OpenEvent.Invoke();
		else
			CloseEvent.Invoke();
	}
}
