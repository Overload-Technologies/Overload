/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <map>

#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/Resources/Mesh.h>
#include <OvRendering/Data/Frustum.h>
#include <OvRendering/Entities/Drawable.h>
#include <OvRendering/HAL/UniformBuffer.h>
#include <OvRendering/HAL/ShaderStorageBuffer.h>

#include "OvCore/Resources/Material.h"
#include "OvCore/ECS/Actor.h"
#include "OvCore/ECS/Components/CCamera.h"
#include "OvCore/SceneSystem/Scene.h"

namespace OvCore::Rendering
{
	/**
	* Extension of the CompositeRenderer adding support for the scene system (parsing/drawing entities)
	*/
	class SceneRenderer : public OvRendering::Core::CompositeRenderer
	{
	public:
		struct DrawOrder
		{
			int order;
			float distance;

			/**
			* Determines the order of the drawables.
			* Current order is: order -> distance
			* @param p_other
			*/
			bool operator<(const DrawOrder& p_other) const;
			bool operator>(const DrawOrder& p_other) const;
		};

		using OpaqueDrawables = std::multimap<DrawOrder, OvRendering::Entities::Drawable, std::less<DrawOrder>>;
		using TransparentDrawables = std::multimap<DrawOrder, OvRendering::Entities::Drawable, std::greater<DrawOrder>>;
		using UIDrawables = std::multimap<DrawOrder, OvRendering::Entities::Drawable, std::greater<DrawOrder>>;

		struct AllDrawables
		{
			OpaqueDrawables opaques;
			TransparentDrawables transparents;
			UIDrawables ui;
		};

		struct SceneDescriptor
		{
			OvCore::SceneSystem::Scene& scene;
			OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustumOverride;
			OvTools::Utils::OptRef<OvRendering::Data::Material> overrideMaterial;
			OvTools::Utils::OptRef<OvRendering::Data::Material> fallbackMaterial;
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

	protected:
		AllDrawables ParseScene();
	};
}
