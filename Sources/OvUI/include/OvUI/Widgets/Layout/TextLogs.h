/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <OvUI/Types/ColorEffector.h>
#include <OvUI/Styling/Style.h>
#include <OvUI/Widgets/AWidget.h>

namespace OvUI::Widgets::Layout
{
	/**
	* Widget used to display a scrollable list of selectable text logs
	*/
	class TextLogs : public AWidget
	{
	public:
		struct Entry
		{
			std::string header;
			std::string content;
			Types::ColorEffector contentColor = Types::ColorEffector::Ref(OVUI_STYLE(Text));
			uint32_t count = 1;
		};

	protected:
		void _Draw_Impl() override;

	private:
		void ProcessScrollRequest();
		static unsigned int GetRowBackgroundColor(int p_index);
		static float CalculateRowHeight(const std::string& p_message);

	public:
		/**
		* Request a scroll-to-bottom over a few frames to handle the first overflow frame correctly
		* @param p_frames
		*/
		void RequestScrollToBottom(uint8_t p_frames = 2);

		std::vector<Entry> entries;
		std::string emptyText = "No entries.";
		float badgeColumnWidth = 40.0f;

	private:
		uint8_t m_scrollToBottomFrames = 0;
		bool m_wasScrolledToBottom = true;
	};
}
