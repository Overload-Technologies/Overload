/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvRendering::Settings
{
	/**
	* Options that can be used when blitting
	*/
	enum class EBlitFlags : uint8_t
	{
		NONE = 0x0,
		RESIZE_DST_TO_MATCH_SRC = 0x1,
		FILL_INPUT_TEXTURE = 0x2,
		USE_MATERIAL_STATE_MASK = 0x4,
		UPDATE_VIEWPORT_SIZE = 0x8,
		DEFAULT = RESIZE_DST_TO_MATCH_SRC | FILL_INPUT_TEXTURE | UPDATE_VIEWPORT_SIZE,
		ALL = 0xFF
	};

	inline EBlitFlags operator~ (EBlitFlags a) { return (EBlitFlags)~(int)a; }
	inline EBlitFlags operator| (EBlitFlags a, EBlitFlags b) { return (EBlitFlags)((int)a | (int)b); }
	inline EBlitFlags operator& (EBlitFlags a, EBlitFlags b) { return (EBlitFlags)((int)a & (int)b); }
	inline EBlitFlags operator^ (EBlitFlags a, EBlitFlags b) { return (EBlitFlags)((int)a ^ (int)b); }
	inline EBlitFlags& operator|= (EBlitFlags& a, EBlitFlags b) { return (EBlitFlags&)((int&)a |= (int)b); }
	inline EBlitFlags& operator&= (EBlitFlags& a, EBlitFlags b) { return (EBlitFlags&)((int&)a &= (int)b); }
	inline EBlitFlags& operator^= (EBlitFlags& a, EBlitFlags b) { return (EBlitFlags&)((int&)a ^= (int)b); }
	inline bool IsFlagSet(EBlitFlags p_flag, EBlitFlags p_mask) { return (int)p_flag & (int)p_mask; }
}