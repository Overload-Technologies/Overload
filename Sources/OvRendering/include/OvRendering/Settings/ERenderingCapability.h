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
	* Rendering Capabilities
	*/
	enum class ERenderingCapability : uint8_t
	{
		BLEND,						// Blend the computed fragment color values with the values in the color buffers. See glBlendFunc.
		CULL_FACE,					// Cull polygons based on their winding in window coordinates.
		DEPTH_TEST,					// Do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled.
		DITHER,						// Dither color components or indices before they are written to the color buffer.
		POLYGON_OFFSET_FILL,		// An offset is added to depth values of a polygon's fragments produced by rasterization.
		SAMPLE_ALPHA_TO_COVERAGE,	// Compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location. The temporary coverage value is then ANDed with the fragment coverage value.
		SAMPLE_COVERAGE,			// The fragment's coverage is ANDed with the temporary coverage value. If GL_SAMPLE_COVERAGE_INVERT is set to GL_TRUE, invert the coverage value.
		SCISSOR_TEST,				// Discard fragments that are outside the scissor rectangle.
		STENCIL_TEST,				// Do stencil testing and update the stencil buffer.
		MULTISAMPLE					// Use multiple fragment samples in computing the final color of a pixel.	
	};
}