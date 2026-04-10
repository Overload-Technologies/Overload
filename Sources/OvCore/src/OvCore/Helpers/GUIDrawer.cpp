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
#include "OvUI/Widgets/Buttons/AButton.h"

const OvUI::Types::Color OvCore::Helpers::GUIDrawer::TitleColor = { 0.85f, 0.65f, 0.0f };
const OvUI::Types::Color OvCore::Helpers::GUIDrawer::ClearButtonColor = { 0.5f, 0.0f, 0.0f };
const float OvCore::Helpers::GUIDrawer::_MIN_FLOAT = -999999999.f;
const float OvCore::Helpers::GUIDrawer::_MAX_FLOAT = +999999999.f;

namespace
{
	OvRendering::Resources::Texture* __EMPTY_TEXTURE = nullptr;
	OvCore::Helpers::GUIDrawer::FileItemBuilderCallback __FILE_ITEM_BUILDER;
	OvCore::Helpers::GUIDrawer::PickerProviderCallback __PICKER_PROVIDER;

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

	auto items = __FILE_ITEM_BUILDER(p_fileType, std::move(p_onSelect), p_searchProjectFiles, p_searchEngineFiles);
	__PICKER_PROVIDER(std::move(items), TitleFromFileType(p_fileType));
}

void OvCore::Helpers::GUIDrawer::SetPickerProvider(PickerProviderCallback p_provider)
{
	__PICKER_PROVIDER = std::move(p_provider);
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
		OvUI::Widgets::Buttons::AButton& p_button,
		OvTools::Utils::PathParser::EFileType p_fileType,
		std::function<void(const std::string&)> p_onSelect)
	{
		auto token = std::make_shared<bool>(true);
		p_button.ClickedEvent += [p_fileType, p_onSelect = std::move(p_onSelect), token = std::move(token)]
		{
			std::weak_ptr<bool> weak = token;
			OvCore::Helpers::GUIDrawer::OpenAssetPicker(p_fileType, [p_onSelect, weak](const std::string& p_path)
			{
				if (!weak.expired()) p_onSelect(p_path);
			}, true, true);
		};
	}

	template<typename TResource, typename TResourceManager>
	OvUI::Widgets::Texts::Text& DrawResourceWidget(
		OvUI::Internal::WidgetContainer& p_root,
		const std::string& p_name,
		TResource*& p_data,
		OvTools::Utils::PathParser::EFileType p_fileType,
		OvTools::Eventing::Event<>* p_updateNotifier)
	{
		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, p_name);

		std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
		auto& rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
		rightSide.horizontal = true;
		rightSide.stretchWidget = 0;

		auto& widget = rightSide.CreateWidget<OvUI::Widgets::Texts::Text>(displayedText);

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

		widget.lineBreak = false;

		auto& selectButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("...");
		selectButton.lineBreak = false;
		AddSelectButton(selectButton, p_fileType, [&widget, &p_data, p_updateNotifier](const std::string& p_path)
		{
			if (auto resource = OVSERVICE(TResourceManager).GetResource(p_path); resource)
			{
				p_data = resource;
				widget.content = p_path;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		});

		auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
		resetButton.idleBackgroundColor = OvCore::Helpers::GUIDrawer::ClearButtonColor;
		resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
		{
			p_data = nullptr;
			widget.content = "Empty";
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
		};

		return widget;
	}
}

OvUI::Widgets::Texts::Text& OvCore::Helpers::GUIDrawer::DrawMesh(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvRendering::Resources::Model, OvCore::ResourceManagement::ModelManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::MODEL, p_updateNotifier);
}

OvUI::Widgets::Visual::Image& OvCore::Helpers::GUIDrawer::DrawTexture(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Texture *& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
	auto& rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
	rightSide.horizontal = true;
	rightSide.stretchWidget = 0;

	auto& widget = rightSide.CreateWidget<OvUI::Widgets::Visual::Image>(p_data ? p_data->GetTexture().GetID() : (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->GetTexture().GetID() : 0), OvMaths::FVector2{75, 75});

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
		{
			if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widget.textureID.id = resource->GetTexture().GetID();
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};

	widget.lineBreak = false;

	auto& selectButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("...");
	selectButton.lineBreak = false;
	AddSelectButton(selectButton, OvTools::Utils::PathParser::EFileType::TEXTURE, [&widget, &p_data, p_updateNotifier](const std::string& p_path)
	{
		if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_path); resource)
		{
			p_data = resource;
			widget.textureID.id = resource->GetTexture().GetID();
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
		}
	});

	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
	resetButton.idleBackgroundColor = ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	{
		p_data = nullptr;
		widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->GetTexture().GetID() : 0);
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};

	return widget;
}

OvUI::Widgets::Texts::Text& OvCore::Helpers::GUIDrawer::DrawShader(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvRendering::Resources::Shader, OvCore::ResourceManagement::ShaderManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::SHADER, p_updateNotifier);
}

OvUI::Widgets::Texts::Text& OvCore::Helpers::GUIDrawer::DrawMaterial(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvCore::Resources::Material, OvCore::ResourceManagement::MaterialManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::MATERIAL, p_updateNotifier);
}

OvUI::Widgets::Texts::Text& OvCore::Helpers::GUIDrawer::DrawSound(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	return DrawResourceWidget<OvAudio::Resources::Sound, OvCore::ResourceManagement::SoundManager>(p_root, p_name, p_data, OvTools::Utils::PathParser::EFileType::SOUND, p_updateNotifier);
}

OvUI::Widgets::Texts::Text& OvCore::Helpers::GUIDrawer::DrawAsset(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier)
{
	CreateTitle(p_root, p_name);

	const std::string displayedText = (p_data.empty() ? std::string("Empty") : p_data);
	auto& rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
	rightSide.horizontal = true;
	rightSide.stretchWidget = 0;

	auto& widget = rightSide.CreateWidget<OvUI::Widgets::Texts::Text>(displayedText);

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
	{
		p_data = p_receivedData.first;
		widget.content = p_receivedData.first;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};

	widget.lineBreak = false;

	auto& selectButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("...");
	selectButton.lineBreak = false;
	AddSelectButton(selectButton, OvTools::Utils::PathParser::EFileType::UNKNOWN, [&widget, &p_data, p_updateNotifier](const std::string& p_path)
	{
		p_data = p_path;
		widget.content = p_path;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	});

	auto& resetButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
	resetButton.idleBackgroundColor = ClearButtonColor;
	resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
	{
		p_data = "";
		widget.content = "Empty";
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
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
