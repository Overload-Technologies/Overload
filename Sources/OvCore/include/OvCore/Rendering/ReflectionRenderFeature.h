/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/Resources/Material.h>
#include <OvCore/SceneSystem/SceneManager.h>

#include <OvRendering/Entities/Camera.h>
#include <OvRendering/Features/DebugShapeRenderFeature.h>

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

		/**
		* Prepare a given reflection probe for rendering.
		* @param p_reflectionProbe
		*/
		void PrepareProbe(OvCore::ECS::Components::CReflectionProbe& p_reflectionProbe);

		/**
		* Sends the reflection probe data to the material.
		* @param p_material
		* @param p_reflectionProbe (if set to nullopt, empty probe data will be sent)
		*/
		void SendProbeData(
			OvRendering::Data::Material& p_material,
			OvTools::Utils::OptRef<const OvCore::ECS::Components::CReflectionProbe> p_reflectionProbe
		);

		/**
		* Bind the reflection probe data to the proper binding point.
		* @param p_reflectionProbe
		* @param p_material
		*/
		void BindProbe(
			const OvCore::ECS::Components::CReflectionProbe& p_reflectionProbe
		);

	protected:
		virtual void OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor) override;
		virtual void OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable);
	};
}
