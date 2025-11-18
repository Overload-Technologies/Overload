/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Resources/Loaders/TextureLoader.h>

#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/GroupCollapsable.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

#include "OvEditor/Panels/Hierarchy.h"
#include "OvEditor/Panels/AssetBrowser.h"

namespace OvEditor::Panels
{
	class Inspector : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Inspector
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Destructor
		*/
		~Inspector();

		/**
		* Focus the given actor
		* @param p_target
		*/
		void FocusActor(OvCore::ECS::Actor& p_target);

		/**
		* Unfocus the currently targeted actor
		*/
		void UnFocus();

		/**
		* Returns the currently selected actor
		*/
		OvTools::Utils::OptRef<OvCore::ECS::Actor> GetTargetActor() const;

		/**
		* Refresh the inspector
		*/
		void Refresh();

	private:
		void _Populate();
		void _PopulateActorInfo();
		void _PopulateActorComponents();
		void _PopulateActorBehaviours();
		void _DrawAddComponentSection();
		void _DrawAddScriptSection();
		void _DrawComponent(OvCore::ECS::Components::AComponent& p_component);
		void _DrawBehaviour(OvCore::ECS::Components::Behaviour& p_behaviour);
		void _UpdateAddComponentButton();
		void _UpdateAddScriptButton();

	private:
		OvTools::Utils::OptRef<OvCore::ECS::Actor> m_targetActor = std::nullopt;
		OvUI::Widgets::Layout::Group* m_content;

		OvTools::Utils::OptRef<OvUI::Widgets::Buttons::Button> m_addComponentButton;
		OvTools::Utils::OptRef<OvUI::Widgets::Buttons::Button> m_addScriptButton;

		// We store this here so that re-creating the inspector doesn't
		// reset the selected component.
		int m_selectedComponent = 0;

		// Same as the selected component above, we want to keep the value
		// of the selected script even if the inspector is re-created
		std::string m_selectedScript;

		uint64_t m_componentAddedListener	= 0;
		uint64_t m_componentRemovedListener = 0;
		uint64_t m_behaviourAddedListener	= 0;
		uint64_t m_behaviourRemovedListener = 0;
		uint64_t m_destroyedListener		= 0;
	};
}