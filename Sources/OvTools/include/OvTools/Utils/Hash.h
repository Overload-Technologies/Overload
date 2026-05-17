/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>

namespace OvTools::Utils
{
	template<typename T>
	inline void HashCombine(std::size_t& seed, const T& value)
	{
		// A large odd constant derived from the golden ratio (64-bit variant).
		// It helps distribute bits more uniformly and reduce clustering/collisions.
		constexpr std::size_t kHashCombineConstant = 0x9e3779b97f4a7c15ULL;

		seed ^= std::hash<T>{}(value)
			+ kHashCombineConstant 
			// Bit mixing. They spread entropy from earlier values so order matters
			// and nearby hashes don’t collapse into similar outputs.
			+ (seed << 6)
			+ (seed >> 2);
	}
}

