/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <OvUI/Types/Color.h>
#include <OvUI/Widgets/AWidget.h>

namespace OvUI::Widgets::Layout
{
	class TextLogs : public AWidget
	{
	public:
		struct Entry
		{
			std::string header;
			std::string content;
			Types::Color contentColor = Types::Color::White;
			uint32_t count = 1;
		};

	protected:
		void _Draw_Impl() override;

	private:
		void ProcessScrollRequest();
		static unsigned int GetRowBackgroundColor(int p_index);
		static float CalculateRowHeight(const std::string& p_message);

	public:
		void RequestScrollToBottom(uint8_t p_frames = 2);

		std::vector<Entry> entries;
		std::string emptyText = "No entries.";
		float badgeColumnWidth = 40.0f;

	private:
		uint8_t m_scrollToBottomFrames = 0;
		bool m_wasScrolledToBottom = true;
	};
}
