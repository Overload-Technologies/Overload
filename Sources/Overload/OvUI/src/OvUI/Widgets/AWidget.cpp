/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/AWidget.h"

uint64_t OvUI::Widgets::AWidget::__WIDGET_ID_INCREMENT = 0;

OvUI::Widgets::AWidget::AWidget()
{
	m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void OvUI::Widgets::AWidget::LinkTo(const AWidget& p_widget)
{
	m_widgetID = p_widget.m_widgetID;
}

void OvUI::Widgets::AWidget::Destroy()
{
	m_destroyed = true;
}

bool OvUI::Widgets::AWidget::IsDestroyed() const
{
	return m_destroyed;
}

void OvUI::Widgets::AWidget::SetParent(Internal::WidgetContainer * p_parent)
{
	m_parent = p_parent;
}

bool OvUI::Widgets::AWidget::HasParent() const
{
	return m_parent;
}

OvUI::Internal::WidgetContainer * OvUI::Widgets::AWidget::GetParent()
{
	return m_parent;
}

void OvUI::Widgets::AWidget::Draw()
{
	if (enabled)
	{
		_Draw_Impl();

		if (m_autoExecutePlugins)
			ExecutePlugins(Plugins::EPluginExecutionContext::WIDGET);

		if (!lineBreak)
			ImGui::SameLine();
	}
}
