/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/AWidget.h"

namespace OvUI::Widgets::Visual
{
	/**
	* Simple widget that display a separator
	*/
	class Separator : public AWidget
	{
	public:
		bool fullWidthInColumns = false;

	protected:
		void _Draw_Impl() override;
	};
}
