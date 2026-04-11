/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FVector4.h>
#include <OvMaths/FQuaternion.h>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Internal/WidgetContainer.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragSingleScalar.h>
#include <OvUI/Widgets/Drags/DragMultipleScalars.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Types/Color.h>

namespace OvCore::Resources
{
	class Material;
}

namespace OvAudio::Resources
{
	class Sound;
}

namespace OvRendering::Resources
{
	class Model;
	class Shader;
	class Texture;
}

namespace OvCore::Helpers
{
	/**
	* Provide some helpers to draw UI elements
	*/
	class GUIDrawer
	{
	public:
		/**
		* Represents a single item in the picker.
		* The key uniquely identifies the item for deduplication when combining lists.
		*/
		struct PickerItem
		{
			std::string key;         // unique identifier used for deduplication
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
		* Called internally by OpenAssetPicker — register once via SetFileItemBuilder.
		*/
		using FileItemBuilderCallback = std::function<PickerItemList(OvTools::Utils::PathParser::EFileType, std::function<void(std::string)>, bool, bool)>;

		using PickerProviderCallback = std::function<void(PickerItemList, std::string)>;

		static const OvUI::Types::Color TitleColor;

		static const float _MIN_FLOAT;
		static const float _MAX_FLOAT;

		/**
		* Defines the texture to use when there is no texture in a texture resource field
		* @param p_emptyTexture
		*/
		static void ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture);

		/**
		* Register the function that builds a PickerItemList for a given file type.
		* This is called internally by OpenAssetPicker.
		* Call this once during editor startup.
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
		* Register the function that opens the picker window.
		* Call this once during editor startup (typically in Editor::SetupUI).
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

		/**
		* Draw a title with the title color
		* @param p_root
		* @param p_name
		*/
		static void CreateTitle(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name);
	
		template <typename T>
		static void DrawScalar(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, T& p_data, float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
		static void DrawBoolean(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, bool& p_data);
		static void DrawVec2(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector2& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec3(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector3& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec4(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector4& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawQuat(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FQuaternion& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawString(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data);
		static void DrawColor(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvUI::Types::Color& p_color, bool p_hasAlpha = false);
		static OvUI::Widgets::Texts::Text& DrawMesh(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::Visual::Image& DrawTexture(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Texture*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::Texts::Text& DrawShader(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::Texts::Text& DrawMaterial(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::Texts::Text& DrawSound(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::Texts::Text& DrawAsset(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);

		template <typename T>
		static void DrawScalar(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<T(void)> p_gatherer, std::function<void(T)> p_provider, float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
		static void DrawBoolean(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider);
		static void DrawVec2(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<OvMaths::FVector2(void)> p_gatherer, std::function<void(OvMaths::FVector2)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec3(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<OvMaths::FVector3(void)> p_gatherer, std::function<void(OvMaths::FVector3)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec4(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<OvMaths::FVector4(void)> p_gatherer, std::function<void(OvMaths::FVector4)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawQuat(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<OvMaths::FQuaternion(void)> p_gatherer, std::function<void(OvMaths::FQuaternion)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawDDString(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider, const std::string& p_identifier);
		static void DrawString(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider);
		static void DrawColor(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<OvUI::Types::Color(void)> p_gatherer, std::function<void(OvUI::Types::Color)> p_provider, bool p_hasAlpha = false);

		template <typename T>
		static ImGuiDataType_ GetDataType();

		template <typename T>
		static std::string GetFormat();
	};
}

#include "OvCore/Helpers/GUIDrawer.inl"
