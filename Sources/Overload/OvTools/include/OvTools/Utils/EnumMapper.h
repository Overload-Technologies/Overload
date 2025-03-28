/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <tuple>

namespace OvTools::Utils
{
	template <auto EnumValue, auto Value>
	struct EnumValuePair
	{
		static constexpr auto enumValue = EnumValue;
		static constexpr auto value = Value;
		using ValueType = decltype(Value);
		using EnumType = decltype(EnumValue);
	};

	template <typename Tuple>
	constexpr std::size_t tuple_size_v = std::tuple_size<Tuple>::value;

	template <typename EnumType, typename ValueType>
	struct MappingFor;

	template <typename EnumType, typename ValueType, typename Mappings = typename MappingFor<EnumType, ValueType>::type, std::size_t I = 0>
	constexpr ValueType ToValueImpl(EnumType enumValue)
	{
		if constexpr (I == tuple_size_v<Mappings>)
		{
			return ValueType{};
		}
		else
		{
			using CurrentPair = typename std::tuple_element<I, Mappings>::type;
			if (enumValue == CurrentPair::enumValue)
			{
				return CurrentPair::value;
			}
			else
			{
				return ToValueImpl<EnumType, ValueType, Mappings, I + 1>(enumValue);
			}
		}
	}

	template <typename EnumType, typename ValueType, typename Mappings = typename MappingFor<EnumType, ValueType>::type, std::size_t I = 0>
	constexpr EnumType FromValueImpl(ValueType value)
	{
		if constexpr (I == tuple_size_v<Mappings>)
		{
			throw "Value not found in mapping";
		}
		else
		{
			using CurrentPair = typename std::tuple_element<I, Mappings>::type;
			if (value == CurrentPair::value)
			{
				return CurrentPair::enumValue;
			}
			else
			{
				return FromValueImpl<EnumType, ValueType, Mappings, I + 1>(value);
			}
		}
	}
}
