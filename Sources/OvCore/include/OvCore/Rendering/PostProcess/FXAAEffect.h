/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Rendering/PostProcess/AEffect.h>
#include <OvRendering/Data/Material.h>

namespace OvCore::Rendering::PostProcess
{
	/**
	* Settings for the FXAA effect
	*/
	struct FXAASettings : public EffectSettings {};

	/**
	* Post-processing effect that applies FXAA (Fast Approximate Anti-Aliasing).
	*/
	class FXAAEffect : public AEffect
	{
	public:
		/**
		* Constructor of the FXAAEffect class
		* @param p_renderer
		*/
		FXAAEffect(OvRendering::Core::CompositeRenderer& p_renderer);

		/**
		* Render the FXAA effect
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
		OvRendering::Data::Material m_material;
	};
}
