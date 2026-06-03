/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Utils/OptRef.h>

#include <OvRendering/Entities/Camera.h>

#include <baregl/Framebuffer.h>

namespace OvRendering::Data
{
	/**
	* Describe how a given frame should be rendered
	*/
	struct FrameDescriptor
	{
		uint16_t renderWidth = 0;
		uint16_t renderHeight = 0;
		OvTools::Utils::OptRef<OvRendering::Entities::Camera> camera;
		OvTools::Utils::OptRef<baregl::Framebuffer> outputBuffer;

		/**
		* Ensures that the data provided in the frame descriptor is valid
		*/
		bool IsValid() const
		{
			return camera.has_value();
		}
	};
}
