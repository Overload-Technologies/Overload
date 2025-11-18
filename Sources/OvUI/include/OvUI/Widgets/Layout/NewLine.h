/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/AWidget.h"

namespace OvUI::Widgets::Layout
{
	/**
	* Widget that adds an empty line to the panel
	*/
	class NewLine : public AWidget
	{
	protected:
		void _Draw_Impl() override;
	};
}