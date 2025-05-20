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
#include "OvEditor/Core/GizmoBehaviour.h"

#include "OvEditor/Core/Context.h"

namespace OvEditor::Panels { class AView; }

namespace OvEditor::Rendering
{
	enum class EDebugViewMode
	{
		NONE,
		ALBEDO,
		METALLIC,
		ROUGHNESS,
		AO,
		NORMAL,
		UV,
		DEPTH,
		WIREFRAME
	};

	/**
	* Provide a debug layer on top of the default scene renderer to see "invisible" entities such as
	* lights, cameras, 
	*/
	class DebugSceneRenderer : public OvCore::Rendering::SceneRenderer
	{
	public:
		struct DebugSceneDescriptor
		{
			OvEditor::Core::EGizmoOperation gizmoOperation;
			OvTools::Utils::OptRef<OvCore::ECS::Actor> highlightedActor;
			OvTools::Utils::OptRef<OvCore::ECS::Actor> selectedActor;
			std::optional<OvEditor::Core::GizmoBehaviour::EDirection> highlightedGizmoDirection;
		};

		/**
		* Constructor of the Renderer
		* @param p_driver
		*/
		DebugSceneRenderer(OvRendering::Context::Driver& p_driver);

		/**
		* Set a debug view mode
		* @param p_mode
		*/
		void SetDebugViewMode(EDebugViewMode p_mode) const;
	};
}