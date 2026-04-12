/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>
#include <cfloat>
#include <iterator>
#include <imgui.h>
#include <imgui_internal.h>

#include <OvUI/Internal/WidgetContainer.h>
#include <OvUI/Widgets/Visual/Separator.h>

namespace OvUI::Widgets::Layout
{
	/**
	* Widget that allow columnification
	*/
	template <size_t _Size>
	class Columns : public AWidget, public Internal::WidgetContainer
	{
	public:
		/**
		* Constructor
		*/
		Columns()
		{
			widths.fill(-1.f);
		}

	protected:
		virtual void _Draw_Impl() override
		{
			CollectGarbages();

			constexpr auto tableFlags =
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_NoSavedSettings |
				ImGuiTableFlags_NoBordersInBodyUntilResize;

			auto drawTableSegment =
				[this](auto p_begin, auto p_end, size_t p_segmentIndex)
			{
				if (p_begin == p_end)
				{
					return;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0.0f, ImGui::GetStyle().CellPadding.y });

				if (ImGui::BeginTable(("table" + m_widgetID + "_" + std::to_string(p_segmentIndex)).c_str(), static_cast<int>(_Size), tableFlags))
				{
					for (size_t i = 0; i < _Size; ++i)
					{
						const auto columnFlags = widths[i] != -1.f ?
							ImGuiTableColumnFlags_WidthFixed :
							ImGuiTableColumnFlags_WidthStretch;

						ImGui::TableSetupColumn(("##col_" + std::to_string(i)).c_str(), columnFlags, widths[i] != -1.f ? widths[i] : 0.0f);
					}

					size_t counter = 0;

					for (auto it = p_begin; it != p_end; ++it)
					{
						if (counter == 0)
						{
							// Skip the row entirely if all its widgets are disabled
							bool allDisabled = true;
							auto lookahead = it;
							for (size_t k = 0; k < _Size && lookahead != p_end; ++k, ++lookahead)
							{
								if (lookahead->first->enabled)
								{
									allDisabled = false;
									break;
								}
							}

							if (allDisabled)
							{
								for (size_t k = 1; k < _Size && std::next(it) != p_end; ++k)
									++it;
								continue;
							}

							ImGui::TableNextRow();
						}

						ImGui::TableSetColumnIndex(static_cast<int>(counter));

						if (widths[counter] == -1.f)
						{
							ImGui::SetNextItemWidth(-FLT_MIN);
						}

						it->first->Draw();

						counter = (counter + 1) % _Size;
					}

					for (size_t i = 0; i < _Size; ++i)
					{
						if (widths[i] != -1.f)
						{
							widths[i] = GImGui->CurrentTable->Columns[static_cast<int>(i)].WidthGiven;
						}
					}

					ImGui::EndTable();
				}

				ImGui::PopStyleVar();
			};

			auto segmentBegin = m_widgets.begin();
			size_t segmentIndex = 0;

			for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it)
			{
				if (dynamic_cast<Visual::Separator*>(it->first))
				{
					drawTableSegment(segmentBegin, it, segmentIndex++);
					it->first->Draw();
					segmentBegin = std::next(it);
				}
			}

			drawTableSegment(segmentBegin, m_widgets.end(), segmentIndex);
		}

	public:
		std::array<float, _Size> widths;
	};
}
