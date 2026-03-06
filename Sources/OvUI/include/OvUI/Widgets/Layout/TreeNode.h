/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvTools/Eventing/Event.h>

#include "OvUI/Internal/WidgetContainer.h"
#include "OvUI/Widgets/DataWidget.h"

namespace OvUI::Widgets::Layout
{
	/**
	* Widget that allow columnification
	*/
	class TreeNode : public DataWidget<std::string>, public Internal::WidgetContainer 
	{
	public:
		/**
		* Constructor
		* @param p_name
		* @param p_arrowClickToOpen
		*/
		TreeNode(const std::string& p_name = "", bool p_arrowClickToOpen = false);

		/**
		* Open the tree node
		*/
		void Open();

		/**
		* Close the tree node
		*/
		void Close();

		/**
		* Returns true if the TreeNode is currently opened
		*/
		bool IsOpened() const;

	protected:
		virtual void _Draw_Impl() override;

	public:
		std::string name;
		bool selected = false;
		bool leaf = false;

		OvTools::Eventing::Event<> ClickedEvent;
		OvTools::Eventing::Event<> DoubleClickedEvent;
		OvTools::Eventing::Event<> OpenedEvent;
		OvTools::Eventing::Event<> ClosedEvent;

	private:
		bool m_arrowClickToOpen = false;
		bool m_shouldOpen = false;
		bool m_shouldClose = false;
		bool m_opened = false;
	};
}