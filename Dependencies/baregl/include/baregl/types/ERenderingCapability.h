/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace baregl::types
{
	/**
	* Enumeration of rendering capabilities
	*/
	enum class ERenderingCapability : uint8_t
	{
		BLEND,
		CULL_FACE,
		DEPTH_TEST,
		DITHER,
		POLYGON_OFFSET_FILL,
		SAMPLE_ALPHA_TO_COVERAGE,
		SAMPLE_COVERAGE,
		SCISSOR_TEST,
		STENCIL_TEST,
		MULTISAMPLE
	};
}
