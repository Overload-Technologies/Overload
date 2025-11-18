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
	inline T IniFile::Get(const std::string& p_key) const
	{
		if (!IsKeyExisting(p_key))
		{
			return T{};
		}

		const auto value = m_data.at(p_key);

		if constexpr (std::same_as<T, bool>)
		{
			return value == "1" || value == "T" || value == "t" || value == "True" || value == "true";
		}
		else if constexpr (std::same_as<T, std::string>)
		{
			return value;
		}
		else if constexpr (std::integral<T>)
		{
			return static_cast<T>(std::atoi(value.c_str()));
		}
		else if constexpr (std::floating_point<T>)
		{
			return static_cast<T>(std::atof(value.c_str()));
		}
	}

	template<SupportedIniType T>
	inline T IniFile::GetOrDefault(const std::string& p_key, T p_default) const
	{
		return IsKeyExisting(p_key) ? Get<T>(p_key) : p_default;
	}

	template<SupportedIniType T>
	inline bool IniFile::TryGet(const std::string& p_key, T& p_outValue) const
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
