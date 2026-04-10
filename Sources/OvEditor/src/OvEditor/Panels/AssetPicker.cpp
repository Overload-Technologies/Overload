/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>

#include <imgui.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/AssetPicker.h>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextClickable.h>
#include <OvUI/Widgets/Visual/Separator.h>

using namespace OvEditor::Panels;
using namespace OvTools::Utils;

namespace
{
	bool ContainsCaseInsensitive(const std::string& p_str, const std::string& p_search)
	{
		if (p_search.empty())
			return true;

		return std::search(
			p_str.begin(), p_str.end(),
			p_search.begin(), p_search.end(),
			[](char a, char b) { return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); }
		) != p_str.end();
	}
}

AssetPicker::AssetPicker(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings)
	: PanelWindow(p_title, p_opened, p_windowSettings)
{
	minSize = { 250.f, 250.f };

	m_searchField = &CreateWidget<OvUI::Widgets::InputFields::InputText>("", "Search");
	m_searchField->ContentChangedEvent += [this](const std::string& p_text)
	{
		FilterList(p_text);
	};

	CreateWidget<OvUI::Widgets::Visual::Separator>();
	m_assetListGroup = &CreateWidget<OvUI::Widgets::Layout::Group>();
}

void AssetPicker::Open(PathParser::EFileType p_fileType, std::function<void(std::string)> p_callback)
{
	m_fileType = p_fileType;
	m_callback = std::move(p_callback);
	m_searchField->content = "";
	Populate();
	ScrollToTop();

	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const float winW = minSize.x;
	const float winH = minSize.y;

	const ImVec2 buttonMin = ImGui::GetItemRectMin();
	const ImVec2 buttonMax = ImGui::GetItemRectMax();

	// Default: top-left corner of the window aligned with the bottom-left of the button
	float x = buttonMin.x;
	float y = buttonMax.y;

	// Not enough room below → open above the button instead
	if (y + winH > display.y)
		y = buttonMin.y - winH;

	// Not enough room to the right → right-align to button's right edge
	if (x + winW > display.x)
		x = buttonMax.x - winW;

	// Keep fully on-screen
	x = std::max(0.f, x);
	y = std::max(0.f, y);

	SetPosition({ x, y });

	PanelWindow::Open();
	Focus();
}

void AssetPicker::Populate()
{
	m_assetListGroup->RemoveAllWidgets();
	m_items.clear();

	const auto collectFromDirectory = [&](const std::filesystem::path& p_directory, bool p_isEngine)
	{
		if (!std::filesystem::exists(p_directory))
			return;

		std::error_code ec;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(
			p_directory, std::filesystem::directory_options::skip_permission_denied, ec))
		{
			if (!entry.is_regular_file())
				continue;

			const std::string path = entry.path().string();
			const PathParser::EFileType fileType = PathParser::GetFileType(path);

			if (fileType == PathParser::EFileType::UNKNOWN)
				continue;

			if (m_fileType != PathParser::EFileType::UNKNOWN && fileType != m_fileType)
				continue;

			const std::string resourcePath = EDITOR_EXEC(GetResourcePath(path, p_isEngine));
			const std::string filename = PathParser::GetElementName(resourcePath);

			auto& item = m_assetListGroup->CreateWidget<OvUI::Widgets::Texts::TextClickable>(filename);
			item.tooltip = resourcePath;
			item.ClickedEvent += [this, resourcePath]
			{
				if (m_callback)
					m_callback(resourcePath);
				Close();
			};

			m_items.emplace_back(resourcePath, &item);
		}
	};

	collectFromDirectory(EDITOR_CONTEXT(engineAssetsPath), true);
	collectFromDirectory(EDITOR_CONTEXT(projectAssetsPath), false);
}

void AssetPicker::FilterList(const std::string& p_search)
{
	for (auto& [path, widget] : m_items)
		widget->enabled = ContainsCaseInsensitive(path, p_search);
}
