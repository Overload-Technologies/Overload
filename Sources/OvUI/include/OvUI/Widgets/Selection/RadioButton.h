/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/DataWidget.h"

namespace OvUI::Widgets::Selection
{
	class RadioButtonLinker;

	/**
	* Widget that represent a button that is selectable. Only one radio button can be selected
	* in the same RadioButtonLinker
	*/
	class RadioButton : public DataWidget<bool>
	{
		friend RadioButtonLinker;
		
	public:
		/**
		* Constructor
		* @param p_selected
		* @param p_label
		*/
		RadioButton(bool p_selected = false, const std::string& p_label = "");

		/**
		* Make the radio button selected
		*/
		void Select();

		/**
		* Returns true if the radio button is selected
		*/
		bool IsSelected() const;

		/**
		* Returns the radio button ID
		*/
		bool GetRadioID() const;

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		OvTools::Eventing::Event<int> ClickedEvent;

	private:
		bool m_selected = false;
		int m_radioID = 0;
	};
}