/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <assert.h>

#include "OvTools/Filesystem/IniFile.h"

namespace OvTools::Filesystem
{
	template<SupportedIniType T>
	inline T IniFile::Get(const std::string& p_key)
	{
		if (!IsKeyExisting(p_key))
		{
			return T{};
		}

		if constexpr (std::same_as<T, bool>)
		{
			return m_data[p_key] == "1" || m_data[p_key] == "T" || m_data[p_key] == "t" || m_data[p_key] == "True" || m_data[p_key] == "true";
		}
		else if constexpr (std::same_as<T, std::string>)
		{
			return m_data[p_key];
		}
		else if constexpr (std::integral<T>)
		{
			return static_cast<T>(std::atoi(m_data[p_key].c_str()));
		}
		else if constexpr (std::floating_point<T>)
		{
			return static_cast<T>(std::atof(m_data[p_key].c_str()));
		}
	}

	template<SupportedIniType T>
	inline T IniFile::GetOrDefault(const std::string& p_key, T p_default)
	{
		return IsKeyExisting(p_key) ? Get<T>(p_key) : p_default;
	}

	template<SupportedIniType T>
	inline bool IniFile::TryGet(const std::string& p_key, T& p_outValue)
	{
		if (IsKeyExisting(p_key))
		{
			p_outValue = Get<T>(p_key);
			return true;
		}

		return false;
	}

	template<SupportedIniType T>
	inline bool IniFile::Set(const std::string& p_key, const T& p_value)
	{
		if (IsKeyExisting(p_key))
		{
			if constexpr (std::same_as<T, bool>)
			{
				m_data[p_key] = p_value ? "true" : "false";
			}
			else if constexpr (std::same_as<T, std::string>)
			{
				m_data[p_key] = p_value;
			}
			else
			{
				m_data[p_key] = std::to_string(p_value);
			}

			return true;
		}

		return false;
	}

	template<SupportedIniType T>
	inline bool IniFile::Add(const std::string& p_key, const T& p_value)
	{
		if (!IsKeyExisting(p_key))
		{
			if constexpr (std::is_same<bool, T>::value)
			{
				RegisterPair(p_key, p_value ? "true" : "false");
			}
			else if constexpr (std::is_same<std::string, T>::value)
			{
				RegisterPair(p_key, p_value);
			}
			else
			{
				RegisterPair(p_key, std::to_string(p_value));
			}

			return true;
		}

		return false;
	}
}
