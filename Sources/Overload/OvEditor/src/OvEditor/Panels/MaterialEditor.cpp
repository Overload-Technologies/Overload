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

	xyzButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = true;
		rgbWidget.enabled = false;
	};

	rgbButton.ClickedEvent += [&]
	{
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

	xyzwButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = true;
		rgbaWidget.enabled = false;
	};

	rgbaButton.ClickedEvent += [&]
	{
		xyzWidget.enabled = false;
		rgbaWidget.enabled = true;
	};
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

	m_settings->enabled = false;
	m_shaderSettings->enabled = false;

	m_materialDroppedEvent	+= std::bind(&MaterialEditor::OnMaterialDropped, this);
	m_shaderDroppedEvent	+= std::bind(&MaterialEditor::OnShaderDropped, this);
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

std::string UniformFormat(const std::string& p_string)
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

void OvEditor::Panels::MaterialEditor::GenerateShaderSettingsContent()
{
	using namespace OvRendering::Settings;
	using namespace OvRendering::Resources;

	m_shaderSettingsColumns->RemoveAllWidgets(); // Ensure that the m_shaderSettingsColumns is empty

	std::multimap<int, std::pair<std::string, std::any*>> sortedUniformsData;

	for (auto&[name, value] : m_target->GetProperties())
	{
		int orderID = 0;

		auto uniformData = m_target->GetShader()->GetProgram().GetUniformInfo(name);

		if (uniformData && name.length() > 0 && name[0] != '_') // Uniforms starting with '_' are internal (private), so not exposed
		{
			switch (uniformData->type)
			{
			case EUniformType::SAMPLER_2D:	orderID = 0; break;
			case EUniformType::FLOAT_VEC4:	orderID = 1; break;
			case EUniformType::FLOAT_VEC3:	orderID = 2; break;
			case EUniformType::FLOAT_VEC2:	orderID = 3; break;
			case EUniformType::FLOAT:		orderID = 4; break;
			case EUniformType::INT:			orderID = 5; break;
			case EUniformType::BOOL:			orderID = 6; break;
			}

			sortedUniformsData.emplace(orderID, std::pair<std::string, std::any*>{ name, & value.value });
		}
	}

	for (auto& [order, info] : sortedUniformsData)
	{
		auto uniformData = m_target->GetShader()->GetProgram().GetUniformInfo(info.first);
		
		if (uniformData)
		{
			const auto formattedType = UniformFormat(info.first);
			switch (uniformData->type)
			{
			case EUniformType::BOOL:			GUIDrawer::DrawBoolean(*m_shaderSettingsColumns, formattedType, reinterpret_cast<bool&>(*info.second));																	break;
			case EUniformType::INT:			GUIDrawer::DrawScalar<int>(*m_shaderSettingsColumns, formattedType, reinterpret_cast<int&>(*info.second));																break;
			case EUniformType::FLOAT:		GUIDrawer::DrawScalar<float>(*m_shaderSettingsColumns, formattedType, reinterpret_cast<float&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);		break;
			case EUniformType::FLOAT_VEC2:	GUIDrawer::DrawVec2(*m_shaderSettingsColumns, formattedType, reinterpret_cast<OvMaths::FVector2&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);	break;
			case EUniformType::FLOAT_VEC3:	DrawHybridVec3(*m_shaderSettingsColumns, formattedType, reinterpret_cast<OvMaths::FVector3&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);			break;
			case EUniformType::FLOAT_VEC4:	DrawHybridVec4(*m_shaderSettingsColumns, formattedType, reinterpret_cast<OvMaths::FVector4&>(*info.second), 0.01f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);			break;
			case EUniformType::SAMPLER_2D:
				{
					try
					{
						auto& texture = std::any_cast<Texture*&>(*info.second);
						GUIDrawer::DrawTexture(*m_shaderSettingsColumns, formattedType, texture);
					}
					catch (const std::bad_any_cast&)
					{
						// If the cast failed, it means that the uniform is not a texture (It's a texture handle)
					}
				}
			}
		}
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
}
