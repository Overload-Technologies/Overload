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
	* Enumeration of pixel data types
	*/
	enum class EPixelDataType : uint8_t
	{
		BYTE,
		UNSIGNED_BYTE,
		BITMAP,
		SHORT,
		UNSIGNED_SHORT,
		INT,
		UNSIGNED_INT,
		FLOAT,
		UNSIGNED_BYTE_3_3_2,
		UNSIGNED_BYTE_2_3_3_REV,
		UNSIGNED_SHORT_5_6_5,
		UNSIGNED_SHORT_5_6_5_REV,
		UNSIGNED_SHORT_4_4_4_4,
		UNSIGNED_SHORT_4_4_4_4_REV,
		UNSIGNED_SHORT_5_5_5_1,
		UNSIGNED_SHORT_1_5_5_5_REV,
		UNSIGNED_INT_8_8_8_8,
		UNSIGNED_INT_8_8_8_8_REV,
		UNSIGNED_INT_10_10_10_2,
		UNSIGNED_INT_2_10_10_10_REV
	};
}