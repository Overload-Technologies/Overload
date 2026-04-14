/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/InputFields/ObjectField.h>

namespace OvUI::Widgets::InputFields
{
	template<typename T>
	ObjectField<T>::ObjectField(T p_content, std::string p_displayName)
		: DataWidget<T>(content), content(std::move(p_content)), displayName(std::move(p_displayName))
	{
	}

	template<typename T>
	void ObjectField<T>::_Draw_Impl()
	{
		const float buttonSize = ImGui::GetFrameHeight();
		const float innerSize = buttonSize - 2.0f * ImGui::GetStyle().FramePadding.x;

		const std::string name = displayFormatter ? displayFormatter(content) : displayName;
		const std::string label = name.empty() ? "None" : name;
		const std::string tip = tooltipFormatter ? tooltipFormatter(content) : tooltip;

		ImGui::BeginGroup();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize);

		char buf[512] = {};
		label.copy(buf, sizeof(buf) - 1);
		ImGui::BeginDisabled();
		ImGui::InputText((this->m_widgetID + "i").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
		ImGui::EndDisabled();

		if (!tip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
			ImGui::SetTooltip("%s", tip.c_str());

		if (content != T{} && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			DoubleClickedEvent.Invoke();

		ImGui::SameLine(0, 0);

		bool clicked;
		if (iconTextureID != 0)
		{
			clicked = ImGui::ImageButton(
				("icon" + this->m_widgetID).c_str(),
				(ImTextureID)(uintptr_t)iconTextureID,
				ImVec2(innerSize, innerSize),
				ImVec2(0.f, 1.f),
				ImVec2(1.f, 0.f)
			);
		}
		else
		{
			clicked = ImGui::Button(("..." + this->m_widgetID).c_str(), ImVec2(buttonSize, buttonSize));
		}

		if (clicked)
			ClickedEvent.Invoke();

		if (previewTextureID != 0)
		{
			const float gap = ImGui::GetStyle().FramePadding.y;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y + gap);

			ImVec2 imgMin = ImGui::GetCursorScreenPos();
			ImGui::Image(
				(ImTextureID)(uintptr_t)previewTextureID,
				ImVec2(previewSize, previewSize),
				ImVec2(0.f, 1.f), ImVec2(1.f, 0.f)
			);
			ImGui::GetWindowDrawList()->AddRect(
				imgMin,
				ImVec2(imgMin.x + previewSize, imgMin.y + previewSize),
				ImGui::GetColorU32(ImGuiCol_Border)
			);
			if (ImGui::IsItemClicked())
				ClickedEvent.Invoke();
		}

		ImGui::EndGroup();
	}

	template class ObjectField<std::string>;
	template class ObjectField<uint64_t>;
}
