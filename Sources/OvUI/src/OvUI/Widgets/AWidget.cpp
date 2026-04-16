/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Widgets/AWidget.h>

#include <imgui.h>
#include <imgui_internal.h>

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

void OvUI::Widgets::AWidget::BeginDisableOverride()
{
	if (GImGui->CurrentItemFlags & ImGuiItemFlags_Disabled && neverDisabled)
	{
		ImGui::BeginDisabledOverrideReenable();
		m_isInDisableOverrideScope = true;
	}
}

void OvUI::Widgets::AWidget::EndDisableOverride()
{
	if (m_isInDisableOverrideScope)
	{
		ImGui::EndDisabledOverrideReenable();
		m_isInDisableOverrideScope = false;
	}
}

void OvUI::Widgets::AWidget::Draw()
{
	if (enabled)
	{
		if (disabled)
		{
			ImGui::BeginDisabled();
		}

		BeginDisableOverride();

		_Draw_Impl();

		if (!tooltip.empty())
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			{
				ImGui::SetTooltip("%s", tooltip.c_str());
			}
		}

		EndDisableOverride();

		if (disabled)
		{
			ImGui::EndDisabled();
		}

		if (m_autoExecutePlugins)
			ExecutePlugins(Plugins::EPluginExecutionContext::WIDGET);

		if (!lineBreak)
		{
			ImGui::SameLine();
		}
	}
}
