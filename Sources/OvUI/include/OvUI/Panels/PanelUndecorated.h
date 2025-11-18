/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Panels/APanelTransformable.h"

namespace OvUI::Panels
{
	/**
	* A simple panel that is transformable and without decorations (No background)
	*/
	class PanelUndecorated : public APanelTransformable
	{
	public:
		void _Draw_Impl() override;

	private:
		int CollectFlags();
	};
}