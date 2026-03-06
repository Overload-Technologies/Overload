/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <concepts>

namespace OvRendering::Features
{
	class ARenderFeature;
}

namespace OvRendering::Types
{
	template<typename T>
	concept RenderFeatureType = std::derived_from<T, OvRendering::Features::ARenderFeature>;
}
