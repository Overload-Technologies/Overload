/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Core/ARenderPass.h>
#include <OvRendering/HAL/Framebuffer.h>
#include <OvCore/Rendering/PostProcess/AEffect.h>
#include <OvCore/SceneSystem/Scene.h>

namespace OvCore::Rendering
{
	/**
	* Draw reflections
	*/
	class ReflectionRenderPass : public OvRendering::Core::ARenderPass
	{
	public:
		/**
		* Constructor of the post-process render pass
		* @param p_renderer
		*/
		ReflectionRenderPass(OvRendering::Core::CompositeRenderer& p_renderer);

	protected:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override;

		void _DrawReflections(OvRendering::Data::PipelineState p_pso, OvCore::SceneSystem::Scene& p_scene);
	};
}
