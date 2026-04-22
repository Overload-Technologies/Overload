/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <functional>
#include <string>
#include <unordered_map>

#include <imgui.h>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Panels/Console.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvUI/Widgets/AWidget.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Visual/Separator.h>

namespace
{
	bool ContainsCaseInsensitive(const std::string& p_str, const std::string& p_search)
	{
		if (p_search.empty())
			return true;

		return std::search(
			p_str.begin(), p_str.end(),
			p_search.begin(), p_search.end(),
			[](char p_left, char p_right)
			{
				return std::tolower(static_cast<unsigned char>(p_left))
					== std::tolower(static_cast<unsigned char>(p_right));
			}
		) != p_str.end();
	}

	std::string FormatLogDate(const std::string& p_date)
	{
		std::string result = "[";
		bool isTimePart = false;

		for (char character : p_date)
		{
			if (isTimePart)
				result.push_back(character == '-' ? ':' : character);

			if (character == '_')
				isTimePart = true;
		}

		result += ']';
		return result;
	}

	ImVec4 GetLogLevelColor(OvDebug::ELogLevel p_logLevel)
	{
		switch (p_logLevel)
		{
		case OvDebug::ELogLevel::LOG_DEFAULT: return { 0.90f, 0.90f, 0.90f, 1.0f };
		case OvDebug::ELogLevel::LOG_INFO: return { 0.30f, 0.85f, 1.0f, 1.0f };
		case OvDebug::ELogLevel::LOG_WARNING: return { 1.00f, 0.85f, 0.20f, 1.0f };
		case OvDebug::ELogLevel::LOG_ERROR: return { 1.00f, 0.35f, 0.35f, 1.0f };
		}

		return ImGui::GetStyleColorVec4(ImGuiCol_Text);
	}

	ImU32 GetRowBackgroundColor(int p_index)
	{
		const ImVec4 base = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
		const float brightnessOffset = p_index % 2 == 0 ? 0.03f : 0.08f;
		const ImVec4 tinted
		{
			std::min(base.x + brightnessOffset, 1.0f),
			std::min(base.y + brightnessOffset, 1.0f),
			std::min(base.z + brightnessOffset, 1.0f),
			0.35f
		};

		return ImGui::GetColorU32(tinted);
	}

}

namespace OvEditor::Panels
{
	class ConsoleContentWidget : public OvUI::Widgets::AWidget
	{
	public:
		explicit ConsoleContentWidget(Console& p_console) : m_console(p_console)
		{
		}

	private:
		void _Draw_Impl() override
		{
			m_console.DrawContent();
		}

	private:
		Console& m_console;
	};
}

using namespace OvUI::Widgets;

OvEditor::Panels::Console::Console
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	scrollable = false;

	auto& toolbar = CreateWidget<Layout::Group>();
	toolbar.horizontal = true;

	auto& clearButton = toolbar.CreateWidget<Buttons::Button>("Clear");
	clearButton.idleBackgroundColor = { 0.5f, 0.f, 0.f };
	clearButton.ClickedEvent += std::bind(&Console::Clear, this);

	auto& clearOnPlay = toolbar.CreateWidget<Selection::CheckBox>(m_clearOnPlay, "Auto clear on play");
	auto& collapseIdentical = toolbar.CreateWidget<Selection::CheckBox>(m_collapseIdenticalLogs, "Collapse");
	auto& enableDefault = toolbar.CreateWidget<Selection::CheckBox>(m_showDefaultLog, "Default");
	auto& enableInfo = toolbar.CreateWidget<Selection::CheckBox>(m_showInfoLog, "Info");
	auto& enableWarning = toolbar.CreateWidget<Selection::CheckBox>(m_showWarningLog, "Warning");
	auto& enableError = toolbar.CreateWidget<Selection::CheckBox>(m_showErrorLog, "Error");

	clearOnPlay.ValueChangedEvent += [this](bool p_value) { m_clearOnPlay = p_value; };
	collapseIdentical.ValueChangedEvent += std::bind(&Console::SetCollapseIdenticalLogs, this, std::placeholders::_1);
	enableDefault.ValueChangedEvent += std::bind(&Console::SetShowDefaultLogs, this, std::placeholders::_1);
	enableInfo.ValueChangedEvent += std::bind(&Console::SetShowInfoLogs, this, std::placeholders::_1);
	enableWarning.ValueChangedEvent += std::bind(&Console::SetShowWarningLogs, this, std::placeholders::_1);
	enableError.ValueChangedEvent += std::bind(&Console::SetShowErrorLogs, this, std::placeholders::_1);

	const uint32_t searchIconID = []{
		if (auto* tex = EDITOR_CONTEXT(editorResources)->GetTexture("Search"))
			return tex->GetTexture().GetID();
		return 0u;
	}();

	m_searchField = &OvCore::Helpers::GUIDrawer::DrawSearchBar(*this, searchIconID);
	m_searchField->ContentChangedEvent += [this](const std::string&)
	{
		FilterLogs();
	};

	CreateWidget<Visual::Separator>();
	CreateWidget<ConsoleContentWidget>(*this).SetID("console-content");

	EDITOR_EVENT(PlayEvent) += std::bind(&Console::ClearOnPlay, this);
	OvDebug::Logger::LogEvent += std::bind(&Console::OnLogIntercepted, this, std::placeholders::_1);
}

void OvEditor::Panels::Console::OnLogIntercepted(const OvDebug::LogData& p_logData)
{
	m_logs.push_back({ p_logData });

	TruncateLogs();
	RebuildVisibleLogs();
	m_requestScrollToBottom = true;
}

void OvEditor::Panels::Console::ClearOnPlay()
{
	if (m_clearOnPlay)
		Clear();
}

void OvEditor::Panels::Console::Clear()
{
	m_logs.clear();
	m_visibleLogs.clear();
	m_requestScrollToBottom = false;
}

void OvEditor::Panels::Console::FilterLogs()
{
	RebuildVisibleLogs();
}

bool OvEditor::Panels::Console::IsAllowedByFilter(OvDebug::ELogLevel p_logLevel) const
{
	switch (p_logLevel)
	{
	case OvDebug::ELogLevel::LOG_DEFAULT: return m_showDefaultLog;
	case OvDebug::ELogLevel::LOG_INFO: return m_showInfoLog;
	case OvDebug::ELogLevel::LOG_WARNING: return m_showWarningLog;
	case OvDebug::ELogLevel::LOG_ERROR: return m_showErrorLog;
	}

	return false;
}

void OvEditor::Panels::Console::TruncateLogs()
{
	while (m_logs.size() > static_cast<size_t>(Settings::EditorSettings::ConsoleMaxLogs.Get()))
		m_logs.erase(m_logs.begin());
}

void OvEditor::Panels::Console::SetShowDefaultLogs(bool p_value)
{
	m_showDefaultLog = p_value;
	FilterLogs();
}

void OvEditor::Panels::Console::SetShowInfoLogs(bool p_value)
{
	m_showInfoLog = p_value;
	FilterLogs();
}

void OvEditor::Panels::Console::SetShowWarningLogs(bool p_value)
{
	m_showWarningLog = p_value;
	FilterLogs();
}

void OvEditor::Panels::Console::SetShowErrorLogs(bool p_value)
{
	m_showErrorLog = p_value;
	FilterLogs();
}

void OvEditor::Panels::Console::SetCollapseIdenticalLogs(bool p_value)
{
	m_collapseIdenticalLogs = p_value;
	FilterLogs();
}

void OvEditor::Panels::Console::RebuildVisibleLogs()
{
	m_visibleLogs.clear();

	if (m_collapseIdenticalLogs)
	{
		std::unordered_map<std::string, size_t> collapsedByMessage;

		for (size_t index = 0; index < m_logs.size(); ++index)
		{
			const auto& logEntry = m_logs[index];
			if (!IsAllowedByFilter(logEntry.data.logLevel) || !MatchesSearch(logEntry))
				continue;

			const auto [iterator, inserted] = collapsedByMessage.emplace(logEntry.data.message, m_visibleLogs.size());
			if (inserted)
			{
				m_visibleLogs.push_back({ index, 1 });
			}
			else
			{
				auto& visibleEntry = m_visibleLogs[iterator->second];
				visibleEntry.latestRawIndex = index;
				++visibleEntry.count;
			}
		}

		std::sort(m_visibleLogs.begin(), m_visibleLogs.end(), [](const VisibleLogEntry& p_left, const VisibleLogEntry& p_right)
		{
			return p_left.latestRawIndex < p_right.latestRawIndex;
		});
	}
	else
	{
		for (size_t index = 0; index < m_logs.size(); ++index)
		{
			const auto& logEntry = m_logs[index];
			if (!IsAllowedByFilter(logEntry.data.logLevel) || !MatchesSearch(logEntry))
				continue;

			m_visibleLogs.push_back({ index, 1 });
		}
	}
}

void OvEditor::Panels::Console::DrawContent()
{
	DrawLogList();
}

void OvEditor::Panels::Console::DrawLogList()
{
	if (!ImGui::BeginChild("##console-log-list", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::EndChild();
		return;
	}

	if (m_visibleLogs.empty())
	{
		ImGui::TextDisabled("%s", m_logs.empty() ? "No logs yet." : "No logs match the current filters.");
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

	if (ImGui::BeginTable("##console-table", 2, tableFlags))
	{
		ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 40.0f);

		for (size_t index = 0; index < m_visibleLogs.size(); ++index)
		{
			const auto& visibleEntry = m_visibleLogs[index];
			const auto& logEntry = m_logs[visibleEntry.latestRawIndex];
			const std::string timestamp = FormatLogDate(logEntry.data.date);
			const float rowHeight = CalculateLogHeight(logEntry.data.message);

			ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, GetRowBackgroundColor(static_cast<int>(index)));

			ImGui::TableSetColumnIndex(0);

			std::string rowBuffer = logEntry.data.message;
			rowBuffer.push_back('\0');

			ImGui::PushID(static_cast<int>(visibleEntry.latestRawIndex));
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			ImGui::TextUnformatted(timestamp.c_str());
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, GetLogLevelColor(logEntry.data.logLevel));
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
			if (visibleEntry.count > 1)
			{
				const std::string badge = '(' + std::to_string(visibleEntry.count) + ')';
				const float badgeWidth = ImGui::CalcTextSize(badge.c_str()).x;
				const float cursorX = ImGui::GetCursorPosX() + std::max(0.0f, ImGui::GetColumnWidth() - badgeWidth - ImGui::GetStyle().CellPadding.x * 2.0f);
				ImGui::SetCursorPosX(cursorX);
				ImGui::TextDisabled("%s", badge.c_str());
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	if (m_requestScrollToBottom)
	{
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		m_requestScrollToBottom = false;
	}

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(4);

	ImGui::EndChild();
}

bool OvEditor::Panels::Console::MatchesSearch(const LogEntry& p_logEntry) const
{
	return !m_searchField || ContainsCaseInsensitive(p_logEntry.data.message, m_searchField->content);
}

float OvEditor::Panels::Console::CalculateLogHeight(const std::string& p_message) const
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
