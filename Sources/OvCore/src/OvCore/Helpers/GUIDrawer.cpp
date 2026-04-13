/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <memory>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Widgets/Texts/TextColored.h>
#include <OvUI/Widgets/Drags/DragSingleScalar.h>
#include <OvUI/Widgets/Drags/DragMultipleScalars.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/InputFields/AssetField.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Buttons/ButtonSmall.h>
#include <OvUI/Plugins/DDTarget.h>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/SoundManager.h>

#include "OvCore/Helpers/GUIDrawer.h"

const OvUI::Types::Color OvCore::Helpers::GUIDrawer::TitleColor = { 0.85f, 0.65f, 0.0f };
const float OvCore::Helpers::GUIDrawer::_MIN_FLOAT = -999999999.f;
const float OvCore::Helpers::GUIDrawer::_MAX_FLOAT = +999999999.f;

namespace
{
	OvRendering::Resources::Texture* __EMPTY_TEXTURE = nullptr;
	OvCore::Helpers::GUIDrawer::FileItemBuilderCallback __FILE_ITEM_BUILDER;
	OvCore::Helpers::GUIDrawer::PickerProviderCallback __PICKER_PROVIDER;
	OvCore::Helpers::GUIDrawer::IconProviderCallback __ICON_PROVIDER;
	OvCore::Helpers::GUIDrawer::OpenProviderCallback __OPEN_PROVIDER;

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
		default:                  return "Pick Asset";
		}
	}
}

void OvCore::Helpers::GUIDrawer::ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture)
{
	__EMPTY_TEXTURE = &p_emptyTexture;
}

void OvCore::Helpers::GUIDrawer::SetFileItemBuilder(FileItemBuilderCallback p_builder)
{
	__FILE_ITEM_BUILDER = std::move(p_builder);
}

void OvCore::Helpers::GUIDrawer::OpenAssetPicker(
	OvTools::Utils::PathParser::EFileType p_fileType,
	std::function<void(std::string)> p_onSelect,
	bool p_searchProjectFiles,
	bool p_searchEngineFiles
)
{
	if (!__FILE_ITEM_BUILDER || !__PICKER_PROVIDER)
		return;

	// Keep a copy so we can attach it to the None item before moving.
	auto onSelectCopy = p_onSelect;
	auto assetItems = __FILE_ITEM_BUILDER(p_fileType, std::move(p_onSelect), p_searchProjectFiles, p_searchEngineFiles);

	// Build the final list with "None" at the top.
	PickerItemList items;
	items.Add({ "__none__", "None", "Clear the current selection", 0, [onSelectCopy] { onSelectCopy(""); } });
	for (const auto& item : assetItems.Items())
		items.Add(item);

	__PICKER_PROVIDER(std::move(items), TitleFromFileType(p_fileType));
}

void OvCore::Helpers::GUIDrawer::SetPickerProvider(PickerProviderCallback p_provider)
{
	__PICKER_PROVIDER = std::move(p_provider);
}

void OvCore::Helpers::GUIDrawer::SetIconProvider(IconProviderCallback p_provider)
{
	__ICON_PROVIDER = std::move(p_provider);
}

void OvCore::Helpers::GUIDrawer::SetOpenProvider(OpenProviderCallback p_provider)
{
	__OPEN_PROVIDER = std::move(p_provider);
}

void OvCore::Helpers::GUIDrawer::Open(const std::string& p_path)
{
	if (__OPEN_PROVIDER && !p_path.empty())
		__OPEN_PROVIDER(p_path);
}

void OvCore::Helpers::GUIDrawer::OpenPicker(PickerItemList p_items, std::string p_title)
{
	if (__PICKER_PROVIDER)
		__PICKER_PROVIDER(std::move(p_items), std::move(p_title));
}

void OvCore::Helpers::GUIDrawer::CreateTitle(OvUI::Internal::WidgetContainer& p_root, const std::string & p_name)
{
	p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>(p_name, TitleColor);
}

void OvCore::Helpers::GUIDrawer::DrawBoolean(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, bool & p_data)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Selection::CheckBox>();
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<bool>>();
	dispatcher.RegisterReference(reinterpret_cast<bool&>(p_data));
}

void OvCore::Helpers::GUIDrawer::DrawVec2(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvMaths::FVector2 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 2>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 2>&>(p_data));
}

void OvCore::Helpers::GUIDrawer::DrawVec3(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvMaths::FVector3 & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 3>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(p_data));
}

void OvCore::Helpers::GUIDrawer::DrawVec4(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvMaths::FVector4& p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 4>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
}

void OvCore::Helpers::GUIDrawer::DrawQuat(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvMaths::FQuaternion & p_data, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 4>>>();
	dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
}

void OvCore::Helpers::GUIDrawer::DrawString(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::string & p_data)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
	dispatcher.RegisterReference(p_data);
}

void OvCore::Helpers::GUIDrawer::DrawColor(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvUI::Types::Color & p_color, bool p_hasAlpha)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Selection::ColorEdit>(p_hasAlpha);
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<OvUI::Types::Color>>();
	dispatcher.RegisterReference(p_color);
}

namespace
{
	void AddSelectButton(
		OvTools::Eventing::Event<>& p_clickedEvent,
		OvTools::Utils::PathParser::EFileType p_fileType,
		std::function<void(const std::string&)> p_onSelect)
	{
		auto token = std::make_shared<bool>(true);
		p_clickedEvent += [p_fileType, p_onSelect = std::move(p_onSelect), token = std::move(token)]
		{
			std::weak_ptr<bool> weak = token;
			OvCore::Helpers::GUIDrawer::OpenAssetPicker(p_fileType, [p_onSelect, weak](const std::string& p_path)
			{
				if (!weak.expired()) p_onSelect(p_path);
			}, true, true);
		};
	}

	template<typename TResource, typename TResourceManager>
	OvUI::Widgets::InputFields::AssetField& DrawResourceWidget(
		OvUI::Internal::WidgetContainer& p_root,
		const std::string& p_name,
		TResource*& p_data,
		OvTools::Utils::PathParser::EFileType p_fileType,
		OvTools::Eventing::Event<>* p_updateNotifier)
	{
		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, p_name);

		const std::string displayedText = p_data ? p_data->path : std::string{};
		auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(displayedText);
		if (__ICON_PROVIDER)
			widget.iconTextureID = __ICON_PROVIDER(p_fileType);

		widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
			[&widget, &p_data, p_updateNotifier, p_fileType](auto p_receivedData)
		{
			if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == p_fileType)
			{
				if (auto resource = OVSERVICE(TResourceManager).GetResource(p_receivedData.first); resource)
				{
					p_data = resource;
					widget.content = p_receivedData.first;
					if (p_updateNotifier)
						p_updateNotifier->Invoke();
				}
			}
		};

		AddSelectButton(widget.ClickedEvent, p_fileType, [&widget, &p_data, p_updateNotifier](const std::string& p_path)
		{
			if (p_path.empty())
			{
				p_data = nullptr;
				widget.content.clear();
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
				return;
			}
			if (auto resource = OVSERVICE(TResourceManager).GetResource(p_path); resource)
			{
				p_data = resource;
				widget.content = p_path;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		});

		if (__OPEN_PROVIDER)
			widget.DoubleClickedEvent += [&widget] { __OPEN_PROVIDER(widget.content); };

		return widget;
	}
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawMesh(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvRendering::Resources::Model, OvCore::ResourceManagement::ModelManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::MODEL, p_updateNotifier);
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawTexture(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Texture*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	auto getPreviewID = [&]() -> uint32_t
	{
		if (p_data) return p_data->GetTexture().GetID();
		return __EMPTY_TEXTURE ? __EMPTY_TEXTURE->GetTexture().GetID() : 0;
	};

	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(p_data ? p_data->path : std::string{});
	if (__ICON_PROVIDER)
		widget.iconTextureID = __ICON_PROVIDER(OvTools::Utils::PathParser::EFileType::TEXTURE);
	widget.previewTextureID = getPreviewID();

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
		[&widget, &p_data, p_updateNotifier, getPreviewID](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
		{
			if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.content = p_receivedData.first;
				widget.previewTextureID = resource->GetTexture().GetID();
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};

	AddSelectButton(widget.ClickedEvent, OvTools::Utils::PathParser::EFileType::TEXTURE, [&widget, &p_data, p_updateNotifier, getPreviewID](const std::string& p_path)
	{
		if (p_path.empty())
		{
			p_data = nullptr;
			widget.content.clear();
			widget.previewTextureID = getPreviewID();
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
			return;
		}
		if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_path); resource)
		{
			p_data = resource;
			widget.content = p_path;
			widget.previewTextureID = resource->GetTexture().GetID();
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
		}
	});

	if (__OPEN_PROVIDER)
		widget.DoubleClickedEvent += [&widget] { __OPEN_PROVIDER(widget.content); };

	return widget;
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawShader(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvRendering::Resources::Shader, OvCore::ResourceManagement::ShaderManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::SHADER, p_updateNotifier);
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawMaterial(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvCore::Resources::Material, OvCore::ResourceManagement::MaterialManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::MATERIAL, p_updateNotifier);
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawSound(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvAudio::Resources::Sound, OvCore::ResourceManagement::SoundManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::SOUND, p_updateNotifier);
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawAsset(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	const std::string displayedText = p_data;
	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(displayedText);

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		p_data = p_receivedData.first;
		widget.content = p_receivedData.first;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};

	AddSelectButton(widget.ClickedEvent, OvTools::Utils::PathParser::EFileType::UNKNOWN, [&widget, &p_data, p_updateNotifier](const std::string& p_path)
	{
		p_data = p_path;
		widget.content = p_path;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	});

	return widget;
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawScene(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider)
{
	CreateTitle(p_root, p_name);

	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(p_gatherer());
	if (__ICON_PROVIDER)
		widget.iconTextureID = __ICON_PROVIDER(OvTools::Utils::PathParser::EFileType::SCENE);

	widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer(p_gatherer);

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
		[&widget, p_provider](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SCENE)
		{
			widget.content = p_receivedData.first;
			p_provider(p_receivedData.first);
		}
	};

	auto token = std::make_shared<bool>(true);
	widget.ClickedEvent += [&widget, p_provider, token]()
	{
		std::weak_ptr<bool> weak = token;
		OpenAssetPicker(OvTools::Utils::PathParser::EFileType::SCENE, [&widget, p_provider, weak](const std::string& p_path)
		{
			if (!weak.expired())
			{
				widget.content = p_path;
				p_provider(p_path);
			}
		}, true, false);
	};

	return widget;
}

void OvCore::Helpers::GUIDrawer::DrawBoolean(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Selection::CheckBox>();
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<bool>>();

	dispatcher.RegisterGatherer([p_gatherer]()
	{
		bool value = p_gatherer();
		return reinterpret_cast<bool&>(value);
	});

	dispatcher.RegisterProvider([p_provider](bool p_value)
	{
		p_provider(reinterpret_cast<bool&>(p_value));
	});
}

void OvCore::Helpers::GUIDrawer::DrawVec2(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvMaths::FVector2(void)> p_gatherer, std::function<void(OvMaths::FVector2)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 2>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 2>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		OvMaths::FVector2 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 2>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 2> p_value)
	{
		p_provider(reinterpret_cast<OvMaths::FVector2&>(p_value));
	});
}

void OvCore::Helpers::GUIDrawer::DrawVec3(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvMaths::FVector3(void)> p_gatherer, std::function<void(OvMaths::FVector3)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 3>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 3>>>();

	dispatcher.RegisterGatherer([p_gatherer]()
	{
		OvMaths::FVector3 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 3>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 3> p_value)
	{
		p_provider(reinterpret_cast<OvMaths::FVector3&>(p_value));
	});
}

void OvCore::Helpers::GUIDrawer::DrawVec4(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvMaths::FVector4(void)> p_gatherer, std::function<void(OvMaths::FVector4)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		OvMaths::FVector4 value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([p_provider](std::array<float, 4> p_value)
	{
		p_provider(reinterpret_cast<OvMaths::FVector4&>(p_value));
	});
}

void OvCore::Helpers::GUIDrawer::DrawQuat(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvMaths::FQuaternion(void)> p_gatherer, std::function<void(OvMaths::FQuaternion)> p_provider, float p_step, float p_min, float p_max)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 4>>(GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GetFormat<float>());
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 4>>>();
	
	dispatcher.RegisterGatherer([p_gatherer]()
	{
		OvMaths::FQuaternion value = p_gatherer();
		return reinterpret_cast<const std::array<float, 4>&>(value);
	});

	dispatcher.RegisterProvider([&dispatcher, p_provider](std::array<float, 4> p_value)
	{
		p_provider(OvMaths::FQuaternion::Normalize(reinterpret_cast<OvMaths::FQuaternion&>(p_value)));
	});
}

void OvCore::Helpers::GUIDrawer::DrawDDString(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name,
	std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider,
	const std::string& p_identifier)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);

	auto& ddTarget = widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>(p_identifier);
	ddTarget.DataReceivedEvent += [&widget, &dispatcher](std::pair<std::string, OvUI::Widgets::Layout::Group*> p_data)
	{
		widget.content = p_data.first;
		dispatcher.NotifyChange();
	};
}

void OvCore::Helpers::GUIDrawer::DrawString(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

void OvCore::Helpers::GUIDrawer::DrawColor(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvUI::Types::Color(void)> p_gatherer, std::function<void(OvUI::Types::Color)> p_provider, bool p_hasAlpha)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Selection::ColorEdit>(p_hasAlpha);
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<OvUI::Types::Color>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}
