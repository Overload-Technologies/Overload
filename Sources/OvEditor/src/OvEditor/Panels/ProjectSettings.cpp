/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Panels/ProjectSettings.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/Inspector.h"
#include "OvTools/Utils/PathParser.h"

#include <OvCore/Resources/Loaders/MaterialLoader.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvUI/Styling/Style.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Layout/GroupCollapsable.h>
#include <OvUI/Widgets/Layout/TreeNode.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Plugins/DataDispatcher.h>

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvCore::Helpers;

namespace
{
	constexpr uint32_t kSlotCount = OvPhysics::Settings::CollisionLayers::kMaxLayerCount;

	// Widgets are destroyed rather than removed, so this stays safe when called from the event
	// handler of one of the widgets being replaced
	void ClearWidgets(OvUI::Internal::WidgetContainer& p_container)
	{
		for (auto& widget : p_container.GetWidgets())
		{
			widget.first->Destroy();
		}
	}

	bool HasFreeLayer(const OvPhysics::Settings::CollisionLayers& p_collisionLayers)
	{
		for (uint32_t layer = 0; layer < kSlotCount; ++layer)
		{
			if (!p_collisionLayers.IsLayerUsed(layer))
			{
				return true;
			}
		}

		return false;
	}

	std::string GenerateLayerName(const OvPhysics::Settings::CollisionLayers& p_collisionLayers)
	{
		std::string name = "New Layer";

		for (uint32_t suffix = 2; p_collisionLayers.FindLayer(name); ++suffix)
		{
			name = "New Layer " + std::to_string(suffix);
		}

		return name;
	}
}

OvEditor::Panels::ProjectSettings::ProjectSettings(const std::string & p_title, bool p_opened, const OvUI::Settings::PanelWindowSettings & p_windowSettings) :
	PanelWindow(p_title, p_opened, p_windowSettings),
	m_projectFile(EDITOR_CONTEXT(projectSettings))
{
	auto& saveButton = CreateWidget<Buttons::Button>("Apply");
	saveButton.backgroundColor = OVUI_STYLE(SuccessButton);
	saveButton.hoveredBackgroundColor = OVUI_STYLE(SuccessButtonHovered);
	saveButton.clickedBackgroundColor = OVUI_STYLE(SuccessButtonActive);
	saveButton.ClickedEvent += [this]
	{
		EDITOR_CONTEXT(ApplyProjectSettings());
		m_projectFile.Rewrite();

		/* Inspectors built from the settings, such as the collision layer picker, are now stale */
		EDITOR_PANEL(OvEditor::Panels::Inspector, "Inspector").Refresh();
	};

	saveButton.lineBreak = false;

	auto& resetButton = CreateWidget<Buttons::Button>("Reset");
	resetButton.backgroundColor = OVUI_STYLE(DangerButton);
	resetButton.hoveredBackgroundColor = OVUI_STYLE(DangerButtonHovered);
	resetButton.clickedBackgroundColor = OVUI_STYLE(DangerButtonActive);
	resetButton.ClickedEvent += [this]
	{
		EDITOR_CONTEXT(ResetProjectSettings());
		ReloadCollisionLayers();
	};

	CreateWidget<Visual::Separator>();

	{
		/* Physics settings */
		auto& root = CreateWidget<Layout::GroupCollapsable>("Physics");
		auto& columns = root.CreateWidget<Layout::Columns<2>>();
		columns.widths[0] = 125 * OVUI_SCALE;

		GUIDrawer::DrawScalar<float>(columns, "Gravity", GenerateGatherer<float>("gravity"), GenerateProvider<float>("gravity"), 0.1f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);

		m_collisionLayersRoot = &root.CreateWidget<Layout::Group>();
		m_collisionMatrixRoot = &root.CreateWidget<Layout::Group>();
		ReloadCollisionLayers();
	}

	{
		/* Build settings */
		auto& generationRoot = CreateWidget<Layout::GroupCollapsable>("Build");
		auto& columns = generationRoot.CreateWidget<Layout::Columns<2>>();
		columns.widths[0] = 125 * OVUI_SCALE;

		GUIDrawer::CreateTitle(columns, "Build Type");
		auto& comboBox = columns.CreateWidget<OvUI::Widgets::Selection::ComboBox>(m_projectFile.Get<int>("build_type"));
		comboBox.choices = {
			{ static_cast<int>(OvEditor::Core::EBuildType::Debug),   OvEditor::Core::GetBuildTypeName(OvEditor::Core::EBuildType::Debug) },
			{ static_cast<int>(OvEditor::Core::EBuildType::Release), OvEditor::Core::GetBuildTypeName(OvEditor::Core::EBuildType::Release) },
			{ static_cast<int>(OvEditor::Core::EBuildType::Publish), OvEditor::Core::GetBuildTypeName(OvEditor::Core::EBuildType::Publish) },
		};
		auto& dispatcher = comboBox.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
		dispatcher.RegisterGatherer(GenerateGatherer<int>("build_type"));
		dispatcher.RegisterProvider(GenerateProvider<int>("build_type"));
	}

	{
		/* Windowing settings */
		auto& windowingRoot = CreateWidget<Layout::GroupCollapsable>("Windowing");
		auto& columns = windowingRoot.CreateWidget<Layout::Columns<2>>();
		columns.widths[0] = 125 * OVUI_SCALE;

		GUIDrawer::DrawScalar<int>(columns, "Resolution X", GenerateGatherer<int>("x_resolution"), GenerateProvider<int>("x_resolution"), 1, 0, 10000);
		GUIDrawer::DrawScalar<int>(columns, "Resolution Y", GenerateGatherer<int>("y_resolution"), GenerateProvider<int>("y_resolution"), 1, 0, 10000);
		GUIDrawer::DrawBoolean(columns, "Fullscreen", GenerateGatherer<bool>("fullscreen"), GenerateProvider<bool>("fullscreen"));
		GUIDrawer::DrawBoolean(columns, "Allow resizing", GenerateGatherer<bool>("resizable"), GenerateProvider<bool>("resizable"));
		GUIDrawer::DrawString(columns, "Executable name", GenerateGatherer<std::string>("executable_name"), GenerateProvider<std::string>("executable_name"));
		GUIDrawer::DrawAsset(columns, "Window Icon", GenerateGatherer<std::string>("window_icon"), GenerateProvider<std::string>("window_icon"), OvTools::Utils::PathParser::EFileType::TEXTURE);
	}

	{
		/* Rendering settings */
		auto& renderingRoot = CreateWidget<Layout::GroupCollapsable>("Rendering");
		auto& columns = renderingRoot.CreateWidget<Layout::Columns<2>>();
		columns.widths[0] = 125 * OVUI_SCALE;

		GUIDrawer::DrawBoolean(columns, "Vertical Sync.", GenerateGatherer<bool>("vsync"), GenerateProvider<bool>("vsync"));
		GUIDrawer::DrawBoolean(columns, "Multi-sampling", GenerateGatherer<bool>("multisampling"), GenerateProvider<bool>("multisampling"));
		GUIDrawer::DrawScalar<int>(columns, "Samples", GenerateGatherer<int>("samples"), GenerateProvider<int>("samples"), 1, 2, 16);
	}

	{
		/* Scene Management settings */
		auto& gameRoot = CreateWidget<Layout::GroupCollapsable>("Scene Management");
		auto& columns = gameRoot.CreateWidget<Layout::Columns<2>>();
		columns.widths[0] = 125 * OVUI_SCALE;

		GUIDrawer::DrawScene(columns, "Start scene", GenerateGatherer<std::string>("start_scene"), GenerateProvider<std::string>("start_scene"));
	}
}

void OvEditor::Panels::ProjectSettings::BuildCollisionLayerWidgets(OvUI::Internal::WidgetContainer& p_container)
{
	ClearWidgets(p_container);

	GUIDrawer::CreateTitle(p_container, "Collision Layers");

	for (uint32_t layer = 0; layer < kSlotCount; ++layer)
	{
		if (!m_collisionLayers.IsLayerUsed(layer))
		{
			continue;
		}

		auto& layerRow = p_container.CreateWidget<Layout::Columns<2>>();
		layerRow.widths[0] = 125 * OVUI_SCALE;
		layerRow.SetID("collision_layer_" + std::to_string(layer));

		/* Scenes store the slot index, so it is shown to tell the layers apart whatever their name */
		GUIDrawer::CreateTitle(layerRow, "Layer " + std::to_string(layer));

		if (layer == OvPhysics::Settings::CollisionLayers::kDefaultLayer)
		{
			layerRow.CreateWidget<Texts::Text>(m_collisionLayers.GetLayerName(layer));
		}
		else
		{
			auto& layerEditor = layerRow.CreateWidget<Layout::Group>();
			layerEditor.horizontal = true;
			layerEditor.stretchWidget = 0; // Otherwise the name field shrinks down to its content

			auto& layerName = layerEditor.CreateWidget<InputFields::InputText>(m_collisionLayers.GetLayerName(layer));
			layerName.ContentChangedEvent += [this, layer](const std::string& p_name)
			{
				m_collisionLayers.RenameLayer(layer, p_name);
				StoreCollisionLayers();

				// Only the matrix is rebuilt, so the field being typed into keeps its focus
				BuildCollisionMatrixWidgets(*m_collisionMatrixRoot);
			};

			// Rebuilding restores the field to the stored name, showing that a rename was refused
			layerName.EnterPressedEvent += [this](const std::string&)
			{
				BuildCollisionLayerWidgets(*m_collisionLayersRoot);
			};

			auto& removeButton = layerEditor.CreateWidget<Buttons::Button>("Remove");
			removeButton.ClickedEvent += [this, layer]
			{
				m_collisionLayers.RemoveLayer(layer);
				StoreCollisionLayers();
				BuildCollisionLayerWidgets(*m_collisionLayersRoot);
				BuildCollisionMatrixWidgets(*m_collisionMatrixRoot);
			};
		}
	}

	if (HasFreeLayer(m_collisionLayers))
	{
		auto& addButton = p_container.CreateWidget<Buttons::Button>("Add Layer");
		addButton.ClickedEvent += [this]
		{
			m_collisionLayers.AddLayer(GenerateLayerName(m_collisionLayers));
			StoreCollisionLayers();
			BuildCollisionLayerWidgets(*m_collisionLayersRoot);
			BuildCollisionMatrixWidgets(*m_collisionMatrixRoot);
		};
	}
}

void OvEditor::Panels::ProjectSettings::BuildCollisionMatrixWidgets(OvUI::Internal::WidgetContainer& p_container)
{
	ClearWidgets(p_container);

	GUIDrawer::CreateTitle(p_container, "Collision Matrix");

	for (uint32_t first = 0; first < kSlotCount; ++first)
	{
		if (!m_collisionLayers.IsLayerUsed(first))
		{
			continue;
		}

		auto& matrixRow = p_container.CreateWidget<Layout::TreeNode>(m_collisionLayers.GetLayerName(first));

		/* The extra '#' makes it a '###' id, which ImGui derives from the slot alone, so the node
		   keeps its folded state while its layer is being renamed */
		matrixRow.SetID("#collision_matrix_" + std::to_string(first));

		/* The matrix is symmetric, so every pair is only offered once, on the row of its first layer */
		for (uint32_t second = first; second < kSlotCount; ++second)
		{
			if (!m_collisionLayers.IsLayerUsed(second))
			{
				continue;
			}

			auto& cell = matrixRow.CreateWidget<Selection::CheckBox>(
				m_collisionLayers.GetLayerCollision(first, second),
				m_collisionLayers.GetLayerName(second)
			);
			cell.ValueChangedEvent += [this, first, second](bool p_collide)
			{
				m_collisionLayers.SetLayerCollision(first, second, p_collide);
				StoreCollisionLayers();
			};
		}
	}
}

void OvEditor::Panels::ProjectSettings::ReloadCollisionLayers()
{
	m_collisionLayers.Deserialize(m_projectFile);

	BuildCollisionLayerWidgets(*m_collisionLayersRoot);
	BuildCollisionMatrixWidgets(*m_collisionMatrixRoot);
}

void OvEditor::Panels::ProjectSettings::StoreCollisionLayers()
{
	m_collisionLayers.Serialize(m_projectFile);
}
