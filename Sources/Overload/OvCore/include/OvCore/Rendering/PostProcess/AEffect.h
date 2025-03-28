/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Core/ARenderPass.h>
#include <OvRendering/Data/Material.h>
#include <OvRendering/Core/CompositeRenderer.h>

namespace OvCore::Rendering::PostProcess
{
	/**
	* Base effect settings structure
	*/
	struct EffectSettings
	{
		bool enabled = true;
	};

	/**
	* Base class for any post-processing effect
	*/
	class AEffect
	{
	public:
		/**
		* Constructor of the effect class
		* @param p_renderer
		*/
		AEffect(OvRendering::Core::CompositeRenderer& p_renderer);

		/**
		* Returns true if the effect is applicable with the given settings.
		* If the effect is not applicable, it will be skipped by the post processing render pass
		* @param p_settings
		*/
		virtual bool IsApplicable(const EffectSettings& p_settings) const;

		/**
		* Draw the effect
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
		) = 0;

	protected:
		OvRendering::Core::CompositeRenderer& m_renderer;
	};
}
