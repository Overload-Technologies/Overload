/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Panels/APanel.h>
#include <OvUI/Plugins/ContextualMenu.h>

void OvUI::Plugins::ContextualMenu::Execute(EPluginExecutionContext p_context)
{
	if (p_context == EPluginExecutionContext::PANEL ? ImGui::BeginPopupContextWindow() : ImGui::BeginPopupContextItem())
	{
		DrawWidgets();
		ImGui::EndPopup();
	}
}

void OvUI::Plugins::ContextualMenu::Close()
{
	ImGui::CloseCurrentPopup();
}
