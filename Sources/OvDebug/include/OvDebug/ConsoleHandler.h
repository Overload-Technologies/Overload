/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include "OvDebug/ILogHandler.h"

namespace OvDebug
{
	/*
	* Treat the log in the console
	*/
	class ConsoleHandler : public ILogHandler
	{
	public:

		/**
		* Log to the console
		* @param p_logData
		*/
		void Log(const LogData& p_logData);

	private:
		static std::string GetLogHeader(ELogLevel p_logLevel);

		static const std::string __DEFAULT_HEADER;
		static const std::string __INFO_HEADER;
		static const std::string __WARNING_HEADER;
		static const std::string __ERROR_HEADER;
	};
}