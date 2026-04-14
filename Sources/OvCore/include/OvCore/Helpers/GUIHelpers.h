/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include <OvTools/Utils/PathParser.h>

namespace OvRendering::Resources
{
	class Texture;
}

namespace OvCore::Helpers
{
	/**
	* Asset picking and opening helpers shared across the editor.
	* Implements a service locator pattern for UI operations.
	*/
	class GUIHelpers
	{
	public:
		struct PickerItem
		{
			std::string key;
			std::string displayName;
			std::string tooltip;
			uint32_t iconID = 0;
			std::function<void()> onSelected;
		};

		class PickerItemList
		{
		public:
			bool Add(PickerItem p_item)
			{
				if (!m_keys.insert(p_item.key).second)
					return false;
				m_items.push_back(std::move(p_item));
				return true;
			}

			const std::vector<PickerItem>& Items() const { return m_items; }
			bool empty() const { return m_items.empty(); }
			size_t size() const { return m_items.size(); }

		private:
			std::vector<PickerItem> m_items;
			std::unordered_set<std::string> m_keys;
		};

		using FileItemBuilderCallback = std::function<PickerItemList(OvTools::Utils::PathParser::EFileType, std::function<void(std::string)>, bool, bool)>;
		using OpenProviderCallback = std::function<void(const std::string&)>;
		using PickerProviderCallback = std::function<void(PickerItemList, std::string)>;
		using IconProviderCallback = std::function<uint32_t(OvTools::Utils::PathParser::EFileType)>;
		using ActorSelectionProviderCallback = std::function<void(uint64_t)>;

		static void ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture);
		static OvRendering::Resources::Texture* GetEmptyTexture();

		static void SetFileItemBuilder(FileItemBuilderCallback p_builder);

		static void OpenAssetPicker(
			OvTools::Utils::PathParser::EFileType p_fileType,
			std::function<void(std::string)> p_onSelect,
			bool p_searchProjectFiles = true,
			bool p_searchEngineFiles = true
		);

		static void SetOpenProvider(OpenProviderCallback p_provider);
		static void Open(const std::string& p_path);

		static void SetIconProvider(IconProviderCallback p_provider);
		static uint32_t GetIconForFileType(OvTools::Utils::PathParser::EFileType p_fileType);

		static void SetPickerProvider(PickerProviderCallback p_provider);
		static void OpenPicker(PickerItemList p_items, std::string p_title);

		static void SetActorIconID(uint32_t p_id);
		static uint32_t GetActorIconID();

		static void SetActorSelectionProvider(ActorSelectionProviderCallback p_provider);
		static void SelectActor(uint64_t p_guid);
	};
}
