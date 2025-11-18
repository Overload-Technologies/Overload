/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Rendering/PostProcess/AEffect.h>
#include <OvRendering/Data/Material.h>
#include <OvRendering/HAL/Framebuffer.h>

namespace OvCore::Rendering::PostProcess
{
	namespace BloomConstants
	{
		constexpr uint32_t kMinPassCount = 1;
		constexpr uint32_t kMaxPassCount = 10;
		constexpr float kMaxBloomIntensity = 1.0f / 0.04f; // 0.04f being the default internal interpolation factor.
	}

	/**
	* Settings for the BloomEffect
	*/
	struct BloomSettings : public EffectSettings
	{
		float intensity = 1.0f;
		int passes = 6;
	};

	/**
	* Bloom post-processing effect:
	* This effect will make the bright parts of the image glow
	*/
	class BloomEffect : public AEffect
	{
	public:
		/**
		* Constructor of the BloomEffect class
		* @param p_renderer
		*/
		BloomEffect(OvRendering::Core::CompositeRenderer& p_renderer);

		/**
		* Returns true if the effect is applicable with the given settings.
		* If the effect is not applicable, it will be skipped by the post processing render pass
		* @param p_settings
		*/
		virtual bool IsApplicable(const EffectSettings& p_settings) const override;

		/**
		* Render the bloom effect
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
		std::array<OvRendering::HAL::Framebuffer, BloomConstants::kMaxPassCount> m_bloomSamplingBuffers;
		OvRendering::HAL::Framebuffer m_bloomOutputBuffer;
		OvRendering::Data::Material m_downsamplingMaterial;
		OvRendering::Data::Material m_upsamplingMaterial;
		OvRendering::Data::Material m_bloomMaterial;
		OvRendering::Data::Material m_blitMaterial;
	};
}