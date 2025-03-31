/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Resources/Loaders/TextureLoader.h>
#include <OvRendering/Entities/Camera.h>

#include <OvCore/SceneSystem/SceneManager.h>

#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/TreeNode.h>

namespace OvEditor::Panels
{
	class Hierarchy : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Hierarchy
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);
		
		/**
		* Clear hierarchy nodes
		*/
		void Clear();

		/**
		* Unselect every widgets
		*/
		void UnselectActorsWidgets();

		/**
		* Select the widget corresponding to the given actor
		* @param p_actor
		*/
		void SelectActorByInstance(OvCore::ECS::Actor& p_actor);

		/**
		* Select the widget
		* @param p_actor
		*/
		void SelectActorByWidget(OvUI::Widgets::Layout::TreeNode& p_widget);

		/**
		* Attach the given actor linked widget to its parent widget
		* @param p_actor
		*/
		void AttachActorToParent(OvCore::ECS::Actor& p_actor);

		/**
		* Detach the given actor linked widget from its parent widget
		* @param p_actor
		*/
		void DetachFromParent(OvCore::ECS::Actor& p_actor);

		/**
		* Delete the widget referencing the given actor
		* @param p_actor
		*/
		void DeleteActorByInstance(OvCore::ECS::Actor& p_actor);

		/**
		* Add a widget referencing the given actor
		* @param p_actor
		*/
		void AddActorByInstance(OvCore::ECS::Actor& p_actor);

	public:
		OvTools::Eventing::Event<OvCore::ECS::Actor&> ActorSelectedEvent;
		OvTools::Eventing::Event<OvCore::ECS::Actor&> ActorUnselectedEvent;

	private:
		std::unordered_map<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*> m_widgetActorLink;
	};
}