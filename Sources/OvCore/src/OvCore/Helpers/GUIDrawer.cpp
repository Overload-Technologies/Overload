/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <filesystem>
#include <memory>

#include <OvTools/Utils/PathParser.h>

#include <OvCore/Helpers/GUIHelpers.h>

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
#include <OvUI/Widgets/Buttons/Toggle.h>
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

namespace
{
	template <size_t N>
	requires (N == 3 || N == 4)
	void DrawHybridVecNImpl(
		OvUI::Internal::WidgetContainer& p_root,
		const std::string& p_name,
		float* p_data,
		float p_step,
		float p_min,
		float p_max)
	{
		using namespace OvUI::Widgets;
		using namespace OvUI::Plugins;

		constexpr bool kHasAlpha = N == 4;

		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, p_name);

		auto& rightSide = p_root.CreateWidget<Layout::Group>();
		rightSide.horizontal = true;
		rightSide.stretchWidget = 0;

		auto& inputField = rightSide.CreateWidget<Layout::Group>();

		auto& vecWidget = inputField.CreateWidget<Drags::DragMultipleScalars<float, N>>(
			OvCore::Helpers::GUIDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", OvCore::Helpers::GUIDrawer::GetFormat<float>());
		vecWidget.template AddPlugin<DataDispatcher<std::array<float, N>>>()
			.RegisterReference(reinterpret_cast<std::array<float, N>&>(*p_data));

		OvUI::Types::Color initialColor;
		if constexpr (N == 3)
			initialColor = { p_data[0], p_data[1], p_data[2] };
		else
			initialColor = { p_data[0], p_data[1], p_data[2], p_data[3] };

		auto& colorWidget = inputField.CreateWidget<Selection::ColorEdit>(kHasAlpha, initialColor);
		auto& colorDispatcher = colorWidget.AddPlugin<DataDispatcher<OvUI::Types::Color>>();
		if constexpr (N == 3)
		{
			colorDispatcher.RegisterGatherer([p_data]() -> OvUI::Types::Color { return { p_data[0], p_data[1], p_data[2] }; });
			colorDispatcher.RegisterProvider([p_data](OvUI::Types::Color c) { p_data[0] = c.r; p_data[1] = c.g; p_data[2] = c.b; });
		}
		else
		{
			colorDispatcher.RegisterReference(reinterpret_cast<OvUI::Types::Color&>(*p_data));
		}
		colorWidget.enabled = false;

		auto& toggle = rightSide.CreateWidget<Buttons::Toggle>("VEC", "COL");
		toggle.tooltip = "Toggle between vector and color display";
		toggle.neverDisabled = true;
		toggle.StateChangedEvent += [&vecWidget, &colorWidget](bool colorMode) {
			vecWidget.enabled   = !colorMode;
			colorWidget.enabled =  colorMode;
		};
	}
}

void OvCore::Helpers::GUIDrawer::DrawHybridVec3(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector3& p_data, float p_step, float p_min, float p_max)
{
	DrawHybridVecNImpl<3>(p_root, p_name, &p_data.x, p_step, p_min, p_max);
}

void OvCore::Helpers::GUIDrawer::DrawHybridVec4(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector4& p_data, float p_step, float p_min, float p_max)
{
	DrawHybridVecNImpl<4>(p_root, p_name, &p_data.x, p_step, p_min, p_max);
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

std::string OvCore::Helpers::GUIDrawer::GetAssetDisplayName(const std::string& p_path)
{
	const std::string friendly = OvTools::Utils::PathParser::GetFriendlyPath(p_path);
	return friendly.empty() ? "" : std::filesystem::path(friendly).stem().string();
}

std::string OvCore::Helpers::GUIDrawer::GetAssetTooltip(const std::string& p_path)
{
	return OvTools::Utils::PathParser::GetFriendlyPath(p_path);
}

namespace
{

	/**
	* Helper to attach an asset picker to a widget's click event.
	* The picker will be available only if the callback is still alive when invoked.
	*/
	void AddSelectButton(
		OvTools::Eventing::Event<>& p_clickedEvent,
		OvTools::Utils::PathParser::EFileType p_fileType,
		std::function<void(const std::string&)> p_onSelect)
	{
		auto token = std::make_shared<bool>(true);
		p_clickedEvent += [p_fileType, p_onSelect = std::move(p_onSelect), token = std::move(token)]
		{
			std::weak_ptr<bool> weak = token;
			OvCore::Helpers::GUIHelpers::OpenAssetPicker(p_fileType, [p_onSelect, weak](const std::string& p_path)
			{
				if (!weak.expired()) p_onSelect(p_path);
			}, true, true);
		};
	}

	/**
	* Generic asset field widget for resources with drag-drop and file selection support.
	* Handles resource loading via the service locator and optional change notifications.
	*/
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
		widget.iconTextureID = OvCore::Helpers::GUIHelpers::GetIconForFileType(p_fileType);
		widget.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
		widget.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;

		// Create a shared widget reference for safe access in captured lambdas
		auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&widget, [](void*) {});

		widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
			[widgetPtr, &p_data, p_updateNotifier, p_fileType](auto p_receivedData)
		{
			if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == p_fileType)
			{
				if (auto resource = OVSERVICE(TResourceManager).GetResource(p_receivedData.first); resource)
				{
					p_data = resource;
					widgetPtr->content = p_receivedData.first;
					if (p_updateNotifier)
						p_updateNotifier->Invoke();
				}
			}
		};

		AddSelectButton(widget.ClickedEvent, p_fileType, [widgetPtr, &p_data, p_updateNotifier](const std::string& p_path)
		{
			if (p_path.empty())
			{
				p_data = nullptr;
				widgetPtr->content.clear();
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
				return;
			}
			if (auto resource = OVSERVICE(TResourceManager).GetResource(p_path); resource)
			{
				p_data = resource;
				widgetPtr->content = p_path;
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		});

		widget.DoubleClickedEvent += [widgetPtr] { OvCore::Helpers::GUIHelpers::Open(widgetPtr->content); };

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
		auto* empty = GUIHelpers::GetEmptyTexture();
		return empty ? empty->GetTexture().GetID() : 0;
	};

	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(p_data ? p_data->path : std::string{});
	widget.iconTextureID = GUIHelpers::GetIconForFileType(OvTools::Utils::PathParser::EFileType::TEXTURE);
	widget.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
	widget.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;
	widget.previewTextureID = getPreviewID();

	// Create a shared widget reference for safe access in captured lambdas
	auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&widget, [](void*) {});

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
		[widgetPtr, &p_data, p_updateNotifier, getPreviewID](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
		{
			if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
			{
				p_data = resource;
				widgetPtr->content = p_receivedData.first;
				widgetPtr->previewTextureID = resource->GetTexture().GetID();
				if (p_updateNotifier)
					p_updateNotifier->Invoke();
			}
		}
	};

	AddSelectButton(widget.ClickedEvent, OvTools::Utils::PathParser::EFileType::TEXTURE, [widgetPtr, &p_data, p_updateNotifier, getPreviewID](const std::string& p_path)
	{
		if (p_path.empty())
		{
			p_data = nullptr;
			widgetPtr->content.clear();
			widgetPtr->previewTextureID = getPreviewID();
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
			return;
		}
		if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_path); resource)
		{
			p_data = resource;
			widgetPtr->content = p_path;
			widgetPtr->previewTextureID = resource->GetTexture().GetID();
			if (p_updateNotifier)
				p_updateNotifier->Invoke();
		}
	});

	widget.DoubleClickedEvent += [widgetPtr] { GUIHelpers::Open(widgetPtr->content); };

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
	widget.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
	widget.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;

	// Create a shared widget reference for safe access in captured lambdas
	auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&widget, [](void*) {});

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [widgetPtr, &p_data, p_updateNotifier](auto p_receivedData)
	{
		p_data = p_receivedData.first;
		widgetPtr->content = p_receivedData.first;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	};

	AddSelectButton(widget.ClickedEvent, OvTools::Utils::PathParser::EFileType::UNKNOWN, [widgetPtr, &p_data, p_updateNotifier](const std::string& p_path)
	{
		p_data = p_path;
		widgetPtr->content = p_path;
		if (p_updateNotifier)
			p_updateNotifier->Invoke();
	});

	widget.DoubleClickedEvent += [widgetPtr] { GUIHelpers::Open(widgetPtr->content); };

	return widget;
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawAsset(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider, OvTools::Utils::PathParser::EFileType p_fileType)
{
	CreateTitle(p_root, p_name);

	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(p_gatherer());
	widget.iconTextureID = GUIHelpers::GetIconForFileType(p_fileType);
	widget.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
	widget.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;

	auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&widget, [](void*) {});

	widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer(p_gatherer);

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
		[widgetPtr, p_provider, p_fileType](auto p_receivedData)
	{
		const bool fileTypeMatch = p_fileType == OvTools::Utils::PathParser::EFileType::UNKNOWN
			|| OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == p_fileType;

		if (fileTypeMatch)
		{
			widgetPtr->content = p_receivedData.first;
			p_provider(p_receivedData.first);
		}
	};

	auto token = std::make_shared<bool>(true);
	widget.ClickedEvent += [widgetPtr, p_provider, p_fileType, token]()
	{
		std::weak_ptr<bool> weak = token;
		GUIHelpers::OpenAssetPicker(p_fileType, [widgetPtr, p_provider, weak](const std::string& p_path)
		{
			if (!weak.expired())
			{
				widgetPtr->content = p_path;
				p_provider(p_path);
			}
		}, true, true);
	};

	widget.DoubleClickedEvent += [widgetPtr] { GUIHelpers::Open(widgetPtr->content); };

	return widget;
}

OvUI::Widgets::InputFields::AssetField& OvCore::Helpers::GUIDrawer::DrawScene(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider)
{
	CreateTitle(p_root, p_name);

	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::AssetField>(p_gatherer());
	widget.iconTextureID = GUIHelpers::GetIconForFileType(OvTools::Utils::PathParser::EFileType::SCENE);
	widget.displayFormatter = &OvCore::Helpers::GUIDrawer::GetAssetDisplayName;
	widget.tooltipFormatter = &OvCore::Helpers::GUIDrawer::GetAssetTooltip;

	// Create a shared widget reference for safe access in captured lambdas
	auto widgetPtr = std::shared_ptr<OvUI::Widgets::InputFields::AssetField>(&widget, [](void*) {});

	widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>().RegisterGatherer(p_gatherer);

	widget.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
		[widgetPtr, p_provider](auto p_receivedData)
	{
		if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SCENE)
		{
			widgetPtr->content = p_receivedData.first;
			p_provider(p_receivedData.first);
		}
	};

	auto token = std::make_shared<bool>(true);
	widget.ClickedEvent += [widgetPtr, p_provider, token]()
	{
		std::weak_ptr<bool> weak = token;
		GUIHelpers::OpenAssetPicker(OvTools::Utils::PathParser::EFileType::SCENE, [widgetPtr, p_provider, weak](const std::string& p_path)
		{
			if (!weak.expired())
			{
				widgetPtr->content = p_path;
				p_provider(p_path);
			}
		}, true, false);
	};

	widget.DoubleClickedEvent += [widgetPtr] { GUIHelpers::Open(widgetPtr->content); };

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

void OvCore::Helpers::GUIDrawer::DrawReadOnlyString(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, std::function<std::string(void)> p_gatherer)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
	widget.disabled = true;
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
	dispatcher.RegisterGatherer(p_gatherer);
}

void OvCore::Helpers::GUIDrawer::DrawColor(OvUI::Internal::WidgetContainer & p_root, const std::string & p_name, std::function<OvUI::Types::Color(void)> p_gatherer, std::function<void(OvUI::Types::Color)> p_provider, bool p_hasAlpha)
{
	CreateTitle(p_root, p_name);
	auto& widget = p_root.CreateWidget<OvUI::Widgets::Selection::ColorEdit>(p_hasAlpha);
	auto& dispatcher = widget.AddPlugin<OvUI::Plugins::DataDispatcher<OvUI::Types::Color>>();
	dispatcher.RegisterGatherer(p_gatherer);
	dispatcher.RegisterProvider(p_provider);
}

OvUI::Widgets::InputFields::InputText& OvCore::Helpers::GUIDrawer::DrawSearchBar(OvUI::Internal::WidgetContainer& p_root, uint32_t p_searchIconTextureID)
{
	auto& searchBar = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>();
	searchBar.fullWidth = true;
	searchBar.iconTextureID = p_searchIconTextureID;
	return searchBar;
}
