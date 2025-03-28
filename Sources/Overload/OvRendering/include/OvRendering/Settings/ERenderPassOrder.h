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
	* Represents some culling options
	*/
	struct ERenderPassOrder
	{
		using PassType = uint32_t;

		static constexpr PassType First				= 00000U;
		static constexpr PassType Shadows			= 10000U;
		static constexpr PassType Opaque			= 20000U;
		static constexpr PassType Transparent		= 30000U;
		static constexpr PassType PostProcessing	= 40000U;
		static constexpr PassType UI				= 50000U;
		static constexpr PassType Debug				= 60000U;
		static constexpr PassType Last				= ~First;
	};
}