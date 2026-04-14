/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <OvCore/Helpers/GUIHelpers.h>
#include <OvUI/Panels/PanelWindow.h>

namespace OvUI::Widgets
{
	namespace InputFields { class InputText; }
	namespace Layout { class Group; }
}

namespace OvEditor::Panels
{
	/**
	* A floating search-and-pick panel.
	* Fully generic: open it with any pre-built PickerItemList and it will
	* display the items with their icons, filter them as the user types, and
	* invoke each item's onSelected callback on click.
	*
	* Use OvEditor::Helpers::PickerHelpers::AddFileItems() (and similar helpers)
	* to build the list before opening.
	*/
	class ItemPicker : public OvUI::Panels::PanelWindow
	{
	public:
		ItemPicker(
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Open the picker with the given item list.
		* The window title is updated to p_title and it is positioned near
		* the widget that triggered the click.
		* @param p_items
		* @param p_title  Title displayed in the window's title bar
		*/
		void Open(OvCore::Helpers::GUIHelpers::PickerItemList p_items, std::string p_title);

	private:
		void _Draw_Impl() override;
		void Populate();
		void FilterList(const std::string& p_search);

	private:
		/* When true, _Draw_Impl will inject SetNextWindowPos with a pivot
		   (bottom-anchor) before calling the parent, then clear the flag. */
		bool m_usePivotAnchor = false;
		float m_anchorX = 0.f;
		float m_anchorY = 0.f;  // bottom-left Y when m_usePivotAnchor is true

	private:
		OvCore::Helpers::GUIHelpers::PickerItemList m_items;

		OvUI::Widgets::InputFields::InputText* m_searchField = nullptr;
		OvUI::Widgets::Layout::Group* m_listGroup = nullptr;

		/* Each entry: (search key, row widget) */
		std::vector<std::pair<std::string, OvUI::Widgets::Layout::Group*>> m_rows;
	};
}
