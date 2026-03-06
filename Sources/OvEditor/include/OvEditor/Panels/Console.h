/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvDebug/Logger.h>

#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextColored.h>

namespace OvEditor::Panels
{
	class Console : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Console
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Method called when a log event occured
		* @param p_logData
		*/
		void OnLogIntercepted(const OvDebug::LogData& p_logData);

		/**
		* Called when the scene plays. It will clear the console if the "Clear on play" settings is on
		*/
		void ClearOnPlay();

		/**
		* Clear the console
		*/
		void Clear();

		/**
		* Filter logs using defined filters
		*/
		void FilterLogs();

		/**
		* Verify if a given log level is allowed by the current filter
		* @param p_logLevel
		*/
		bool IsAllowedByFilter(OvDebug::ELogLevel p_logLevel);

		/**
		* Truncate the logs if the number of logs is greater than the max logs
		*/
		void TruncateLogs();

	private:
		void SetShowDefaultLogs(bool p_value);
		void SetShowInfoLogs(bool p_value);
		void SetShowWarningLogs(bool p_value);
		void SetShowErrorLogs(bool p_value);

	private:
		OvUI::Widgets::Layout::Group* m_logGroup;
		std::unordered_map<OvUI::Widgets::Texts::TextColored*, OvDebug::ELogLevel> m_logTextWidgets;

		bool m_clearOnPlay = true;
		bool m_showDefaultLog = true;
		bool m_showInfoLog = true;
		bool m_showWarningLog = true;
		bool m_showErrorLog = true;
	};
}