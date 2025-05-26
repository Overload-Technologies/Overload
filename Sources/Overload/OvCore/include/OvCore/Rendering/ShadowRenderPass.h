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
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/Rendering/SceneRenderer.h>

namespace OvCore::Rendering
{
	/**
	* Draw the scene to a depth buffer from the point of view of each light source
	*/
	class ShadowRenderPass : public OvRendering::Core::ARenderPass
	{
	public:
		/**
		* Constructor
		* @param p_renderer
		*/
		ShadowRenderPass(OvRendering::Core::CompositeRenderer& p_renderer);

	private:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override;

		void _DrawShadows(
			OvRendering::Data::PipelineState p_pso,
			OvCore::SceneSystem::Scene& p_scene
		);

	private:
		OvCore::Resources::Material m_shadowMaterial;
	};
}