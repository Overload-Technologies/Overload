/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Panels/MaterialEditor.h"
#include "OvEditor/Panels/AssetView.h"

#include "OvEditor/Core/EditorActions.h"

#include <OvCore/Resources/Loaders/MaterialLoader.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Layout/GroupCollapsable.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Texts/TextColored.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Buttons/ButtonSmall.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvCore::Helpers;

namespace
{
	std::string FormatPropertyName(const std::string& p_string)
	{
		std::string result;
		std::string formattedInput = p_string;

		if (formattedInput.rfind("u_", 0) == 0 || formattedInput.rfind("U_", 0) == 0)
		{
			formattedInput = formattedInput.substr(2);
		}

		std::string capsChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		bool first = true;
		bool capsNext = false;

		for (char c : formattedInput)
		{
			if (first || capsNext)
			{
				c = toupper(c);
				first = false;
				capsNext = false;
			}

			if (c == '_')
			{
				c = ' ';
				capsNext = true;
			}

			if (std::find(capsChars.begin(), capsChars.end(), c) != capsChars.end())
				result.push_back(' ');

			result.push_back(c);
		}

		return result;
	}

	void DrawHybridVec3(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector3& p_data, float p_step, float p_min, float p_max)
	{
		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, p_name);

		auto& rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();

		auto& xyzWidget = rightSide.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 3>>(OvCore::Helpers::GUIDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", OvCore::Helpers::GUIDrawer::GetFormat<float>());
		auto& xyzDispatcher = xyzWidget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 3>>>();
		xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 3>&>(p_data));
		xyzWidget.lineBreak = false;

		auto& rgbWidget = rightSide.CreateWidget<OvUI::Widgets::Selection::ColorEdit>(false, OvUI::Types::Color{ p_data.x, p_data.y, p_data.z });
		auto& rgbDispatcher = rgbWidget.AddPlugin<OvUI::Plugins::DataDispatcher<OvUI::Types::Color>>();
		rgbDispatcher.RegisterReference(reinterpret_cast<OvUI::Types::Color&>(p_data));
		rgbWidget.enabled = false;
		rgbWidget.lineBreak = false;

		auto& xyzButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::Button>("XYZ");
		xyzButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
		xyzButton.lineBreak = false;

		auto& rgbButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::Button>("RGB");
		rgbButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

		xyzButton.ClickedEvent += [&] {
			xyzWidget.enabled = true;
			rgbWidget.enabled = false;
		};

		rgbButton.ClickedEvent += [&] {
			xyzWidget.enabled = false;
			rgbWidget.enabled = true;
		};
	}

	void DrawHybridVec4(OvUI::Internal::WidgetContainer& p_root, const std::string& p_name, OvMaths::FVector4& p_data, float p_step, float p_min, float p_max)
	{
		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, p_name);

		auto& rightSide = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();

		auto& xyzWidget = rightSide.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 4>>(OvCore::Helpers::GUIDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", OvCore::Helpers::GUIDrawer::GetFormat<float>());
		auto& xyzDispatcher = xyzWidget.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 4>>>();
		xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 4>&>(p_data));
		xyzWidget.lineBreak = false;

		auto& rgbaWidget = rightSide.CreateWidget<OvUI::Widgets::Selection::ColorEdit>(true, OvUI::Types::Color{ p_data.x, p_data.y, p_data.z, p_data.w });
		auto& rgbaDispatcher = rgbaWidget.AddPlugin<OvUI::Plugins::DataDispatcher<OvUI::Types::Color>>();
		rgbaDispatcher.RegisterReference(reinterpret_cast<OvUI::Types::Color&>(p_data));
		rgbaWidget.enabled = false;
		rgbaWidget.lineBreak = false;

		auto& xyzwButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::Button>("XYZW");
		xyzwButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
		xyzwButton.lineBreak = false;

		auto& rgbaButton = rightSide.CreateWidget<OvUI::Widgets::Buttons::Button>("RGBA");
		rgbaButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };

		xyzwButton.ClickedEvent += [&] {
			xyzWidget.enabled = true;
			rgbaWidget.enabled = false;
		};

		rgbaButton.ClickedEvent += [&] {
			xyzWidget.enabled = false;
			rgbaWidget.enabled = true;
		};
	}
}

OvEditor::Panels::MaterialEditor::MaterialEditor
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings)
{
	CreateHeaderButtons();
	CreateWidget<OvUI::Widgets::Visual::Separator>();
	CreateMaterialSelector();
	m_settings = &CreateWidget<Layout::Group>();
	CreateShaderSelector();
	CreateMaterialSettings();
	CreateShaderSettings();
	CreateFeatureSettings();

	m_settings->enabled = false;
	m_shaderSettings->enabled = false;

	m_materialDroppedEvent += std::bind(&MaterialEditor::OnMaterialDropped, this);
	m_shaderDroppedEvent += std::bind(&MaterialEditor::OnShaderDropped, this);
}

void OvEditor::Panels::MaterialEditor::Refresh()
{
	if (m_target)
		SetTarget(*m_target);
}

void OvEditor::Panels::MaterialEditor::SetTarget(OvCore::Resources::Material & p_newTarget)
{
	m_target = &p_newTarget;
	m_targetMaterialText->content = m_target->path;
	OnMaterialDropped();
}

OvCore::Resources::Material * OvEditor::Panels::MaterialEditor::GetTarget() const
{
	return m_target;
}

void OvEditor::Panels::MaterialEditor::RemoveTarget()
{
	m_target = nullptr;
	m_targetMaterialText->content = "Empty";
	OnMaterialDropped();
}

void OvEditor::Panels::MaterialEditor::Preview()
{
	auto& assetView = EDITOR_PANEL(Panels::AssetView, "Asset View");

	if (m_target)
		assetView.SetResource(m_target);

	assetView.Open();
}

void OvEditor::Panels::MaterialEditor::Reset()
{
	if (m_target && m_shader)
	{
		m_target->SetShader(nullptr);
		OnShaderDropped();
	}
}

void OvEditor::Panels::MaterialEditor::OnMaterialDropped()
{
	m_settings->enabled = m_target; // Enable m_settings group if the target material is non-null

	if (m_settings->enabled)
	{
		GenerateMaterialSettingsContent();
		m_shaderText->content = m_target->GetShader() ? m_target->GetShader()->path : "Empty";
		m_shader = m_target->GetShader();
	}
	else
	{
		m_materialSettingsColumns->RemoveAllWidgets();
	}

	m_shaderSettings->enabled = false;
	m_shaderSettingsColumns->RemoveAllWidgets();

	if (m_target && m_target->GetShader())
		OnShaderDropped();
}

void OvEditor::Panels::MaterialEditor::OnShaderDropped()
{
	m_shaderSettings->enabled = m_shader; // Enable m_shaderSettings group if the shader of the target material is non-null

	if (m_shader != m_target->GetShader())
		m_target->SetShader(m_shader);

	if (m_shaderSettings->enabled)
	{
		GenerateShaderSettingsContent();
		GenerateMaterialFeaturesContent();
	}
	else
	{
		m_shaderSettingsColumns->RemoveAllWidgets();
	}
}

void OvEditor::Panels::MaterialEditor::CreateHeaderButtons()
{
	auto& saveButton = CreateWidget<Buttons::Button>("Save to file");
	saveButton.idleBackgroundColor = { 0.0f, 0.5f, 0.0f };
	saveButton.ClickedEvent += [this]
	{
		if (m_target)
			OvCore::Resources::Loaders::MaterialLoader::Save(*m_target, EDITOR_EXEC(GetRealPath(m_target->path)));
	};

	saveButton.lineBreak = false;

	auto& reloadButton = CreateWidget<Buttons::Button>("Reload from file");
	reloadButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	reloadButton.ClickedEvent += [this]
	{
		if (m_target)
			OvCore::Resources::Loaders::MaterialLoader::Reload(*m_target, EDITOR_EXEC(GetRealPath(m_target->path)));

		OnMaterialDropped();
	};

	reloadButton.lineBreak = false;

	auto& previewButton = CreateWidget<Buttons::Button>("Preview");
	previewButton.idleBackgroundColor = { 0.7f, 0.5f, 0.0f };
	previewButton.ClickedEvent += std::bind(&MaterialEditor::Preview, this);
	previewButton.lineBreak = false;

	auto& resetButton = CreateWidget<Buttons::Button>("Reset to default");
	resetButton.idleBackgroundColor = { 0.5f, 0.0f, 0.0f };
	resetButton.ClickedEvent += std::bind(&MaterialEditor::Reset, this);
}

void OvEditor::Panels::MaterialEditor::CreateMaterialSelector()
{
	auto& columns = CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
	columns.widths[0] = 150;
	m_targetMaterialText = &GUIDrawer::DrawMaterial(columns, "Material", m_target, &m_materialDroppedEvent);
}

void OvEditor::Panels::MaterialEditor::CreateShaderSelector()
{
	auto& columns = m_settings->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
	columns.widths[0] = 150;
	m_shaderText = &GUIDrawer::DrawShader(columns, "Shader", m_shader, &m_shaderDroppedEvent);
}

void OvEditor::Panels::MaterialEditor::CreateMaterialSettings()
{
	m_materialSettings = &m_settings->CreateWidget<Layout::GroupCollapsable>("Material Settings");
	m_materialSettingsColumns = &m_materialSettings->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
	m_materialSettingsColumns->widths[0] = 150;
}

void OvEditor::Panels::MaterialEditor::CreateShaderSettings()
{
	m_shaderSettings = &m_settings->CreateWidget<Layout::GroupCollapsable>("Shader Settings");
	m_shaderSettingsColumns = &m_shaderSettings->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
	m_shaderSettingsColumns->widths[0] = 150;
}

void OvEditor::Panels::MaterialEditor::CreateFeatureSettings()
{
	m_featureSettings = &m_settings->CreateWidget<Layout::GroupCollapsable>("Feature Settings");
	m_featureSettingsColumns = &m_featureSettings->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
	m_featureSettingsColumns->widths[0] = 150;
}

void OvEditor::Panels::MaterialEditor::GenerateShaderSettingsContent()
{
	using namespace OvRendering::Settings;
	using namespace OvRendering::Resources;

	m_shaderSettingsColumns->RemoveAllWidgets(); // Ensure that the m_shaderSettingsColumns is empty

	std::multimap<
		int,
		std::pair<
			std::string,
			std::reference_wrapper<OvRendering::Data::MaterialPropertyType>
		>
	> sortedProperties;

	auto typeIndexVisitor = [&](auto& arg) -> uint32_t {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, Texture*>) return 0;
		else if constexpr (std::is_same_v<T, OvMaths::FVector4>) return 1;
		else if constexpr (std::is_same_v<T, OvMaths::FVector3>) return 2;
		else if constexpr (std::is_same_v<T, OvMaths::FVector2>) return 3;
		else if constexpr (std::is_same_v<T, float>) return 4;
		else if constexpr (std::is_same_v<T, int>) return 5;
		if constexpr (std::is_same_v<T, bool>) return 6;
		return ~static_cast<uint32_t>(0UL);
	};

	for (auto&[name, prop] : m_target->GetProperties())
	{
		if (name.length() > 0 && name[0] != '_') // Uniforms starting with '_' are internal (private), so not exposed
		{
			const auto index = std::visit(typeIndexVisitor, prop.value);
			sortedProperties.emplace(
				index,
				std::pair<std::string, std::reference_wrapper<OvRendering::Data::MaterialPropertyType>>{ 
					name,
					std::ref(prop.value)
			});
		}
	}

	for (auto& [index, propInfo] : sortedProperties)
	{
		const auto& name = propInfo.first;
		auto& prop = propInfo.second.get();

		const auto formattedType = FormatPropertyName(name);

		// Create a visitor to handle each type in the variant
		auto drawVisitor = [&](auto& arg) {
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, bool>)
			{
				GUIDrawer::DrawBoolean(*m_shaderSettingsColumns, formattedType, arg);
			}
			else if constexpr (std::is_same_v<T, int>)
			{
				GUIDrawer::DrawScalar<int>(*m_shaderSettingsColumns, formattedType, arg);
			}
			else if constexpr (std::is_same_v<T, float>)
			{
				GUIDrawer::DrawScalar<float>(*m_shaderSettingsColumns, formattedType, arg, 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
			}
			else if constexpr (std::is_same_v<T, OvMaths::FVector2>)
			{
				GUIDrawer::DrawVec2(*m_shaderSettingsColumns, formattedType, arg, 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
			}
			else if constexpr (std::is_same_v<T, OvMaths::FVector3>)
			{
				DrawHybridVec3(*m_shaderSettingsColumns, formattedType, arg, 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
			}
			else if constexpr (std::is_same_v<T, OvMaths::FVector4>)
			{
				DrawHybridVec4(*m_shaderSettingsColumns, formattedType, arg, 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
			}
			else if constexpr (std::is_same_v<T, Texture*>)
			{
				GUIDrawer::DrawTexture(*m_shaderSettingsColumns, formattedType, arg);
			}
			// No UI for TextureHandle* since it's not handled in the original code
		};

		// Apply the visitor to the variant
		std::visit(drawVisitor, prop);
	}
}

void OvEditor::Panels::MaterialEditor::GenerateMaterialSettingsContent()
{
	m_materialSettingsColumns->RemoveAllWidgets(); // Ensure that the m_shaderSettingsColumns is empty

	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Blendable", std::bind(&OvCore::Resources::Material::IsBlendable, m_target), std::bind(&OvCore::Resources::Material::SetBlendable, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Back-face Culling", std::bind(&OvCore::Resources::Material::HasBackfaceCulling, m_target), std::bind(&OvCore::Resources::Material::SetBackfaceCulling, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Front-face Culling", std::bind(&OvCore::Resources::Material::HasFrontfaceCulling, m_target), std::bind(&OvCore::Resources::Material::SetFrontfaceCulling, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Depth Test", std::bind(&OvCore::Resources::Material::HasDepthTest, m_target), std::bind(&OvCore::Resources::Material::SetDepthTest, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Depth Writing", std::bind(&OvCore::Resources::Material::HasDepthWriting, m_target), std::bind(&OvCore::Resources::Material::SetDepthWriting, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Color Writing", std::bind(&OvCore::Resources::Material::HasColorWriting, m_target), std::bind(&OvCore::Resources::Material::SetColorWriting, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Shadow Casting", std::bind(&OvCore::Resources::Material::IsShadowCaster, m_target), std::bind(&OvCore::Resources::Material::SetCastShadows, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "Shadow Receiving", std::bind(&OvCore::Resources::Material::IsShadowReceiver, m_target), std::bind(&OvCore::Resources::Material::SetReceiveShadows, m_target, std::placeholders::_1));
	GUIDrawer::DrawBoolean(*m_materialSettingsColumns, "User Interface", std::bind(&OvCore::Resources::Material::IsUserInterface, m_target), std::bind(&OvCore::Resources::Material::SetUserInterface, m_target, std::placeholders::_1));
	GUIDrawer::DrawScalar<int>(*m_materialSettingsColumns, "GPU Instances", std::bind(&OvCore::Resources::Material::GetGPUInstances, m_target), std::bind(&OvCore::Resources::Material::SetGPUInstances, m_target, std::placeholders::_1), 1.0f, 0, 100000);
	GUIDrawer::DrawScalar<int>(*m_materialSettingsColumns, "Draw Order", std::bind(&OvCore::Resources::Material::GetDrawOrder, m_target), std::bind(&OvCore::Resources::Material::SetDrawOrder, m_target, std::placeholders::_1), 1.0f, 0, 100000);
}

void OvEditor::Panels::MaterialEditor::GenerateMaterialFeaturesContent()
{
	m_featureSettingsColumns->RemoveAllWidgets();

	if (m_target && m_target->GetShader())
	{
		auto shader = m_target->GetShader();

		auto& features = shader->GetFeatures();
		for (const auto& feature : features)
		{
			GUIDrawer::DrawBoolean(
				*m_featureSettingsColumns,
				feature,
				[this, feature]() -> bool {
					return m_target->HasFeature(feature);
				},
				[this, feature](bool p_enabled) {
					if (p_enabled)
					{
						m_target->AddFeature(feature);
					}
					else
					{
						m_target->RemoveFeature(feature);
					}
				}
			);
		}
	}
}
