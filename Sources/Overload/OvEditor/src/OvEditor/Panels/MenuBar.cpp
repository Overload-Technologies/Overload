/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvTools/Utils/SystemCalls.h>

#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CAudioListener.h>

#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Widgets/Sliders/SliderInt.h>
#include <OvUI/Widgets/Sliders/SliderFloat.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

#include "OvEditor/Panels/MenuBar.h"
#include "OvEditor/Panels/SceneView.h"
#include "OvEditor/Panels/AssetView.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Settings/EditorSettings.h"
#include "OvEditor/Utils/ActorCreationMenu.h"

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvUI::Widgets::Menu;
using namespace OvCore::ECS::Components;

OvEditor::Panels::MenuBar::MenuBar()
{
	CreateFileMenu();
	CreateBuildMenu();
	CreateWindowMenu();
	CreateActorsMenu();
	CreateResourcesMenu();
	CreateSettingsMenu();
	CreateLayoutMenu();
	CreateHelpMenu();
}

void OvEditor::Panels::MenuBar::HandleShortcuts(float p_deltaTime)
{
	auto& inputManager = *EDITOR_CONTEXT(inputManager);

	if (inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_CONTROL) == OvWindowing::Inputs::EKeyState::KEY_DOWN)
	{
		if (inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_N))
			EDITOR_EXEC(LoadEmptyScene());

		if (inputManager.IsKeyPressed(OvWindowing::Inputs::EKey::KEY_S))
		{
			if (inputManager.GetKeyState(OvWindowing::Inputs::EKey::KEY_LEFT_SHIFT) == OvWindowing::Inputs::EKeyState::KEY_UP)
				EDITOR_EXEC(SaveSceneChanges());
			else
				EDITOR_EXEC(SaveAs());
		}
	}
}

void OvEditor::Panels::MenuBar::InitializeSettingsMenu()
{
	auto& themeButton = m_settingsMenu->CreateWidget<MenuList>("Editor Theme");
	themeButton.CreateWidget<Texts::Text>("Some themes may require a restart");

	auto& colorTheme = themeButton.CreateWidget<Selection::ComboBox>(static_cast<int>(Settings::EditorSettings::ColorTheme.Get()));
	colorTheme.choices = {
		{ static_cast<int>(OvUI::Styling::EStyle::IM_CLASSIC_STYLE), "ImGui Classic"},
		{ static_cast<int>(OvUI::Styling::EStyle::IM_DARK_STYLE), "ImGui Dark"},
		{ static_cast<int>(OvUI::Styling::EStyle::IM_LIGHT_STYLE), "ImGui Light"},
		{ static_cast<int>(OvUI::Styling::EStyle::DUNE_DARK), "Dune Dark"},
		{ static_cast<int>(OvUI::Styling::EStyle::DEFAULT_DARK), "Alternative Dark"},
		{ static_cast<int>(OvUI::Styling::EStyle::EVEN_DARKER), "Even Darker"}
	};
	colorTheme.ValueChangedEvent += [this](int p_value)
	{
		Settings::EditorSettings::ColorTheme = p_value;
		EDITOR_CONTEXT(uiManager)->ApplyStyle(static_cast<OvUI::Styling::EStyle>(p_value));
	};

	m_settingsMenu->CreateWidget<MenuItem>("Spawn actors at origin", "", true, true).ValueChangedEvent += EDITOR_BIND(SetActorSpawnAtOrigin, std::placeholders::_1);
	m_settingsMenu->CreateWidget<MenuItem>("Vertical Synchronization", "", true, true).ValueChangedEvent += [this](bool p_value) { EDITOR_CONTEXT(device)->SetVsync(p_value); };
	auto& cameraSpeedMenu = m_settingsMenu->CreateWidget<MenuList>("Camera Speed");
	cameraSpeedMenu.CreateWidget<OvUI::Widgets::Sliders::SliderInt>(1, 50, 15, OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Scene View").ValueChangedEvent += EDITOR_BIND(SetSceneViewCameraSpeed, std::placeholders::_1);
	cameraSpeedMenu.CreateWidget<OvUI::Widgets::Sliders::SliderInt>(1, 50, 15, OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Asset View").ValueChangedEvent += EDITOR_BIND(SetAssetViewCameraSpeed, std::placeholders::_1);
	auto& cameraPositionMenu = m_settingsMenu->CreateWidget<MenuList>("Reset Camera");
	cameraPositionMenu.CreateWidget<MenuItem>("Scene View").ClickedEvent += EDITOR_BIND(ResetSceneViewCameraPosition);
	cameraPositionMenu.CreateWidget<MenuItem>("Asset View").ClickedEvent += EDITOR_BIND(ResetAssetViewCameraPosition);

	auto& sceneView = EDITOR_PANEL(Panels::SceneView, "Scene View");
	auto& assetView = EDITOR_PANEL(Panels::AssetView, "Asset View");

	auto& viewColors = m_settingsMenu->CreateWidget<MenuList>("View Colors");
	auto& sceneViewBackground = viewColors.CreateWidget<MenuList>("Scene View Background");
	auto& sceneViewBackgroundPicker = sceneViewBackground.CreateWidget<Selection::ColorEdit>(false, sceneView.GetCamera()->GetClearColor());
	sceneViewBackgroundPicker.ColorChangedEvent += [&](const auto& color)
	{
		sceneView.GetCamera()->SetClearColor({ color.r, color.g, color.b });
	};
	sceneViewBackground.CreateWidget<MenuItem>("Reset").ClickedEvent += [&]
	{
		sceneView.ResetClearColor();
		sceneViewBackgroundPicker.color = sceneView.GetCamera()->GetClearColor();
	};
	auto& sceneViewGrid = viewColors.CreateWidget<MenuList>("Scene View Grid");

	auto& sceneViewGridPicker = sceneViewGrid.CreateWidget<Selection::ColorEdit>(false, sceneView.GetGridColor());
	sceneViewGridPicker.ColorChangedEvent += [this](const auto& color)
	{
		EDITOR_PANEL(Panels::SceneView, "Scene View").SetGridColor({ color.r, color.g, color.b });
	};
	sceneViewGrid.CreateWidget<MenuItem>("Reset").ClickedEvent += [&]
	{
		sceneView.ResetGridColor();
		sceneViewGridPicker.color = sceneView.GetGridColor();
	};

	auto& assetViewBackground = viewColors.CreateWidget<MenuList>("Asset View Background");
	auto& assetViewBackgroundPicker = assetViewBackground.CreateWidget<Selection::ColorEdit>(false, assetView.GetCamera()->GetClearColor());
	assetViewBackgroundPicker.ColorChangedEvent += [&](const auto& color)
	{
		assetView.GetCamera()->SetClearColor({ color.r, color.g, color.b });
	};
	assetViewBackground.CreateWidget<MenuItem>("Reset").ClickedEvent += [&]
	{
		assetView.ResetClearColor();
		assetViewBackgroundPicker.color = assetView.GetCamera()->GetClearColor();
	};

	auto& assetViewGrid = viewColors.CreateWidget<MenuList>("Asset View Grid");
	auto& assetViewGridPicker = assetViewGrid.CreateWidget<Selection::ColorEdit>(false, assetView.GetGridColor());
	assetViewGridPicker.ColorChangedEvent += [&](const auto& color)
	{
		assetView.SetGridColor({ color.r, color.g, color.b });
	};
	assetViewGrid.CreateWidget<MenuItem>("Reset").ClickedEvent += [&]
	{
		assetView.ResetGridColor();
		assetViewGridPicker.color = assetView.GetGridColor();
	};

	auto& sceneViewBillboardScaleMenu = m_settingsMenu->CreateWidget<MenuList>("3D Icons Scales");
	auto& lightBillboardScaleSlider = sceneViewBillboardScaleMenu.CreateWidget<Sliders::SliderInt>(0, 100, static_cast<int>(Settings::EditorSettings::LightBillboardScale * 100.0f), OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Lights");
	lightBillboardScaleSlider.ValueChangedEvent += [this](int p_value) { Settings::EditorSettings::LightBillboardScale = p_value / 100.0f; };
	lightBillboardScaleSlider.format = "%d %%";

	auto& snappingMenu = m_settingsMenu->CreateWidget<MenuList>("Snapping");
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::TranslationSnapUnit, 0.05f, "Translation Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::TranslationSnapUnit = p_value; };
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::RotationSnapUnit, 1.0f, "Rotation Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::RotationSnapUnit = p_value; };
	snappingMenu.CreateWidget<Drags::DragFloat>(0.001f, 999999.0f, Settings::EditorSettings::ScalingSnapUnit, 0.05f, "Scaling Unit").ValueChangedEvent += [this](float p_value) { Settings::EditorSettings::ScalingSnapUnit = p_value; };

	auto& debuggingMenu = m_settingsMenu->CreateWidget<MenuList>("Debugging");
	debuggingMenu.CreateWidget<MenuItem>("Show geometry bounds", "", true, Settings::EditorSettings::ShowGeometryBounds).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowGeometryBounds = p_value; };
	debuggingMenu.CreateWidget<MenuItem>("Show lights bounds", "", true, Settings::EditorSettings::ShowLightBounds).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::ShowLightBounds = p_value; };
	debuggingMenu.CreateWidget<MenuItem>("Debug Frustum Culling", "", true, Settings::EditorSettings::DebugFrustumCulling).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::DebugFrustumCulling = p_value; };
	debuggingMenu.CreateWidget<MenuItem>("Editor Frustum Geometry Culling", "", true, Settings::EditorSettings::EditorFrustumGeometryCulling).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::EditorFrustumGeometryCulling = p_value; };
	debuggingMenu.CreateWidget<MenuItem>("Editor Frustum Light Culling", "", true, Settings::EditorSettings::EditorFrustumLightCulling).ValueChangedEvent += [this](bool p_value) { Settings::EditorSettings::EditorFrustumLightCulling = p_value; };
}

void OvEditor::Panels::MenuBar::CreateFileMenu()
{
	auto& fileMenu = CreateWidget<MenuList>("File");
	fileMenu.CreateWidget<MenuItem>("New Scene", "CTRL + N").ClickedEvent					+= EDITOR_BIND(LoadEmptyScene);
	fileMenu.CreateWidget<MenuItem>("Save Scene", "CTRL + S").ClickedEvent					+= EDITOR_BIND(SaveSceneChanges);
	fileMenu.CreateWidget<MenuItem>("Save Scene As...", "CTRL + SHIFT + S").ClickedEvent	+= EDITOR_BIND(SaveAs);
	fileMenu.CreateWidget<MenuItem>("Exit", "ALT + F4").ClickedEvent						+= [] { EDITOR_CONTEXT(window)->SetShouldClose(true); };
}

void OvEditor::Panels::MenuBar::CreateBuildMenu()
{
	auto& buildMenu = CreateWidget<MenuList>("Build");
	buildMenu.CreateWidget<MenuItem>("Build game").ClickedEvent					+=	EDITOR_BIND(Build, false, false);
	buildMenu.CreateWidget<MenuItem>("Build game and run").ClickedEvent			+=	EDITOR_BIND(Build, true, false);
	buildMenu.CreateWidget<Visual::Separator>();
	buildMenu.CreateWidget<MenuItem>("Temporary build").ClickedEvent			+=	EDITOR_BIND(Build, true, true);
}

void OvEditor::Panels::MenuBar::CreateWindowMenu()
{
	m_windowMenu = &CreateWidget<MenuList>("Window");
	m_windowMenu->CreateWidget<MenuItem>("Close all").ClickedEvent	+= std::bind(&MenuBar::OpenEveryWindows, this, false);
	m_windowMenu->CreateWidget<MenuItem>("Open all").ClickedEvent		+= std::bind(&MenuBar::OpenEveryWindows, this, true);
	m_windowMenu->CreateWidget<Visual::Separator>();

	/* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
	m_windowMenu->ClickedEvent += std::bind(&MenuBar::UpdateToggleableItems, this);
}

void OvEditor::Panels::MenuBar::CreateActorsMenu()
{
	auto& actorsMenu = CreateWidget<MenuList>("Actors");
    Utils::ActorCreationMenu::GenerateActorCreationMenu(actorsMenu);
}

void OvEditor::Panels::MenuBar::CreateResourcesMenu()
{
	auto& resourcesMenu = CreateWidget<MenuList>("Resources");
	resourcesMenu.CreateWidget<MenuItem>("Compile shaders").ClickedEvent += EDITOR_BIND(CompileShaders);
	resourcesMenu.CreateWidget<MenuItem>("Save materials").ClickedEvent += EDITOR_BIND(SaveMaterials);
}

void OvEditor::Panels::MenuBar::CreateSettingsMenu()
{
	m_settingsMenu = &CreateWidget<MenuList>("Settings");
}

void OvEditor::Panels::MenuBar::CreateLayoutMenu() 
{
	auto& layoutMenu = CreateWidget<MenuList>("Layout");
	layoutMenu.CreateWidget<MenuItem>("Reset").ClickedEvent += EDITOR_BIND(ResetLayout);
}

void OvEditor::Panels::MenuBar::CreateHelpMenu()
{
    auto& helpMenu = CreateWidget<MenuList>("Help");
    helpMenu.CreateWidget<MenuItem>("GitHub").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/Overload-Technologies/Overload"); };
    helpMenu.CreateWidget<MenuItem>("Tutorials").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/Overload-Technologies/Overload/wiki/Tutorials"); };
    helpMenu.CreateWidget<MenuItem>("Scripting API").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/Overload-Technologies/Overload/wiki/Scripting-API"); };
    helpMenu.CreateWidget<Visual::Separator>();
    helpMenu.CreateWidget<MenuItem>("Bug Report").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/Overload-Technologies/Overload/issues/new?assignees=&labels=Bug&template=bug_report.md&title="); };
    helpMenu.CreateWidget<MenuItem>("Feature Request").ClickedEvent += [] {OvTools::Utils::SystemCalls::OpenURL("https://github.com/Overload-Technologies/Overload/issues/new?assignees=&labels=Feature&template=feature_request.md&title="); };
    helpMenu.CreateWidget<Visual::Separator>();
    helpMenu.CreateWidget<Texts::Text>("Version: " + std::string(OVERLOAD_VERSION));
}

void OvEditor::Panels::MenuBar::RegisterPanel(const std::string& p_name, OvUI::Panels::PanelWindow& p_panel)
{
	auto& menuItem = m_windowMenu->CreateWidget<MenuItem>(p_name, "", true, true);
	menuItem.ValueChangedEvent += std::bind(&OvUI::Panels::PanelWindow::SetOpened, &p_panel, std::placeholders::_1);

	m_panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
}

void OvEditor::Panels::MenuBar::UpdateToggleableItems()
{
	for (auto&[name, panel] : m_panels)
		panel.second.get().checked = panel.first.get().IsOpened();
}

void OvEditor::Panels::MenuBar::OpenEveryWindows(bool p_state)
{
	for (auto&[name, panel] : m_panels)
		panel.first.get().SetOpened(p_state);
}
