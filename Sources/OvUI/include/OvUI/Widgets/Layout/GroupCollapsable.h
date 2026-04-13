/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/Layout/Group.h"

namespace OvUI::Widgets::Layout
{
	/**
	* Widget that can contains other widgets and is collapsable
	*/
	class GroupCollapsable : public Group
	{
	public:
		/**
		* Constructor
		* @param p_name
		*/
		GroupCollapsable(const std::string& p_name = "");

	protected:
		virtual void _Draw_Impl() override;

	public:
		std::string name;
		bool closable = false;
		bool opened = true;
		bool reorderable = false;
		bool canMoveUp = true;
		bool canMoveDown = true;
		OvTools::Eventing::Event<> CloseEvent;
		OvTools::Eventing::Event<> OpenEvent;
		OvTools::Eventing::Event<> MoveUpEvent;
		OvTools::Eventing::Event<> MoveDownEvent;
	};
}