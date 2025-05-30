/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TTextureHandle.h>

namespace OvRendering::HAL
{
	struct NoneTextureHandleContext
	{
		const Settings::ETextureType type;
	};

	using NoneTextureHandle = TTextureHandle<Settings::EGraphicsBackend::NONE, NoneTextureHandleContext>;
}
