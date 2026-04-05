/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <span>

#include <OvRendering/Settings/EDataType.h>

namespace OvRendering::Settings
{
	/**
	* Structure that holds information about a uniform
	*/
	struct VertexAttribute
	{
		EDataType type = EDataType::FLOAT;
		uint8_t count = 4;
		bool normalized = false;
	};

	using VertexAttributeLayout = std::span<const Settings::VertexAttribute>;
}
