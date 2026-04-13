/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <string>

#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FVector4.h>
#include <OvMaths/FQuaternion.h>

#include <OvUI/Internal/WidgetContainer.h>
#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragSingleScalar.h>
#include <OvUI/Widgets/Drags/DragMultipleScalars.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/InputFields/AssetField.h>
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
	* Provides helpers to draw UI elements for common data types and asset resources.
	*/
	class GUIDrawer
	{
	public:
		static const OvUI::Types::Color TitleColor;

		static const float _MIN_FLOAT;
		static const float _MAX_FLOAT;

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
		static OvUI::Widgets::InputFields::AssetField& DrawMesh(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::InputFields::AssetField& DrawTexture(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Texture*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::InputFields::AssetField& DrawShader(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::InputFields::AssetField& DrawMaterial(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::InputFields::AssetField& DrawSound(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static OvUI::Widgets::InputFields::AssetField& DrawAsset(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);

		static OvUI::Widgets::InputFields::AssetField& DrawScene(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider);

		/**
		* Creates a full-width search bar with an optional search icon
		* @param p_root
		* @param p_searchIconTextureID
		*/
		static OvUI::Widgets::InputFields::InputText& DrawSearchBar(OvUI::Internal::WidgetContainer& p_root, uint32_t p_searchIconTextureID = 0);

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
