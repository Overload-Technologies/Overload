/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>

#include "OvRendering/Data/PipelineState.h"

namespace OvRendering::Settings
{
	/**
	* Settings that are sent to the driver at construction
	*/
	struct DriverSettings
	{
		bool debugMode = false;
		std::optional<OvRendering::Data::PipelineState> defaultPipelineState = std::nullopt;
	};
}
