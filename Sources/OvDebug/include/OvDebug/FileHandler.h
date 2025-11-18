/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <queue>

#include "OvDebug/ILogHandler.h"

namespace OvDebug
{
	/*
	* Treat the log in a file
	*/
	class FileHandler : public ILogHandler
	{
	public:

		/**
		* Log the the file
		*/
		void Log(const LogData& p_logData);
	
		/**
		* Returns the log file path
		*/
		static std::string& GetLogFilePath();

		/**
		* Set log file path
		* @param p_path
		*/
		static void SetLogFilePath(const std::string& p_path);

	private:
		static std::string GetLogHeader(ELogLevel p_logLevel);

		static const std::string __DEFAULT_HEADER;
		static const std::string __INFO_HEADER;
		static const std::string __WARNING_HEADER;
		static const std::string __ERROR_HEADER;
		static std::string __APP_LAUNCH_DATE;
		static const std::string __LOG_EXTENSION;

		static std::string LOG_FILE_PATH;
		static std::ofstream OUTPUT_FILE;
	};
}