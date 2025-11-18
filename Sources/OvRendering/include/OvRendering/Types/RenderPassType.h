/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <concepts>

namespace OvRendering::Core
{
	class ARenderPass;
}

namespace OvRendering::Types
{
	template<typename T>
	concept RenderPassType = std::derived_from<T, OvRendering::Core::ARenderPass>;
}
