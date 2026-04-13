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
	* Provides asset-picking and asset-opening helpers shared across the editor.
	* GUIDrawer uses these internally for its asset field widgets.
	* The editor registers the concrete implementations once at startup.
	*/
	class GUIHelpers
	{
	public:
		/**
		* Represents a single item in the picker.
		* The key uniquely identifies the item for deduplication when combining lists.
		*/
		struct PickerItem
		{
			std::string key;
			std::string displayName;
			std::string tooltip;
			uint32_t iconID = 0;
			std::function<void()> onSelected;
		};

		/**
		* An ordered, deduplication-aware collection of picker items.
		* Items with the same key are silently dropped when added.
		*/
		class PickerItemList
		{
		public:
			/**
			* Add an item to the list.
			* @returns true if added, false if an item with the same key was already present.
			*/
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

		/**
		* A callback that builds a PickerItemList for a given file type.
		* Register once via SetFileItemBuilder.
		*/
		using FileItemBuilderCallback = std::function<PickerItemList(OvTools::Utils::PathParser::EFileType, std::function<void(std::string)>, bool, bool)>;

		using OpenProviderCallback = std::function<void(const std::string&)>;

		using PickerProviderCallback = std::function<void(PickerItemList, std::string)>;

		using IconProviderCallback = std::function<uint32_t(OvTools::Utils::PathParser::EFileType)>;

		/**
		* Provide the texture used when no texture resource is assigned to a texture field.
		* @param p_emptyTexture
		*/
		static void ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture);

		/**
		* Returns the empty texture set via ProvideEmptyTexture, or nullptr if none was provided.
		*/
		static OvRendering::Resources::Texture* GetEmptyTexture();

		/**
		* Register the function that builds a PickerItemList for a given file type.
		* Call once during editor startup.
		* @param p_builder
		*/
		static void SetFileItemBuilder(FileItemBuilderCallback p_builder);

		/**
		* Open the asset picker for the given file type.
		* Builds the item list via the registered file item builder, derives the window title
		* from the file type, and forwards everything to the registered picker provider.
		* Has no effect if either callback has not been registered.
		* @param p_fileType
		* @param p_onSelect
		* @param p_searchProjectFiles  Include project assets in the results
		* @param p_searchEngineFiles   Include engine assets in the results
		*/
		static void OpenAssetPicker(
			OvTools::Utils::PathParser::EFileType p_fileType,
			std::function<void(std::string)> p_onSelect,
			bool p_searchProjectFiles = true,
			bool p_searchEngineFiles = true
		);

		/**
		* Register the function that opens an asset at a given resource path.
		* Call once during editor startup.
		* @param p_provider
		*/
		static void SetOpenProvider(OpenProviderCallback p_provider);

		/**
		* Open the asset at the given resource path using the registered open provider.
		* Has no effect if no provider has been registered or the path is empty.
		* @param p_path  Resource path (e.g. ":Textures/Default.png" or "Materials/m.ovmat")
		*/
		static void Open(const std::string& p_path);

		/**
		* Register the function that returns a texture ID for a given file type.
		* Used to show asset type icons in asset fields.
		* Call once during editor startup.
		* @param p_provider
		*/
		static void SetIconProvider(IconProviderCallback p_provider);

		/**
		* Returns the icon texture ID for the given file type.
		* Returns 0 if no icon provider has been registered.
		* @param p_fileType
		*/
		static uint32_t GetIconForFileType(OvTools::Utils::PathParser::EFileType p_fileType);

		/**
		* Register the function that opens the picker window.
		* Call once during editor startup (typically in Editor::SetupUI).
		* @param p_provider
		*/
		static void SetPickerProvider(PickerProviderCallback p_provider);

		/**
		* Open the picker with the given list of items.
		* Has no effect if no provider has been registered.
		* @param p_items
		* @param p_title  Title displayed in the window's title bar
		*/
		static void OpenPicker(PickerItemList p_items, std::string p_title);
	};
}
