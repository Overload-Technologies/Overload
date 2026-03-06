/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <span>
#include <ranges>

#include "OvUI/Internal/WidgetContainer.h"

OvUI::Internal::WidgetContainer::~WidgetContainer()
{
	RemoveAllWidgets();
}

void OvUI::Internal::WidgetContainer::RemoveWidget(Widgets::AWidget& p_widget)
{
	auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&p_widget](std::pair<OvUI::Widgets::AWidget*, Internal::EMemoryMode>& p_pair)
	{ 
		return p_pair.first == &p_widget;
	});

	if (found != m_widgets.end())
	{
		if (found->second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete found->first;

		m_widgets.erase(found);
	}
}

void OvUI::Internal::WidgetContainer::RemoveAllWidgets()
{
	std::for_each(m_widgets.begin(), m_widgets.end(), [](auto& pair)
	{
		if (pair.second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete pair.first;
	});

	m_widgets.clear();
}

void OvUI::Internal::WidgetContainer::ConsiderWidget(Widgets::AWidget & p_widget, bool p_manageMemory)
{
	m_widgets.emplace_back(std::make_pair(&p_widget, p_manageMemory ? EMemoryMode::INTERNAL_MANAGMENT : EMemoryMode::EXTERNAL_MANAGMENT));
	p_widget.SetParent(this);
}

void OvUI::Internal::WidgetContainer::UnconsiderWidget(Widgets::AWidget & p_widget)
{
	auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&p_widget](std::pair<OvUI::Widgets::AWidget*, Internal::EMemoryMode>& p_pair)
	{
		return p_pair.first == &p_widget;
	});

	if (found != m_widgets.end())
	{
		p_widget.SetParent(nullptr);
		m_widgets.erase(found);
	}
}

void OvUI::Internal::WidgetContainer::CollectGarbages()
{
	m_widgets.erase(std::remove_if(m_widgets.begin(), m_widgets.end(), [](std::pair<OvUI::Widgets::AWidget*, Internal::EMemoryMode>& p_item)
	{
		bool toDestroy = p_item.first && p_item.first->IsDestroyed();

		if (toDestroy && p_item.second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete p_item.first;

		return toDestroy;
	}), m_widgets.end());
}

void OvUI::Internal::WidgetContainer::DrawWidgets()
{
	CollectGarbages();

	using WidgetType = decltype(m_widgets)::value_type::first_type;

	// We copy the widgets to draw in a separate vector to avoid issues when a widget is
	// added/destroyed during the draw process, which would invalidate the iterator.
	// This is especially useful to allow plugins to add and remove widgets during the draw process.
	std::vector<WidgetType> widgetsToDraw;
	widgetsToDraw.reserve(m_widgets.size());
	std::ranges::copy(m_widgets | std::views::keys, std::back_inserter(widgetsToDraw));

	if (m_reversedDrawOrder) [[unlikely]]
	{
		for (WidgetType widget : widgetsToDraw | std::views::reverse)
		{
			widget->Draw();
		}
	}
	else
	{
		for (WidgetType widget : widgetsToDraw)
		{
			widget->Draw();
		}
	}
}

void OvUI::Internal::WidgetContainer::ReverseDrawOrder(const bool reversed)
{
    m_reversedDrawOrder = reversed;
}

std::vector<std::pair<OvUI::Widgets::AWidget*, OvUI::Internal::EMemoryMode>>& OvUI::Internal::WidgetContainer::GetWidgets()
{
	return m_widgets;
}
