/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <chrono>

#include <OvCore/Rendering/PostProcess/AEffect.h>
#include <OvRendering/Data/Material.h>

namespace OvCore::Rendering::PostProcess
{
	/**
	* Settings for the AutoExposure effect
	*/
	struct AutoExposureSettings : public EffectSettings
	{
		float centerWeightBias = 0.5f;
		float minLuminanceEV = -2.0f;
		float maxLuminanceEV = 16.0f;
		float exposureCompensationEV = 0.0f;
		bool progressive = true;
		float speedUp = 1.0f;
		float speedDown = 1.0f;
	};

	/**
	* Post-processing effect that adapts the exposure based on the scene luminance.
	*/
	class AutoExposureEffect : public AEffect
	{
	public:
		/**
		* Constructor of the AutoExposureEffect class
		* @param p_renderer
		*/
		AutoExposureEffect(OvRendering::Core::CompositeRenderer& p_renderer);

		/**
		* Render the Auto-Exposure effect
		* @note: make sure the effect is applicable before calling this method
		* @param p_pso
		* @param p_src
		* @param p_dst
		* @param p_settings
		*/
		virtual void Draw(
			OvRendering::Data::PipelineState p_pso,
			OvRendering::HAL::Framebuffer& p_src,
			OvRendering::HAL::Framebuffer& p_dst,
			const EffectSettings& p_settings
		) override;

	private:
		std::optional<std::chrono::high_resolution_clock::time_point> m_previousTime;
		OvRendering::HAL::Framebuffer m_luminanceBuffer;
		std::array<OvRendering::HAL::Framebuffer, 2> m_exposurePingPongBuffer;
		uint8_t m_exposurePingPongIndex = 0;
		OvRendering::Data::Material m_luminanceMaterial;
		OvRendering::Data::Material m_exposureMaterial;
		OvRendering::Data::Material m_compensationMaterial;
	};
}