/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvTools/Eventing/Event.h>
#include <OvUI/Widgets/DataWidget.h>

namespace OvUI::Widgets::InputFields
{
	/**
	* Read-only input field that displays an asset path and includes an attached "..." picker button.
	* Behaves like a text input (framed, drag-and-drop target) while the button opens the asset picker.
	*/
	class AssetField : public DataWidget<std::string>
	{
	public:
		/**
		* Constructor
		* @param p_content  Initial content (asset path or empty string)
		*/
		AssetField(const std::string& p_content = "");

	protected:
		void _Draw_Impl() override;

	public:
		std::string content;
		uint32_t iconTextureID = 0;

		/** When non-zero, a framed preview image is drawn above the input row. */
		uint32_t previewTextureID = 0;
		float previewSize = 64.0f;

		OvTools::Eventing::Event<> ClickedEvent;
		OvTools::Eventing::Event<> DoubleClickedEvent;
	};
}
