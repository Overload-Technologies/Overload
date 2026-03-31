/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/Scripting/Lua/LuaDebugFormatter.h"
#include <sstream>
#include <vector>

namespace
{
	std::string LuaObjectToString(const sol::object& p_object)
	{
		if (!p_object.valid())
			return "nil";

		switch (p_object.get_type())
		{
			case sol::type::none:
			case sol::type::nil:
				return "nil";
			case sol::type::string:
				return p_object.as<std::string>();
			case sol::type::number:
			{
				std::ostringstream stream;
				stream << p_object.as<double>();
				return stream.str();
			}
			case sol::type::boolean:
				return p_object.as<bool>() ? "true" : "false";
			default:
				break;
		}

		sol::state_view lua(p_object.lua_state());
		const sol::protected_function tostring = lua["tostring"];

		if (tostring.valid())
		{
			sol::protected_function_result result = tostring(p_object);

			if (result.valid())
				return result.get<std::string>();
		}

		return "<unprintable>";
	}

	std::string FormatDebugMessage(const std::string& p_format, const std::vector<std::string>& p_arguments)
	{
		std::string result;
		result.reserve(p_format.size() + p_arguments.size() * 8);

		std::size_t argumentIndex = 0;

		for (std::size_t i = 0; i < p_format.size(); ++i)
		{
			if (p_format[i] == '{' && i + 1 < p_format.size())
			{
				if (p_format[i + 1] == '{')
				{
					result += '{';
					++i;
					continue;
				}

				if (p_format[i + 1] == '}')
				{
					if (argumentIndex < p_arguments.size())
						result += p_arguments[argumentIndex++];
					else
						result += "{}";

					++i;
					continue;
				}
			}

			if (p_format[i] == '}' && i + 1 < p_format.size() && p_format[i + 1] == '}')
			{
				result += '}';
				++i;
				continue;
			}

			result += p_format[i];
		}

		for (; argumentIndex < p_arguments.size(); ++argumentIndex)
		{
			if (!result.empty())
				result += ' ';

			result += p_arguments[argumentIndex];
		}

		return result;
	}
}

std::string OvCore::Scripting::Lua::BuildDebugMessage(const std::string& p_format, sol::variadic_args p_args)
{
	std::vector<std::string> arguments;
	arguments.reserve(static_cast<std::size_t>(std::distance(p_args.begin(), p_args.end())));

	for (const sol::object& arg : p_args)
		arguments.push_back(LuaObjectToString(arg));

	return FormatDebugMessage(p_format, arguments);
}
