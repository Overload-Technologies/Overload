/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Entities/Camera.h>
#include <OvRendering/Features/DebugShapeRenderFeature.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/SceneSystem/SceneManager.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/Resources/Material.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>

namespace OvCore::Rendering
{
	/**
	* Ensures that reflection probes data is provided to
	* the appropriate drawable entities.
	*/
	class ReflectionRenderFeature : public OvRendering::Features::ARenderFeature
	{
	public:
		/**
		* Provided by the scene renderer, this descriptor contains
		* reflection probes to evaluate and provide to the drawables.
		*/
		struct ReflectionDescriptor
		{
			std::vector<std::reference_wrapper<ECS::Components::CReflectionProbe>> reflectionProbes;
		};

		/**
		* Constructor
		* @param p_renderer
		* @param p_executionPolicy
		*/
		ReflectionRenderFeature(
			OvRendering::Core::CompositeRenderer& p_renderer,
			OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
		);

	protected:
		virtual void OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable);
	};
}
