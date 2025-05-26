/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvCore::Rendering
{
	enum class EVisibilityFlags : uint32_t
	{
		NONE = 0,
		GEOMETRY = 1 << 0,
		SHADOW = 1 << 1,
		REFLECTION = 1 << 2,
		ALL = GEOMETRY | SHADOW | REFLECTION
	};

	inline EVisibilityFlags operator~ (EVisibilityFlags a) { return (EVisibilityFlags)~(int)a; }
	inline EVisibilityFlags operator| (EVisibilityFlags a, EVisibilityFlags b) { return (EVisibilityFlags)((int)a | (int)b); }
	inline EVisibilityFlags operator& (EVisibilityFlags a, EVisibilityFlags b) { return (EVisibilityFlags)((int)a & (int)b); }
	inline EVisibilityFlags operator^ (EVisibilityFlags a, EVisibilityFlags b) { return (EVisibilityFlags)((int)a ^ (int)b); }
	inline EVisibilityFlags& operator|= (EVisibilityFlags& a, EVisibilityFlags b) { return (EVisibilityFlags&)((int&)a |= (int)b); }
	inline EVisibilityFlags& operator&= (EVisibilityFlags& a, EVisibilityFlags b) { return (EVisibilityFlags&)((int&)a &= (int)b); }
	inline EVisibilityFlags& operator^= (EVisibilityFlags& a, EVisibilityFlags b) { return (EVisibilityFlags&)((int&)a ^= (int)b); }
	inline bool IsFlagSet(EVisibilityFlags p_flag, EVisibilityFlags p_mask) { return (int)p_flag & (int)p_mask; }
	inline bool SatisfiesVisibility(EVisibilityFlags p_flag, EVisibilityFlags p_required) { return ((int)p_flag & (int)p_required) == (int)p_required; }
}
