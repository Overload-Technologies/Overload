/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/DataWidget.h"
#include "OvUI/Types/Color.h"

namespace OvUI::Widgets::Selection
{
	/**
	* Widget that allow the selection of a color with a color picker
	*/
	class ColorPicker : public DataWidget<Types::Color>
	{
	public:
		/**
		* Constructor
		* @param p_enableAlpha
		* @param p_defaultColor
		*/
		ColorPicker(bool p_enableAlpha = false, const Types::Color& p_defaultColor = {});

	protected:
		void _Draw_Impl() override;

	public:
		bool enableAlpha;
		Types::Color color;
		OvTools::Eventing::Event<Types::Color&> ColorChangedEvent;
	};
}