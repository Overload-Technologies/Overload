/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvDebug/Logger.h"

namespace OvDebug
{
	template<typename T>
	inline void Logger::LogToHandlerMap(std::map<std::string, T>& p_map, const LogData & p_data, std::string p_id)
	{
		if (p_id != "default")
			p_map[p_id].Log(p_data);
		else
		{
			if (p_map.find("default") != p_map.end())
				p_map["default"].Log(p_data);
			else
			{
				p_map.emplace("default", T());
				p_map["default"].Log(p_data);
			}
		}
	}
}