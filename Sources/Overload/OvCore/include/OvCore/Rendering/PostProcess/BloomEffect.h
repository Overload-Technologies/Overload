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
	/**
	* Settings for the BloomEffect
	*/
	struct BloomSettings : public EffectSettings
	{
		float threshold = 0.8f;
		float radius = 5.0f;
		int kernelSize = 6;
		float intensity = 0.6f;
		int passes = 10;
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
		std::array<OvRendering::HAL::Framebuffer, 2> m_bloomPingPong;
		OvRendering::Data::Material m_brightnessMaterial;
		OvRendering::Data::Material m_blurMaterial;
		OvRendering::Data::Material m_bloomMaterial;
	};
}