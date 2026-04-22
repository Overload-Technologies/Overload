/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <unordered_map>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Panels/Console.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvUI/Styling/Style.h>
#include <OvUI/Types/Color.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/TextLogs.h>
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

	OvUI::Types::ColorEffector GetLogLevelColor(OvDebug::ELogLevel p_logLevel)
	{
		switch (p_logLevel)
		{
		case OvDebug::ELogLevel::LOG_DEFAULT: return OVUI_STYLE(Text);
		case OvDebug::ELogLevel::LOG_INFO:    return OVUI_STYLE(Info);
		case OvDebug::ELogLevel::LOG_WARNING: return OVUI_STYLE(Warning);
		case OvDebug::ELogLevel::LOG_ERROR:   return OVUI_STYLE(Danger);
		}

		return OVUI_STYLE(Text);
	}
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
	clearButton.backgroundColor        = OVUI_STYLE(Danger);
	clearButton.hoveredBackgroundColor  = OVUI_STYLE(DangerHovered);
	clearButton.clickedBackgroundColor  = OVUI_STYLE(DangerActive);
	clearButton.ClickedEvent += [this]
	{
		Clear();
	};

	auto& clearOnPlay = toolbar.CreateWidget<Selection::CheckBox>(m_clearOnPlay, "Clear on play");
	auto& collapseIdentical = toolbar.CreateWidget<Selection::CheckBox>(m_collapseIdenticalLogs, "Collapse");
	auto& enableDefault = toolbar.CreateWidget<Selection::CheckBox>(m_showDefaultLog, "Default");
	auto& enableInfo = toolbar.CreateWidget<Selection::CheckBox>(m_showInfoLog, "Info");
	auto& enableWarning = toolbar.CreateWidget<Selection::CheckBox>(m_showWarningLog, "Warning");
	auto& enableError = toolbar.CreateWidget<Selection::CheckBox>(m_showErrorLog, "Error");

	clearOnPlay.ValueChangedEvent += [this](bool p_value) { m_clearOnPlay = p_value; };
	collapseIdentical.ValueChangedEvent += [this](bool p_value) { SetCollapseIdenticalLogs(p_value); };
	enableDefault.ValueChangedEvent += [this](bool p_value) { SetShowDefaultLogs(p_value); };
	enableInfo.ValueChangedEvent += [this](bool p_value) { SetShowInfoLogs(p_value); };
	enableWarning.ValueChangedEvent += [this](bool p_value) { SetShowWarningLogs(p_value); };
	enableError.ValueChangedEvent += [this](bool p_value) { SetShowErrorLogs(p_value); };

	const uint32_t searchIconID = []{
		if (auto* tex = EDITOR_CONTEXT(editorResources)->GetTexture("Search"))
			return tex->GetTexture().GetID();
		return 0u;
	}();

	m_searchField = &OvCore::Helpers::GUIDrawer::DrawSearchBar(toolbar, searchIconID);
	toolbar.stretchWidget = 7;
	m_searchField->ContentChangedEvent += [this](const std::string&)
	{
		FilterLogs();
	};

	CreateWidget<Visual::Separator>();
	m_logsWidget = &CreateWidget<Layout::TextLogs>();
	m_logsWidget->SetID("console-log-list");

	EDITOR_EVENT(PlayEvent) += [this]
	{
		ClearOnPlay();
	};
	OvDebug::Logger::LogEvent += [this](const OvDebug::LogData& p_logData)
	{
		OnLogIntercepted(p_logData);
	};
}

void OvEditor::Panels::Console::OnLogIntercepted(const OvDebug::LogData& p_logData)
{
	m_logs.push_back({ p_logData });
	m_logsWidget->RequestScrollToBottom();
	TruncateLogs();
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
	RefreshDisplayedLogs();
}

void OvEditor::Panels::Console::FilterLogs()
{
	RebuildVisibleLogs();
	RefreshDisplayedLogs();
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
	const auto maxLogs = static_cast<size_t>(Settings::EditorSettings::ConsoleMaxLogs.Get());
	if (m_logs.size() > maxLogs)
	{
		const auto excessLogs = m_logs.size() - maxLogs;
		m_logs.erase(m_logs.begin(), m_logs.begin() + static_cast<std::ptrdiff_t>(excessLogs));
	}

	RebuildVisibleLogs();
	RefreshDisplayedLogs();
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

	std::vector<size_t> filteredIndices;
	filteredIndices.reserve(m_logs.size());

	for (size_t index = 0; index < m_logs.size(); ++index)
	{
		const auto& logEntry = m_logs[index];
		if (IsAllowedByFilter(logEntry.data.logLevel) && MatchesSearch(logEntry))
			filteredIndices.push_back(index);
	}

	if (m_collapseIdenticalLogs)
	{
		std::unordered_map<std::string, size_t> collapsedByMessage;
		m_visibleLogs.reserve(filteredIndices.size());

		for (const auto index : filteredIndices)
		{
			const auto& logEntry = m_logs[index];
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

		std::ranges::sort(m_visibleLogs, {}, &VisibleLogEntry::latestRawIndex);
	}
	else
	{
		m_visibleLogs.reserve(filteredIndices.size());
		for (const auto index : filteredIndices)
			m_visibleLogs.push_back({ index, 1 });
	}
}

void OvEditor::Panels::Console::RefreshDisplayedLogs()
{
	m_logsWidget->entries.clear();
	m_logsWidget->entries.reserve(m_visibleLogs.size());
	m_logsWidget->emptyText = m_logs.empty() ? "No logs yet." : "No logs match the current filters.";

	for (const auto& visibleEntry : m_visibleLogs)
	{
		const auto& logEntry = m_logs[visibleEntry.latestRawIndex];
		m_logsWidget->entries.push_back({
			FormatLogDate(logEntry.data.date),
			logEntry.data.message,
			GetLogLevelColor(logEntry.data.logLevel),
			visibleEntry.count
		});
	}
}

bool OvEditor::Panels::Console::MatchesSearch(const LogEntry& p_logEntry) const
{
	return !m_searchField || ContainsCaseInsensitive(p_logEntry.data.message, m_searchField->content);
}
