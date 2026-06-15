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

	std::vector<OvUI::Widgets::AWidget*> widgetsToDraw;
	widgetsToDraw.reserve(m_widgets.size());

	for (auto& pair : m_widgets)
	{
		auto* widget = pair.first;
		if (widget && !widget->IsDestroyed())
		{
			widgetsToDraw.push_back(widget);
		}
	}

	if (widgetsToDraw.empty())
	{
		return;
	}

	++m_drawCallDepth;

	const auto& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ style.ItemSpacing.x * 0.5f, style.CellPadding.y });

	if (ImGui::BeginTable(("group" + m_widgetID).c_str(), static_cast<int>(widgetsToDraw.size()), ImGuiTableFlags_NoSavedSettings))
	{
		for (size_t index = 0; index < widgetsToDraw.size(); ++index)
		{
			const auto columnFlags = static_cast<int>(index) == stretchWidget ?
				ImGuiTableColumnFlags_WidthStretch :
				ImGuiTableColumnFlags_WidthFixed;

			ImGui::TableSetupColumn(("##col_" + std::to_string(index)).c_str(), columnFlags);
		}

		ImGui::TableNextRow();

		for (size_t index = 0; index < widgetsToDraw.size(); ++index)
		{
			ImGui::TableSetColumnIndex(static_cast<int>(index));

			if (static_cast<int>(index) == stretchWidget)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
			}

			auto& widget = *widgetsToDraw[index];
			const auto previousLineBreak = widget.lineBreak;
			widget.lineBreak = true;
			widget.Draw();
			widget.lineBreak = previousLineBreak;
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();

	--m_drawCallDepth;

	if (m_drawCallDepth == 0)
	{
		CollectGarbages();
	}
}
