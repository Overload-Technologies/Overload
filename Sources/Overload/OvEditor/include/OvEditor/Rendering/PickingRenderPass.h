/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/Resources/Material.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/SceneSystem/SceneManager.h>

#include <OvEditor/Core/Context.h>
#include <OvEditor/Core/GizmoBehaviour.h>

#include <OvRendering/Entities/Camera.h>
#include <OvRendering/Features/DebugShapeRenderFeature.h>

namespace OvEditor::Rendering
{
	/**
	* Draw the scene for actor picking
	*/
	class PickingRenderPass : public OvRendering::Core::ARenderPass
	{
	public:
		using PickingResult =
			std::optional<
			std::variant<OvTools::Utils::OptRef<OvCore::ECS::Actor>,
			OvEditor::Core::GizmoBehaviour::EDirection>
			>;

		/**
		* Constructor
		* @param p_renderer
		*/
		PickingRenderPass(OvRendering::Core::CompositeRenderer& p_renderer);

		/**
		* Return the picking result at the given position
		* @param p_scene
		* @param p_x
		* @param p_y
		*/
		PickingResult ReadbackPickingResult(
			const OvCore::SceneSystem::Scene& p_scene,
			uint32_t p_x,
			uint32_t p_y
		);

	private:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override;
		void DrawPickableModels(OvRendering::Data::PipelineState p_pso, OvCore::SceneSystem::Scene& p_scene);
		void DrawPickableCameras(OvRendering::Data::PipelineState p_pso, OvCore::SceneSystem::Scene& p_scene);
		void DrawPickableLights(OvRendering::Data::PipelineState p_pso, OvCore::SceneSystem::Scene& p_scene);
		void DrawPickableGizmo(
			OvRendering::Data::PipelineState p_pso,
			const OvMaths::FVector3& p_position,
			const OvMaths::FQuaternion& p_rotation,
			OvEditor::Core::EGizmoOperation p_operation
		);

	private:
		OvRendering::HAL::Framebuffer m_actorPickingFramebuffer;
		OvCore::Resources::Material m_actorPickingMaterial;
		OvCore::Resources::Material m_lightMaterial;
		OvCore::Resources::Material m_gizmoPickingMaterial;
	};
}
