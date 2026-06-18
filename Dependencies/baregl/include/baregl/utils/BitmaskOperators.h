/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/
#pragma once

#include <type_traits>

namespace baregl::utils
{
	// Primary template - disabled by default
	template<typename T>
	struct enable_bitmask_operators
	{
		static constexpr bool enable = false;
	};

	// SFINAE helper to check if bitmask operators are enabled for a type
	template<typename T>
	constexpr bool is_bitmask_enum_v = enable_bitmask_operators<T>::enable && std::is_enum_v<T>;
}

// Bitwise OR operator |
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T>
operator|(T lhs, T rhs) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

// Bitwise AND operator &
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T>
operator&(T lhs, T rhs) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

// Bitwise XOR operator ^
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T>
operator^(T lhs, T rhs) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

// Bitwise NOT operator ~
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T>
operator~(T rhs) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<underlying>(rhs));
}

// Compound assignment OR operator |=
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T&>
operator|=(T& lhs, T rhs) noexcept
{
	return lhs = lhs | rhs;
}

// Compound assignment AND operator &=
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T&>
inline operator&=(T& lhs, T rhs) noexcept
{
	return lhs = lhs & rhs;
}

// Compound assignment XOR operator ^=
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, T&>
inline operator^=(T& lhs, T rhs) noexcept
{
	return lhs = lhs ^ rhs;
}

// Utility function to check if a flag is set
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, bool>
inline IsFlagSet(T flags, T mask) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return (static_cast<underlying>(flags) & static_cast<underlying>(mask)) != 0;
}

// Utility function to check if any flags are set
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, bool>
inline HasAnyFlag(T flags) noexcept
{
	using underlying = std::underlying_type_t<T>;
	return static_cast<underlying>(flags) != 0;
}

// Utility function to check if all flags in mask are set
template<typename T>
constexpr std::enable_if_t<baregl::utils::is_bitmask_enum_v<T>, bool>
inline HasAllFlags(T flags, T mask) noexcept
{
	return (flags & mask) == mask;
}

// Macro to enable bitmask operators for an enum type
#define ENABLE_BITMASK_OPERATORS(x) \
template<> \
struct baregl::utils::enable_bitmask_operators<x> \
{ \
	static constexpr bool enable = true; \
};
