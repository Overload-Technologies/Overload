/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FMatrix4.h>

namespace OvCore::Rendering
{
	/**
	* Descriptor for drawable entities that adds a model and a user matrix.
	* This descriptor, when added on a drawable, is read by the EngineBufferRenderFeature
	* and its data is uploaded to the GPU before issuing a draw call.
	*/
	struct EngineDrawableDescriptor
	{
		OvMaths::FMatrix4 modelMatrix;
		OvMaths::FMatrix4 userMatrix;
	};
}
