/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#define COLOR_BLUE OvDebug::blue
#define COLOR_RED OvDebug::red
#define COLOR_GREEN OvDebug::green
#define COLOR_YELLOW OvDebug::yellow
#define COLOR_WHITE OvDebug::white
#define COLOR_DEFAULT OvDebug::grey

namespace OvDebug
{
	inline std::ostream& blue(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_BLUE | FOREGROUND_GREEN |
			FOREGROUND_INTENSITY);
#else
		s << "\033[1;36m"; // Cyan (bright blue)
#endif
		return s;
	}

	inline std::ostream& red(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
		s << "\033[1;31m"; // Bright red
#endif
		return s;
	}

	inline std::ostream& green(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
		s << "\033[1;32m"; // Bright green
#endif
		return s;
	}

	inline std::ostream& yellow(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_GREEN | FOREGROUND_RED |
			FOREGROUND_INTENSITY);
#else
		s << "\033[1;33m"; // Bright yellow
#endif
		return s;
	}

	inline std::ostream& white(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_RED | FOREGROUND_GREEN |
			FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
		s << "\033[1;37m"; // Bright white
#endif
		return s;
	}

	inline std::ostream& grey(std::ostream& s)
	{
#ifdef _WIN32
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_RED | FOREGROUND_GREEN |
			FOREGROUND_BLUE);
#else
		s << "\033[0m"; // Reset to default
#endif
		return s;
	}

#ifdef _WIN32
	struct color
	{
		color(WORD attribute) : m_color(attribute)
		{
		}

		WORD m_color;
	};

	template <class _Elem, class _Traits>
	std::basic_ostream<_Elem, _Traits>&
		operator<<(std::basic_ostream<_Elem, _Traits>& i, color& c)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, c.m_color);
		return i;
	}
#else
	struct color
	{
		color(const char* ansiCode) : m_color(ansiCode)
		{
		}

		const char* m_color;
	};

	template <class _Elem, class _Traits>
	std::basic_ostream<_Elem, _Traits>&
		operator<<(std::basic_ostream<_Elem, _Traits>& i, color& c)
	{
		i << c.m_color;
		return i;
	}
#endif
}