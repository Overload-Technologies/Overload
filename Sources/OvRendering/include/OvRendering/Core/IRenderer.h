/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvRendering/Data/FrameDescriptor.h"

namespace OvRendering::Core
{
	class IRenderer
	{
	public:
		virtual void BeginFrame(const Data::FrameDescriptor& p_frameDescriptor) = 0;
		virtual void DrawFrame() = 0;
		virtual void EndFrame() = 0;
	};
}