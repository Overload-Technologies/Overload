/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/utils/BitmaskOperators.h>

#include <tuple>
#include <type_traits>

namespace baregl::utils
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

	// New flag conversion functions
	template <typename EnumType, typename ValueType, typename Mappings = typename MappingFor<EnumType, ValueType>::type, std::size_t I = 0>
	constexpr ValueType ToFlagsImpl(EnumType enumFlags)
	{
		if constexpr (I == tuple_size_v<Mappings>)
		{
			return ValueType{};
		}
		else
		{
			using CurrentPair = typename std::tuple_element<I, Mappings>::type;
			ValueType result{};

			// Check if this flag is set in the input
			if ((enumFlags & CurrentPair::enumValue) == CurrentPair::enumValue)
			{
				result = CurrentPair::value;
			}

			// Recursively check remaining flags
			if constexpr (I + 1 < tuple_size_v<Mappings>)
			{
				result = result | ToFlagsImpl<EnumType, ValueType, Mappings, I + 1>(enumFlags);
			}

			return result;
		}
	}

	template <typename EnumType, typename ValueType, typename Mappings = typename MappingFor<EnumType, ValueType>::type, std::size_t I = 0>
	constexpr EnumType FromFlagsImpl(ValueType valueFlags)
	{
		if constexpr (I == tuple_size_v<Mappings>)
		{
			return EnumType{};
		}
		else
		{
			using CurrentPair = typename std::tuple_element<I, Mappings>::type;
			EnumType result{};

			// Check if this flag is set in the input
			if ((valueFlags & CurrentPair::value) == CurrentPair::value)
			{
				result = CurrentPair::enumValue;
			}

			// Recursively check remaining flags
			if constexpr (I + 1 < tuple_size_v<Mappings>)
			{
				result = result | FromFlagsImpl<EnumType, ValueType, Mappings, I + 1>(valueFlags);
			}

			return result;
		}
	}

	// Public interface functions
	template <typename ValueType, typename EnumType>
	constexpr ValueType EnumToValue(EnumType enumValue)
	{
		if constexpr (baregl::utils::is_bitmask_enum_v<EnumType>)
		{
			return baregl::utils::ToFlagsImpl<EnumType, ValueType>(enumValue);
		}
		else
		{
			return baregl::utils::ToValueImpl<EnumType, ValueType>(enumValue);
		}
	}

	template <typename EnumType, typename ValueType>
	constexpr EnumType ValueToEnum(ValueType value)
	{
		if constexpr (baregl::utils::is_bitmask_enum_v<EnumType>)
		{
			return baregl::utils::FromFlagsImpl<EnumType, ValueType>(value);
		}
		else
		{
			return baregl::utils::FromValueImpl<EnumType, ValueType>(value);
		}
	}
}
