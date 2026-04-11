/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>

#include <imgui.h>

#include <OvEditor/Panels/ItemPicker.h>

#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextClickable.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Widgets/Visual/Separator.h>

using namespace OvEditor::Panels;

namespace
{
	bool ContainsCaseInsensitive(const std::string& p_str, const std::string& p_search)
	{
		if (p_search.empty())
			return true;

		return std::search(
			p_str.begin(), p_str.end(),
			p_search.begin(), p_search.end(),
			[](char a, char b) {
				return std::tolower(static_cast<unsigned char>(a))
					== std::tolower(static_cast<unsigned char>(b));
			}
		) != p_str.end();
	}
}

ItemPicker::ItemPicker(
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings)
	: PanelWindow("", p_opened, p_windowSettings)
{
	minSize = { 250.f, 250.f };

	m_searchField = &CreateWidget<OvUI::Widgets::InputFields::InputText>("", "Search");
	m_searchField->ContentChangedEvent += [this](const std::string& p_text)
	{
		FilterList(p_text);
	};

	CreateWidget<OvUI::Widgets::Visual::Separator>();
	m_listGroup = &CreateWidget<OvUI::Widgets::Layout::Group>();
}

void ItemPicker::_Draw_Impl()
{
	if (m_usePivotAnchor)
	{
		// Anchor the bottom-left of the window to (m_anchorX, m_anchorY)
		// so the picker grows upward toward the button.
		ImGui::SetNextWindowPos({ m_anchorX, m_anchorY }, ImGuiCond_Always, { 0.f, 1.f });
		m_usePivotAnchor = false;
	}
	PanelWindow::_Draw_Impl();
	if (IsOpened() && IsFocused())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			Close();
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
		{
			const auto& items = m_items.Items();
			for (size_t i = 0; i < m_rows.size() && i < items.size(); ++i)
			{
				if (m_rows[i].second->enabled)
				{
					items[i].onSelected();
					Close();
					break;
				}
			}
		}
	}
}

void ItemPicker::Open(OvCore::Helpers::GUIDrawer::PickerItemList p_items, std::string p_title)
{
	name = std::move(p_title);
	m_items = std::move(p_items);
	m_searchField->content = "";
	m_searchField->focusOnNextDraw = true;
	Populate();
	ScrollToTop();

	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const ImVec2 buttonMin = ImGui::GetItemRectMin();
	const ImVec2 buttonMax = ImGui::GetItemRectMax();

	const float spaceBelow = display.y - buttonMax.y;
	const float spaceAbove = buttonMin.y;
	const bool placeBelow = spaceBelow >= spaceAbove;

	// Constrain the picker height so it never clips off-screen.
	maxSize.y = placeBelow ? spaceBelow : spaceAbove;

	// Try to align the left edge with the button, then clamp so the right
	// side of the minimum-width window stays within the display, and the
	// left side doesn't go negative.
	float x = buttonMin.x;
	x = std::min(x, display.x - minSize.x);
	x = std::max(0.f, x);

	// Constrain horizontal growth so the window can never overflow the right edge.
	maxSize.x = display.x - x;

	if (placeBelow)
	{
		SetPosition({ x, buttonMax.y });
	}
	else
	{
		// Use SetNextWindowPos with a bottom-left pivot so the window grows upward.
		// Don't call SetPosition() — that uses SetWindowPos (no pivot support).
		m_anchorX = x;
		m_anchorY = buttonMin.y;
		m_usePivotAnchor = true;
	}
	PanelWindow::Open();
	Focus();
}

void ItemPicker::Populate()
{
	m_listGroup->RemoveAllWidgets();
	m_rows.clear();

	for (const auto& item : m_items.Items())
	{
		auto& row = m_listGroup->CreateWidget<OvUI::Widgets::Layout::Group>();

		if (item.iconID != 0)
		{
			auto& icon = row.CreateWidget<OvUI::Widgets::Visual::Image>(item.iconID, OvMaths::FVector2{ 16.f, 16.f });
			icon.lineBreak = false;
		}

		auto& text = row.CreateWidget<OvUI::Widgets::Texts::TextClickable>(item.displayName);
		text.tooltip = item.tooltip;
		text.ClickedEvent += [this, onSelected = item.onSelected]
		{
			onSelected();
			Close();
		};

		m_rows.emplace_back(item.key, &row);
	}
}

void ItemPicker::FilterList(const std::string& p_search)
{
	for (auto& [key, row] : m_rows)
		row->enabled = ContainsCaseInsensitive(key, p_search);
}
