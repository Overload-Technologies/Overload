/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>

#include <imgui.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Panels/AssetPicker.h>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Texts/TextClickable.h>
#include <OvUI/Widgets/Visual/Image.h>
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

void AssetPicker::Open(PathParser::EFileType p_fileType, std::function<void(std::string)> p_callback, bool p_searchProjectFiles, bool p_searchEngineFiles)
{
	m_mode = EMode::FileBased;
	m_fileType = p_fileType;
	m_searchProjectFiles = p_searchProjectFiles;
	m_searchEngineFiles = p_searchEngineFiles;
	m_callback = std::move(p_callback);
	m_prebuiltItems.clear();
	OpenInternal();
}

void AssetPicker::Open(std::vector<OvCore::Helpers::GUIDrawer::PickerItem> p_items)
{
	m_mode = EMode::PreBuilt;
	m_prebuiltItems = std::move(p_items);
	OpenInternal();
}

void AssetPicker::OpenInternal()
{
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

void AssetPicker::Populate()
{
	m_assetListGroup->RemoveAllWidgets();
	m_items.clear();

	const auto addItem = [&](
		const std::string& p_searchKey,
		const std::string& p_displayName,
		const std::string& p_tooltip,
		uint32_t p_iconID,
		std::function<void()> p_onSelected)
	{
		auto& row = m_assetListGroup->CreateWidget<OvUI::Widgets::Layout::Group>();

		if (p_iconID != 0)
		{
			auto& icon = row.CreateWidget<OvUI::Widgets::Visual::Image>(p_iconID, OvMaths::FVector2{ 16.f, 16.f });
			icon.lineBreak = false;
		}

		auto& text = row.CreateWidget<OvUI::Widgets::Texts::TextClickable>(p_displayName);
		text.tooltip = p_tooltip;
		text.ClickedEvent += [this, onSelected = std::move(p_onSelected)]
		{
			onSelected();
			Close();
		};

		m_items.emplace_back(p_searchKey, &row);
	};

	if (m_mode == EMode::PreBuilt)
	{
		for (auto& item : m_prebuiltItems)
		{
			addItem(item.displayName, item.displayName, item.tooltip, item.iconID, item.onSelected);
		}
	}
	else
	{
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
				const uint32_t iconID = EDITOR_CONTEXT(editorResources)->GetFileIcon(path)->GetTexture().GetID();

				addItem(resourcePath, filename, resourcePath, iconID, [this, resourcePath]
				{
					if (m_callback)
						m_callback(resourcePath);
				});
			}
		};

		if (m_searchProjectFiles)
			collectFromDirectory(EDITOR_CONTEXT(projectAssetsPath), false);

		if (m_searchEngineFiles)
			collectFromDirectory(EDITOR_CONTEXT(engineAssetsPath), true);
	}
}

void AssetPicker::FilterList(const std::string& p_search)
{
	for (auto& [key, row] : m_items)
		row->enabled = ContainsCaseInsensitive(key, p_search);
}
