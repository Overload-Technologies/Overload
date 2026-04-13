/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Layout/TreeNode.h>

OvUI::Widgets::Layout::TreeNode::TreeNode(const std::string & p_name, bool p_arrowClickToOpen) :
	DataWidget(name),
	name(p_name),
	m_arrowClickToOpen(p_arrowClickToOpen)
{
	m_autoExecutePlugins = false;
}

void OvUI::Widgets::Layout::TreeNode::Open()
{
	m_shouldOpen = true;
	m_shouldClose = false;
}

void OvUI::Widgets::Layout::TreeNode::Close()
{
	m_shouldClose = true;
	m_shouldOpen = false;
}

bool OvUI::Widgets::Layout::TreeNode::IsOpened() const
{
	return m_opened;
}

void OvUI::Widgets::Layout::TreeNode::_Draw_Impl()
{
	bool prevOpened = m_opened;

	if (m_shouldOpen)
	{
		ImGui::SetNextItemOpen(true);
		m_shouldOpen = false;
	}
	else if (m_shouldClose)
	{
		ImGui::SetNextItemOpen(false);
		m_shouldClose = false;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (m_arrowClickToOpen) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	if (selected)			flags |= ImGuiTreeNodeFlags_Selected;
	if (leaf)				flags |= ImGuiTreeNodeFlags_Leaf;

	if (overrideLabelColor)
		ImGui::PushStyleColor(ImGuiCol_Text, {labelColor.r, labelColor.g, labelColor.b, labelColor.a});

	const bool hasIcon = iconTextureID != 0;
	bool opened;
	bool itemClicked = false;

	if (hasIcon)
	{
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		opened = ImGui::TreeNodeEx(m_widgetID.c_str(), flags);
		itemClicked = ImGui::IsItemClicked();

		// Draw icon and text via the draw list so they don't create new ImGui
		// items (the TreeNodeEx must remain the "last item" for plugins like
		// ContextualMenu that call BeginPopupContextItem).
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const float startX = ImGui::GetItemRectMin().x + ImGui::GetTreeNodeToLabelSpacing();
		const float centerY = (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) * 0.5f;

		const ImU32 tintCol = overrideLabelColor
			? ImGui::ColorConvertFloat4ToU32({labelColor.r, labelColor.g, labelColor.b, labelColor.a})
			: IM_COL32_WHITE;
		const ImVec2 iconMin(startX, centerY - iconSize * 0.5f);
		const ImVec2 iconMax(startX + iconSize, centerY + iconSize * 0.5f);
		drawList->AddImage(iconTextureID, iconMin, iconMax, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), tintCol);

		const float textX = startX + iconSize + ImGui::GetStyle().ItemSpacing.x;
		const ImU32 textCol = overrideLabelColor
			? tintCol
			: ImGui::GetColorU32(ImGuiCol_Text);
		drawList->AddText(ImVec2(textX, ImGui::GetItemRectMin().y), textCol, name.c_str());
	}
	else
	{
		opened = ImGui::TreeNodeEx((name + m_widgetID).c_str(), flags);
		itemClicked = ImGui::IsItemClicked();
	}

    if (itemClicked && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
    {
        ClickedEvent.Invoke();

        if (ImGui::IsMouseDoubleClicked(0))
        {
            DoubleClickedEvent.Invoke();
        }
    }

	if (overrideLabelColor)
		ImGui::PopStyleColor();

	if (opened)
	{
		if (!prevOpened)
			OpenedEvent.Invoke();

		m_opened = true;

		ExecutePlugins(Plugins::EPluginExecutionContext::WIDGET); // Manually execute plugins to make plugins considering the TreeNode and no childs

		DrawWidgets();

		ImGui::TreePop();
	}
	else
	{
		if (prevOpened)
			ClosedEvent.Invoke();

		m_opened = false;

		ExecutePlugins(Plugins::EPluginExecutionContext::WIDGET); // Manually execute plugins to make plugins considering the TreeNode and no childs
	}
}
