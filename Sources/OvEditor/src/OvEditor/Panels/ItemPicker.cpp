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

void ItemPicker::Open(OvCore::Helpers::GUIDrawer::PickerItemList p_items, std::string p_title)
{
	name = std::move(p_title);
	m_items = std::move(p_items);
	m_searchField->content = "";
	Populate();
	ScrollToTop();

	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const float winW = minSize.x;
	const float winH = minSize.y;

	const ImVec2 buttonMin = ImGui::GetItemRectMin();
	const ImVec2 buttonMax = ImGui::GetItemRectMax();

	float x = buttonMin.x;
	float y = buttonMax.y;

	if (y + winH > display.y)
		y = buttonMin.y - winH;

	if (x + winW > display.x)
		x = buttonMax.x - winW;

	x = std::max(0.f, x);
	y = std::max(0.f, y);

	SetPosition({ x, y });
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
