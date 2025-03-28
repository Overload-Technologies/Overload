/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvRendering/Core/CompositeRenderer.h"
#include "OvRendering/Features/ARenderFeature.h"
#include "OvRendering/Data/FrameInfo.h"
#include "OvRendering/Entities/Light.h"
#include "OvRendering/HAL/ShaderStorageBuffer.h"
#include "OvRendering/Data/Frustum.h"

namespace OvRendering::Features
{
	class LightingRenderFeature : public ARenderFeature
	{
	public:
		// TODO: Consider not using references here, but copying the light instead (should be fairly cheap and doesn't require to keep an instance outside of the scope)
		using LightSet = std::vector<std::reference_wrapper<OvRendering::Entities::Light>>;

		struct LightingDescriptor
		{
			LightSet lights;
			OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustumOverride;
		};

		/**
		* Constructor
		* @param p_renderer
		* @param p_bufferBindingPoint
		*/
		LightingRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer, uint32_t p_bufferBindingPoint = 0);


	protected:
		virtual void OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnEndFrame() override;

	private:
		uint32_t m_bufferBindingPoint;
		std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer> m_lightBuffer;
	};
}
