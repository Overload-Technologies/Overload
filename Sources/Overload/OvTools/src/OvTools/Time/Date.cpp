/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>
#include <ctime>

#include "OvTools/Time/Date.h"

std::string OvTools::Time::Date::GetDateAsString()
{
	std::string date;
	const auto now = time(nullptr);
	tm ltm;

#ifdef _WIN32
	localtime_s(&ltm, &now);
#else
	localtime_r(&now, &ltm);
#endif

	std::string dateData[6] =
	{
		std::to_string(1900 + ltm.tm_year),
		std::to_string(1 + ltm.tm_mon),
		std::to_string(ltm.tm_mday),
		std::to_string(ltm.tm_hour),
		std::to_string(ltm.tm_min),
		std::to_string(ltm.tm_sec)
	};

	for (uint8_t i = 1; i < 6; ++i)
		if (dateData[i].size() < 2)
			dateData[i].insert(dateData[i].begin(), '0');

	for (uint8_t i = 0; i < 6; ++i)
	{
		date += dateData[i];
		if (i == 2)
			date += '_';
		else if (i != 5)
			date += '-';
	}

	return date;
}
