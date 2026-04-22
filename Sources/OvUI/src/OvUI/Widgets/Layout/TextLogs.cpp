/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cfloat>

#include <imgui.h>

#include <OvUI/Internal/Converter.h>
#include <OvUI/Widgets/Layout/TextLogs.h>

void OvUI::Widgets::Layout::TextLogs::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	const bool childVisible = ImGui::BeginChild(("text_logs" + m_widgetID).c_str(), ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PopStyleVar();

	if (!childVisible)
	{
		ImGui::EndChild();
		return;
	}

	if (entries.empty())
	{
		ImGui::TextDisabled("%s", emptyText.c_str());
		ProcessScrollRequest();
		m_wasScrolledToBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
		ImGui::EndChild();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 2.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));

	constexpr ImGuiTableFlags tableFlags =
		ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_NoPadOuterX |
		ImGuiTableFlags_NoPadInnerX;

	if (ImGui::BeginTable(("text_logs_table" + m_widgetID).c_str(), 2, tableFlags))
	{
		ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, badgeColumnWidth);

		for (size_t index = 0; index < entries.size(); ++index)
		{
			const auto& entry = entries[index];
			const float rowHeight = CalculateRowHeight(entry.content);

			ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, GetRowBackgroundColor(static_cast<int>(index)));

			ImGui::TableSetColumnIndex(0);

			std::string rowBuffer = entry.content;
			rowBuffer.push_back('\0');

			ImGui::PushID(static_cast<int>(index));
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			ImGui::TextUnformatted(entry.header.c_str());
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, Internal::Converter::ToImVec4(entry.contentColor.Resolve()));
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputTextMultiline(
				"##log-line",
				rowBuffer.data(),
				static_cast<int>(rowBuffer.size()),
				ImVec2(-FLT_MIN, rowHeight - ImGui::GetTextLineHeightWithSpacing() + 2.0f),
				ImGuiInputTextFlags_ReadOnly
			);
			ImGui::PopStyleColor();

			ImGui::TableSetColumnIndex(1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
			if (entry.count > 1)
			{
				const std::string badge = "(" + std::to_string(entry.count) + ")";
				const float badgeWidth = ImGui::CalcTextSize(badge.c_str()).x;
				const float cursorX = ImGui::GetCursorPosX() + std::max(0.0f, ImGui::GetColumnWidth() - badgeWidth - ImGui::GetStyle().CellPadding.x * 2.0f);
				ImGui::SetCursorPosX(cursorX);
				ImGui::TextDisabled("%s", badge.c_str());
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	ProcessScrollRequest();
	m_wasScrolledToBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(4);

	ImGui::EndChild();
}

void OvUI::Widgets::Layout::TextLogs::ProcessScrollRequest()
{
	if (m_scrollToBottomFrames == 0)
		return;

	ImGui::SetScrollY(ImGui::GetScrollMaxY());
	--m_scrollToBottomFrames;
}

unsigned int OvUI::Widgets::Layout::TextLogs::GetRowBackgroundColor(int p_index)
{
	const ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

	if (p_index % 2 == 0)
		return ImGui::GetColorU32(bg);

	return ImGui::GetColorU32(ImVec4(bg.x * 0.82f, bg.y * 0.82f, bg.z * 0.82f, bg.w));
}

float OvUI::Widgets::Layout::TextLogs::CalculateRowHeight(const std::string& p_message)
{
	size_t lineCount = 1;
	for (char character : p_message)
	{
		if (character == '\n')
			++lineCount;
	}

	const float lineHeight = ImGui::GetTextLineHeight();
	const float verticalPadding = ImGui::GetStyle().FramePadding.y * 2.0f;
	const float timestampHeight = ImGui::GetTextLineHeightWithSpacing();
	const float messageHeight = lineCount * lineHeight + verticalPadding + 2.0f;
	return std::max(timestampHeight + lineHeight + verticalPadding, timestampHeight + messageHeight);
}

void OvUI::Widgets::Layout::TextLogs::RequestScrollToBottom(uint8_t p_frames)
{
	if (m_wasScrolledToBottom)
		m_scrollToBottomFrames = std::max(m_scrollToBottomFrames, p_frames);
}
