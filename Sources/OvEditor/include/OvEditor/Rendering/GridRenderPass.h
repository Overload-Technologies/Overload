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

#include "OvEditor/Core/Context.h"

namespace OvEditor::Rendering
{
	/**
	* Draw a grid
	*/
	class GridRenderPass : public OvRendering::Core::ARenderPass
	{
	public:
		struct GridDescriptor
		{
			OvMaths::FVector3 gridColor;
			OvMaths::FVector3 viewPosition;
		};

		/**
		* Constructor
		* @param p_renderer
		*/
		GridRenderPass(OvRendering::Core::CompositeRenderer& p_renderer);

	protected:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override;

	private:
		OvCore::Resources::Material m_gridMaterial;
	};
}