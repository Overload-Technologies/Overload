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

namespace OvEditor::Panels
{
	class ConsoleContentWidget;

	class Console : public OvUI::Panels::PanelWindow
	{
		friend class ConsoleContentWidget;

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
		void DrawContent();
		void DrawLogList();
		bool MatchesSearch(const LogEntry& p_logEntry) const;
		float CalculateLogHeight(const std::string& p_message) const;

	private:
		std::vector<LogEntry> m_logs;
		std::vector<VisibleLogEntry> m_visibleLogs;

		OvUI::Widgets::InputFields::InputText* m_searchField = nullptr;

		bool m_clearOnPlay = true;
		bool m_showDefaultLog = true;
		bool m_showInfoLog = true;
		bool m_showWarningLog = true;
		bool m_showErrorLog = true;
		bool m_collapseIdenticalLogs = true;
		bool m_requestScrollToBottom = false;
	};
}
