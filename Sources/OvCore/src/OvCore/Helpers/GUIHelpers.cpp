/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#include <OvRendering/Resources/Texture.h>

#include "OvCore/Helpers/GUIHelpers.h"

namespace
{
	OvRendering::Resources::Texture* __EMPTY_TEXTURE = nullptr;
	OvCore::Helpers::GUIHelpers::FileItemBuilderCallback __FILE_ITEM_BUILDER;
	OvCore::Helpers::GUIHelpers::PickerProviderCallback __PICKER_PROVIDER;
	OvCore::Helpers::GUIHelpers::IconProviderCallback __ICON_PROVIDER;
	OvCore::Helpers::GUIHelpers::OpenProviderCallback __OPEN_PROVIDER;

	std::string TitleFromFileType(OvTools::Utils::PathParser::EFileType p_type)
	{
		using EFileType = OvTools::Utils::PathParser::EFileType;
		switch (p_type)
		{
		case EFileType::MODEL:    return "Pick Model";
		case EFileType::TEXTURE:  return "Pick Texture";
		case EFileType::SHADER:   return "Pick Shader";
		case EFileType::MATERIAL: return "Pick Material";
		case EFileType::SOUND:    return "Pick Sound";
		case EFileType::SCRIPT:   return "Pick Script";
		case EFileType::SCENE:    return "Pick Scene";
		default:                  return "Pick Asset";
		}
	}
}

void OvCore::Helpers::GUIHelpers::ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture)
{
	__EMPTY_TEXTURE = &p_emptyTexture;
}

OvRendering::Resources::Texture* OvCore::Helpers::GUIHelpers::GetEmptyTexture()
{
	return __EMPTY_TEXTURE;
}

void OvCore::Helpers::GUIHelpers::SetFileItemBuilder(FileItemBuilderCallback p_builder)
{
	__FILE_ITEM_BUILDER = std::move(p_builder);
}

void OvCore::Helpers::GUIHelpers::OpenAssetPicker(
	OvTools::Utils::PathParser::EFileType p_fileType,
	std::function<void(std::string)> p_onSelect,
	bool p_searchProjectFiles,
	bool p_searchEngineFiles
)
{
	if (!__FILE_ITEM_BUILDER || !__PICKER_PROVIDER)
		return;

	auto assetItems = __FILE_ITEM_BUILDER(p_fileType, p_onSelect, p_searchProjectFiles, p_searchEngineFiles);

	PickerItemList items;
	items.Add({ "__none__", "None", "Clear the current selection", 0, [p_onSelect] { p_onSelect(""); } });
	for (const auto& item : assetItems.Items())
		items.Add(item);

	__PICKER_PROVIDER(std::move(items), TitleFromFileType(p_fileType));
}

void OvCore::Helpers::GUIHelpers::SetOpenProvider(OpenProviderCallback p_provider)
{
	__OPEN_PROVIDER = std::move(p_provider);
}

void OvCore::Helpers::GUIHelpers::Open(const std::string& p_path)
{
	if (__OPEN_PROVIDER && !p_path.empty())
		__OPEN_PROVIDER(p_path);
}

void OvCore::Helpers::GUIHelpers::SetIconProvider(IconProviderCallback p_provider)
{
	__ICON_PROVIDER = std::move(p_provider);
}

uint32_t OvCore::Helpers::GUIHelpers::GetIconForFileType(OvTools::Utils::PathParser::EFileType p_fileType)
{
	return __ICON_PROVIDER ? __ICON_PROVIDER(p_fileType) : 0;
}

void OvCore::Helpers::GUIHelpers::SetPickerProvider(PickerProviderCallback p_provider)
{
	__PICKER_PROVIDER = std::move(p_provider);
}

void OvCore::Helpers::GUIHelpers::OpenPicker(PickerItemList p_items, std::string p_title)
{
	if (__PICKER_PROVIDER)
		__PICKER_PROVIDER(std::move(p_items), std::move(p_title));
}
