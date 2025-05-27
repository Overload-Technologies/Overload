/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <map>

#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/Data/Frustum.h>
#include <OvRendering/Entities/Drawable.h>
#include <OvRendering/HAL/UniformBuffer.h>
#include <OvRendering/HAL/ShaderStorageBuffer.h>
#include <OvRendering/Resources/Mesh.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/Rendering/EVisibilityFlags.h>
#include <OvCore/Resources/Material.h>
#include <OvCore/SceneSystem/Scene.h>

namespace OvCore::Rendering
{
	/**
	* Extension of the CompositeRenderer adding support for the scene system (parsing/drawing entities)
	*/
	class SceneRenderer : public OvRendering::Core::CompositeRenderer
	{
	public:
		enum class EOrderingMode
		{
			BACK_TO_FRONT,
			FRONT_TO_BACK,
		};

		template<EOrderingMode OrderingMode>
		struct DrawOrder
		{
			const int order;
			const float distance;

			/**
			* Determines the order of the drawables.
			* Current order is: order -> distance
			* @param p_other
			*/
			bool operator<(const DrawOrder& p_other) const
			{
				if (order == p_other.order)
				{
					if constexpr (OrderingMode == EOrderingMode::BACK_TO_FRONT)
					{
						return distance > p_other.distance;
					}
					else
					{
						return distance < p_other.distance;
					}
				}
				else
				{
					return order < p_other.order;
				}
			}
		};

		template<EOrderingMode OrderingMode>
		using DrawableMap = std::multimap<DrawOrder<OrderingMode>, OvRendering::Entities::Drawable>;

		/**
		* Input data for the scene renderer.
		*/
		struct SceneDescriptor
		{
			OvCore::SceneSystem::Scene& scene;
			OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustumOverride;
			OvTools::Utils::OptRef<OvRendering::Data::Material> overrideMaterial;
			OvTools::Utils::OptRef<OvRendering::Data::Material> fallbackMaterial;
		};

		struct SceneParsingInput
		{
			OvCore::SceneSystem::Scene& scene;
		};

		/**
		* Result of the scene parsing, containing the drawables to be rendered.
		*/
		struct SceneDrawablesDescriptor
		{
			std::vector<OvRendering::Entities::Drawable> drawables;
		};

		/**
		* Additional information for a drawable computed by the scene renderer.
		*/
		struct SceneDrawableDescriptor
		{
			OvCore::ECS::Actor& actor;
			EVisibilityFlags visibilityFlags = EVisibilityFlags::NONE;
			std::optional<OvRendering::Geometry::BoundingSphere> bounds;
		};

		/**
		* Filtered drawables for the scene, categorized by their render pass, and sorted by their draw order.
		*/
		struct SceneFilteredDrawablesDescriptor
		{
			DrawableMap<EOrderingMode::FRONT_TO_BACK> opaques;
			DrawableMap<EOrderingMode::BACK_TO_FRONT> transparents;
			DrawableMap<EOrderingMode::BACK_TO_FRONT> ui;
		};

		struct SceneDrawablesFilteringInput
		{
			const OvRendering::Entities::Camera& camera;
			OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustumOverride;
			OvTools::Utils::OptRef<OvRendering::Data::Material> overrideMaterial;
			OvTools::Utils::OptRef<OvRendering::Data::Material> fallbackMaterial;
			EVisibilityFlags requiredVisibilityFlags = EVisibilityFlags::NONE;
			bool includeUI = true; // Whether to include UI drawables in the filtering
			bool includeTransparent = true; // Whether to include transparent drawables in the filtering
			bool includeOpaque = true; // Whether to include opaque drawables in the filtering
		};

		/**
		* Constructor of the Renderer
		* @param p_driver
		* @param p_stencilWrite (if set to true, also write all the scene geometry to the stencil buffer)
		*/
		SceneRenderer(OvRendering::Context::Driver& p_driver, bool p_stencilWrite = false);

		/**
		* Begin Frame
		* @param p_frameDescriptor
		*/
		virtual void BeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor) override;

		/**
		* Draw a model with a single material
		* @param p_pso
		* @param p_model
		* @param p_material
		* @param p_modelMatrix
		*/
		virtual void DrawModelWithSingleMaterial(
			OvRendering::Data::PipelineState p_pso,
			OvRendering::Resources::Model& p_model,
			OvRendering::Data::Material& p_material,
			const OvMaths::FMatrix4& p_modelMatrix
		);

		/**
		* Parse the scene (as defined in the SceneDescriptor) to find the drawables to render.
		* @param p_sceneDescriptor
		* @param p_options
		*/
		SceneDrawablesDescriptor ParseScene(
			const SceneParsingInput& p_input
		);

		/**
		* Filter and prepare drawables based on the given context.
		* This is where culling and sorting happens.
		* @param p_drawables
		* @param p_filteringInput
		*/
		SceneFilteredDrawablesDescriptor FilterDrawables(
			const SceneDrawablesDescriptor& p_drawables,
			const SceneDrawablesFilteringInput& p_filteringInput
		);
	};
}
