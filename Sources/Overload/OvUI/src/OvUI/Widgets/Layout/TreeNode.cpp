/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Layout/TreeNode.h"

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
		ImGui::SetNextTreeNodeOpen(true);
		m_shouldOpen = false;
	}
	else if (m_shouldClose)
	{
		ImGui::SetNextTreeNodeOpen(false);
		m_shouldClose = false;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (m_arrowClickToOpen) flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	if (selected)			flags |= ImGuiTreeNodeFlags_Selected;
	if (leaf)				flags |= ImGuiTreeNodeFlags_Leaf;

	bool opened = ImGui::TreeNodeEx((name + m_widgetID).c_str(), flags);

    if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
    {
        ClickedEvent.Invoke();

        if (ImGui::IsMouseDoubleClicked(0))
        {
            DoubleClickedEvent.Invoke();
        }
    }

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
