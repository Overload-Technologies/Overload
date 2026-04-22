/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <OvDebug/Logger.h>

#include <OvUI/Panels/PanelWindow.h>

namespace OvUI::Widgets::InputFields
{
	class InputText;
}

namespace OvUI::Widgets::Layout
{
	class TextLogs;
}

namespace OvEditor::Panels
{
	class Console : public OvUI::Panels::PanelWindow
	{
	public:
		Console
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		void OnLogIntercepted(const OvDebug::LogData& p_logData);
		void ClearOnPlay();
		void Clear();
		void FilterLogs();
		bool IsAllowedByFilter(OvDebug::ELogLevel p_logLevel) const;
		void TruncateLogs();

	private:
		struct LogEntry
		{
			OvDebug::LogData data;
		};

		struct VisibleLogEntry
		{
			size_t latestRawIndex = 0;
			uint32_t count = 0;
		};

	private:
		void SetShowDefaultLogs(bool p_value);
		void SetShowInfoLogs(bool p_value);
		void SetShowWarningLogs(bool p_value);
		void SetShowErrorLogs(bool p_value);
		void SetCollapseIdenticalLogs(bool p_value);
		void RebuildVisibleLogs();
		void RefreshDisplayedLogs();
		bool MatchesSearch(const LogEntry& p_logEntry) const;

	private:
		std::vector<LogEntry> m_logs;
		std::vector<VisibleLogEntry> m_visibleLogs;

		OvUI::Widgets::InputFields::InputText* m_searchField = nullptr;
		OvUI::Widgets::Layout::TextLogs* m_logsWidget = nullptr;

		bool m_clearOnPlay = true;
		bool m_showDefaultLog = true;
		bool m_showInfoLog = true;
		bool m_showWarningLog = true;
		bool m_showErrorLog = true;
		bool m_collapseIdenticalLogs = false;
	};
}
