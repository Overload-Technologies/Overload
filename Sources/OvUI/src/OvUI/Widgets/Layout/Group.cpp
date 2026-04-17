/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cfloat>

#include <imgui.h>

#include "OvUI/Widgets/Layout/Group.h"

void OvUI::Widgets::Layout::Group::_Draw_Impl()
{
	if (!horizontal)
	{
		DrawWidgets();
		return;
	}

	CollectGarbages();

	if (m_widgets.empty())
	{
		return;
	}

	const auto& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ style.ItemSpacing.x * 0.5f, style.CellPadding.y });

	if (ImGui::BeginTable(("group" + m_widgetID).c_str(), static_cast<int>(m_widgets.size()), ImGuiTableFlags_NoSavedSettings))
	{
		for (size_t index = 0; index < m_widgets.size(); ++index)
		{
			const auto columnFlags = static_cast<int>(index) == stretchWidget ?
				ImGuiTableColumnFlags_WidthStretch :
				ImGuiTableColumnFlags_WidthFixed;

			ImGui::TableSetupColumn(("##col_" + std::to_string(index)).c_str(), columnFlags);
		}

		ImGui::TableNextRow();

		for (size_t index = 0; index < m_widgets.size(); ++index)
		{
			ImGui::TableSetColumnIndex(static_cast<int>(index));

			if (static_cast<int>(index) == stretchWidget)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
			}

			auto& widget = *m_widgets[index].first;
			const auto previousLineBreak = widget.lineBreak;
			widget.lineBreak = true;
			widget.Draw();
			widget.lineBreak = previousLineBreak;
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();
}
