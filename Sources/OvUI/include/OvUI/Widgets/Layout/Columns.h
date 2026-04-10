/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <array>
#include <cfloat>
#include <imgui.h>

#include <OvUI/Internal/WidgetContainer.h>

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

			if (ImGui::BeginTable(("table" + m_widgetID).c_str(), static_cast<int>(_Size), tableFlags))
			{
				for (size_t i = 0; i < _Size; ++i)
				{
					const auto columnFlags = widths[i] != -1.f ?
						ImGuiTableColumnFlags_WidthFixed :
						ImGuiTableColumnFlags_WidthStretch;

					ImGui::TableSetupColumn(("##col_" + std::to_string(i)).c_str(), columnFlags, widths[i] != -1.f ? widths[i] : 0.0f);
				}

				size_t counter = 0;

				for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it)
				{
					if (counter == 0)
					{
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

				ImGui::EndTable();
			}
		}

	public:
		std::array<float, _Size> widths;
	};
}
