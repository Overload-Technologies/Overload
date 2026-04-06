/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <format>
#include <memory>
#include <vector>

#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Utils/CodeEditor.h>
#include <OvTools/Utils/String.h>
#include <OvTools/Utils/SystemCalls.h>

#include <OvDebug/Logger.h>

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
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Sliders/SliderInt.h>
#include <OvUI/Widgets/Sliders/SliderFloat.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Texts/Text.h>

#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/AssetView.h"
#include "OvEditor/Panels/Console.h"
#include "OvEditor/Panels/MenuBar.h"
#include "OvEditor/Panels/SceneView.h"
#include "OvEditor/Settings/EditorSettings.h"
#include "OvEditor/Utils/ActorCreationMenu.h"

using namespace OvUI::Panels;
using namespace OvUI::Widgets;
using namespace OvUI::Widgets::Menu;
using namespace OvCore::ECS::Components;

namespace
{
	constexpr std::string_view kCodeEditorSettingKey = "code_editor_name";
	constexpr std::string_view kDefaultCodeEditorName = "Visual Studio Code";
	constexpr std::string_view kSublimeTextCodeEditorName = "Sublime Text";
	constexpr std::string_view kAtomCodeEditorName = "Atom";
	constexpr std::string_view kCustomCodeEditorCountSettingKey = "custom_code_editor_count";
	constexpr std::string_view kCustomCodeEditorNameKeySuffix = "name";
	constexpr std::string_view kCustomCodeEditorCommandKeySuffix = "command";
	constexpr std::string_view kCustomCodeEditorArgumentsKeySuffix = "arguments";
	constexpr std::string_view kDefaultCodeEditorArguments = "\"{path}\"";

	struct CustomCodeEditor
	{
		std::string name;
		std::string command;
		std::string arguments;
	};

	std::filesystem::path GetEditorIniFilePath()
	{
		const std::filesystem::path editorDataPath = std::filesystem::path{
			OvTools::Utils::SystemCalls::GetPathToAppdata()
		} / "OverloadTech" / "OvEditor";
		std::filesystem::create_directories(editorDataPath);
		return editorDataPath / "editor.ini";
	}

	template<typename T>
	void SetOrAdd(OvTools::Filesystem::IniFile& p_iniFile, const std::string& p_key, const T& p_value)
	{
		if (!p_iniFile.Set<T>(p_key, p_value))
		{
			p_iniFile.Add<T>(p_key, p_value);
		}
	}

	std::string MakeCustomCodeEditorKey(const int p_index, const std::string_view p_suffix)
	{
		return std::format("custom_code_editor_{}_{}", p_index, p_suffix);
	}

	std::string NormalizeCodeEditorName(std::string p_name)
	{
		std::transform(
			p_name.begin(),
			p_name.end(),
			p_name.begin(),
			[](const unsigned char p_character)
			{
				return static_cast<char>(std::tolower(p_character));
			}
		);
		return p_name;
	}

	bool IsBuiltInCodeEditorName(const std::string& p_editorName)
	{
		const std::string normalizedName = NormalizeCodeEditorName(p_editorName);
		return
			normalizedName == NormalizeCodeEditorName(std::string{ kDefaultCodeEditorName }) ||
			normalizedName == NormalizeCodeEditorName(std::string{ kSublimeTextCodeEditorName }) ||
			normalizedName == NormalizeCodeEditorName(std::string{ kAtomCodeEditorName });
	}

	std::vector<CustomCodeEditor> LoadCustomCodeEditors(OvTools::Filesystem::IniFile& p_settingsFile)
	{
		const int customEditorCount = std::max(
			0,
			p_settingsFile.GetOrDefault<int>(std::string{ kCustomCodeEditorCountSettingKey }, 0)
		);

		std::vector<CustomCodeEditor> customEditors;
		customEditors.reserve(static_cast<size_t>(customEditorCount));

		for (int i = 0; i < customEditorCount; ++i)
		{
			std::string editorName = p_settingsFile.GetOrDefault<std::string>(MakeCustomCodeEditorKey(i, kCustomCodeEditorNameKeySuffix), "");
			std::string editorCommand = p_settingsFile.GetOrDefault<std::string>(MakeCustomCodeEditorKey(i, kCustomCodeEditorCommandKeySuffix), "");
			std::string editorArguments = p_settingsFile.GetOrDefault<std::string>(MakeCustomCodeEditorKey(i, kCustomCodeEditorArgumentsKeySuffix), std::string{ kDefaultCodeEditorArguments });

			OvTools::Utils::String::Trim(editorName);
			OvTools::Utils::String::Trim(editorCommand);
			OvTools::Utils::String::Trim(editorArguments);

			if (editorName.empty() || editorCommand.empty())
			{
				continue;
			}

			if (editorArguments.empty())
			{
				editorArguments = std::string{ kDefaultCodeEditorArguments };
			}

			customEditors.push_back(
				CustomCodeEditor
				{
					editorName,
					editorCommand,
					editorArguments
				}
			);
		}

		return customEditors;
	}

	void SaveCustomCodeEditors(OvTools::Filesystem::IniFile& p_settingsFile, const std::vector<CustomCodeEditor>& p_customEditors)
	{
		const std::string customEditorCountKey{ kCustomCodeEditorCountSettingKey };
		const int previousEditorCount = std::max(
			0,
			p_settingsFile.GetOrDefault<int>(customEditorCountKey, 0)
		);

		for (int i = 0; i < previousEditorCount; ++i)
		{
			p_settingsFile.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorNameKeySuffix));
			p_settingsFile.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorCommandKeySuffix));
			p_settingsFile.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorArgumentsKeySuffix));
		}

		SetOrAdd<int>(p_settingsFile, customEditorCountKey, static_cast<int>(p_customEditors.size()));

		for (size_t i = 0; i < p_customEditors.size(); ++i)
		{
			const int index = static_cast<int>(i);
			const auto& customEditor = p_customEditors[i];

			SetOrAdd<std::string>(p_settingsFile, MakeCustomCodeEditorKey(index, kCustomCodeEditorNameKeySuffix), customEditor.name);
			SetOrAdd<std::string>(p_settingsFile, MakeCustomCodeEditorKey(index, kCustomCodeEditorCommandKeySuffix), customEditor.command);
			SetOrAdd<std::string>(p_settingsFile, MakeCustomCodeEditorKey(index, kCustomCodeEditorArgumentsKeySuffix), customEditor.arguments);
		}
	}

	void UpsertCustomCodeEditor(OvTools::Filesystem::IniFile& p_settingsFile, const CustomCodeEditor& p_customEditor)
	{
		auto customEditors = LoadCustomCodeEditors(p_settingsFile);
		const std::string normalizedCustomEditorName = NormalizeCodeEditorName(p_customEditor.name);

		const auto existingEditor = std::find_if(
			customEditors.begin(),
			customEditors.end(),
			[normalizedCustomEditorName](const CustomCodeEditor& p_editor)
			{
				return NormalizeCodeEditorName(p_editor.name) == normalizedCustomEditorName;
			}
		);

		if (existingEditor != customEditors.end())
		{
			*existingEditor = p_customEditor;
		}
		else
		{
			customEditors.push_back(p_customEditor);
		}

		SaveCustomCodeEditors(p_settingsFile, customEditors);
	}

	bool RemoveCustomCodeEditor(OvTools::Filesystem::IniFile& p_settingsFile, const std::string& p_editorName)
	{
		auto customEditors = LoadCustomCodeEditors(p_settingsFile);
		const std::string normalizedEditorName = NormalizeCodeEditorName(p_editorName);

		const auto removedEditor = std::remove_if(
			customEditors.begin(),
			customEditors.end(),
			[normalizedEditorName](const CustomCodeEditor& p_editor)
			{
				return NormalizeCodeEditorName(p_editor.name) == normalizedEditorName;
			}
		);

		if (removedEditor == customEditors.end())
		{
			return false;
		}

		customEditors.erase(removedEditor, customEditors.end());
		SaveCustomCodeEditors(p_settingsFile, customEditors);
		return true;
	}

	void RegisterCustomCodeEditors(OvTools::Filesystem::IniFile& p_settingsFile)
	{
		for (const auto& customEditor : LoadCustomCodeEditors(p_settingsFile))
		{
			if (IsBuiltInCodeEditorName(customEditor.name))
			{
				continue;
			}

			OvTools::Utils::CodeEditor::Register(customEditor.name, customEditor.command, customEditor.arguments);
		}
	}

	void MigrateLegacyProjectCodeEditorsToEditorSettings
	(
		OvTools::Filesystem::IniFile& p_projectSettings,
		OvTools::Filesystem::IniFile& p_editorSettings
	)
	{
		const int editorSettingsCustomEditorCount = std::max(
			0,
			p_editorSettings.GetOrDefault<int>(std::string{ kCustomCodeEditorCountSettingKey }, 0)
		);

		if (editorSettingsCustomEditorCount > 0)
		{
			return;
		}

		const auto legacyCustomEditors = LoadCustomCodeEditors(p_projectSettings);
		if (legacyCustomEditors.empty())
		{
			return;
		}

		SaveCustomCodeEditors(p_editorSettings, legacyCustomEditors);
		p_editorSettings.Rewrite();

		const int legacyCustomEditorCount = std::max(
			0,
			p_projectSettings.GetOrDefault<int>(std::string{ kCustomCodeEditorCountSettingKey }, 0)
		);

		for (int i = 0; i < legacyCustomEditorCount; ++i)
		{
			p_projectSettings.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorNameKeySuffix));
			p_projectSettings.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorCommandKeySuffix));
			p_projectSettings.Remove(MakeCustomCodeEditorKey(i, kCustomCodeEditorArgumentsKeySuffix));
		}

		SetOrAdd<int>(p_projectSettings, std::string{ kCustomCodeEditorCountSettingKey }, 0);
		p_projectSettings.Rewrite();
	}

	void MigrateLegacyProjectCodeEditorSelectionToEditorSettings
	(
		OvTools::Filesystem::IniFile& p_projectSettings,
		OvTools::Filesystem::IniFile& p_editorSettings
	)
	{
		const std::string codeEditorSettingKey{ kCodeEditorSettingKey };
		bool projectSettingsChanged = false;

		if (!p_editorSettings.IsKeyExisting(codeEditorSettingKey) && p_projectSettings.IsKeyExisting(codeEditorSettingKey))
		{
			SetOrAdd<std::string>(p_editorSettings, codeEditorSettingKey, p_projectSettings.Get<std::string>(codeEditorSettingKey));
			p_editorSettings.Rewrite();
		}

		if (p_projectSettings.Remove(codeEditorSettingKey))
		{
			projectSettingsChanged = true;
		}

		if (projectSettingsChanged)
		{
			p_projectSettings.Rewrite();
		}
	}

	void FillCodeEditorSelectorChoices
	(
		OvUI::Widgets::Selection::ComboBox* p_codeEditorSelector,
		const std::vector<std::string>& p_codeEditors,
		const std::string& p_selectedCodeEditor
	)
	{
		if (p_codeEditorSelector == nullptr || p_codeEditors.empty())
		{
			return;
		}

		p_codeEditorSelector->choices.clear();

		int selectedChoice = 0;
		bool selectedCodeEditorFound = false;

		for (size_t i = 0; i < p_codeEditors.size(); ++i)
		{
			const int choiceId = static_cast<int>(i);
			const std::string& codeEditor = p_codeEditors[i];
			p_codeEditorSelector->choices[choiceId] = codeEditor;

			if (codeEditor == p_selectedCodeEditor)
			{
				selectedChoice = choiceId;
				selectedCodeEditorFound = true;
			}
		}

		if (!selectedCodeEditorFound)
		{
			selectedChoice = 0;
		}

		p_codeEditorSelector->currentChoice = selectedChoice;
	}
}

OvEditor::Panels::MenuBar::MenuBar()
{
	CreateFileMenu();
	CreateBuildMenu();
	CreateWindowMenu();
	CreateActorsMenu();
	CreateResourcesMenu();
	CreateToolsMenu();
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
	auto& projectSettings = EDITOR_CONTEXT(projectSettings);
	auto editorSettingsFile = std::make_shared<OvTools::Filesystem::IniFile>(GetEditorIniFilePath().string());
	MigrateLegacyProjectCodeEditorsToEditorSettings(projectSettings, *editorSettingsFile);
	MigrateLegacyProjectCodeEditorSelectionToEditorSettings(projectSettings, *editorSettingsFile);
	RegisterCustomCodeEditors(*editorSettingsFile);

	const std::string codeEditorSettingKey{ kCodeEditorSettingKey };
	auto supportedCodeEditors = std::make_shared<std::vector<std::string>>(
		OvTools::Utils::CodeEditor::GetRegistered()
	);

	auto& codeEditorMenu = m_settingsMenu->CreateWidget<MenuList>("Code Editor");
	OvUI::Widgets::Selection::ComboBox* codeEditorSelector = nullptr;

	if (supportedCodeEditors->empty())
	{
		codeEditorMenu.CreateWidget<Texts::Text>("No registered code editors");
	}
	else
	{
		int selectedChoice = 0;
		auto& newCodeEditorSelector = codeEditorMenu.CreateWidget<Selection::ComboBox>(selectedChoice);
		codeEditorSelector = &newCodeEditorSelector;
	}

	const auto RefreshCodeEditorSelection = [editorSettingsFile, supportedCodeEditors, codeEditorSelector, codeEditorSettingKey](std::string p_preferredCodeEditor, bool p_logWarningOnFallback)
	{
		*supportedCodeEditors = OvTools::Utils::CodeEditor::GetRegistered();

		std::string selectedCodeEditor = p_preferredCodeEditor.empty() ?
			editorSettingsFile->GetOrDefault<std::string>(codeEditorSettingKey, std::string{ kDefaultCodeEditorName }) :
			p_preferredCodeEditor;

		std::string fallbackCodeEditor;
		if (!OvTools::Utils::CodeEditor::SetDefault(selectedCodeEditor))
		{
			if (!supportedCodeEditors->empty())
			{
				fallbackCodeEditor = supportedCodeEditors->front();
				selectedCodeEditor = fallbackCodeEditor;
				OvTools::Utils::CodeEditor::SetDefault(selectedCodeEditor);
			}
		}

		if (
			p_logWarningOnFallback &&
			!p_preferredCodeEditor.empty() &&
			!fallbackCodeEditor.empty() &&
			fallbackCodeEditor != p_preferredCodeEditor
		)
		{
			OVLOG_WARNING("Code editor \"" + p_preferredCodeEditor + "\" is not supported. Falling back to \"" + fallbackCodeEditor + "\".");
		}

		SetOrAdd<std::string>(*editorSettingsFile, codeEditorSettingKey, selectedCodeEditor);
		editorSettingsFile->Rewrite();

		FillCodeEditorSelectorChoices(codeEditorSelector, *supportedCodeEditors, selectedCodeEditor);
		return selectedCodeEditor;
	};

	const std::string selectedCodeEditor = editorSettingsFile->GetOrDefault<std::string>(
		codeEditorSettingKey,
		std::string{ kDefaultCodeEditorName }
	);
	RefreshCodeEditorSelection(selectedCodeEditor, true);

	if (codeEditorSelector != nullptr)
	{
		codeEditorSelector->ValueChangedEvent += [supportedCodeEditors, RefreshCodeEditorSelection](int p_value)
		{
			if (p_value < 0 || p_value >= static_cast<int>(supportedCodeEditors->size()))
			{
				return;
			}

			std::string codeEditor = (*supportedCodeEditors)[static_cast<size_t>(p_value)];
			RefreshCodeEditorSelection(codeEditor, false);
		};
	}

	auto& customCodeEditorMenu = codeEditorMenu.CreateWidget<MenuList>("Add custom editor");
	customCodeEditorMenu.CreateWidget<Texts::Text>("Arguments placeholders:");
	customCodeEditorMenu.CreateWidget<Texts::Text>("{path} {path_windows} {path_unix}");

	auto& nameField = customCodeEditorMenu.CreateWidget<InputFields::InputText>("", "Name");
	auto& commandField = customCodeEditorMenu.CreateWidget<InputFields::InputText>("", "Command");
	auto& argumentsField = customCodeEditorMenu.CreateWidget<InputFields::InputText>(std::string{ kDefaultCodeEditorArguments }, "Arguments");

	auto& registerEditorButton = customCodeEditorMenu.CreateWidget<Buttons::Button>("Register");
	registerEditorButton.idleBackgroundColor = { 0.0f, 0.5f, 0.0f };

	auto& manageCustomCodeEditorsMenu = codeEditorMenu.CreateWidget<MenuList>("Manage custom editors");
	auto rebuildManageCustomEditorsMenu = std::make_shared<std::function<void()>>();
	const std::weak_ptr<std::function<void()>> weakRebuildManageCustomEditorsMenu = rebuildManageCustomEditorsMenu;
	const auto ScheduleManageCustomEditorsRefresh = [weakRebuildManageCustomEditorsMenu]
	{
		EDITOR_EXEC(DelayAction([weakRebuildManageCustomEditorsMenu]
		{
			if (const auto rebuildManageCustomEditorsMenu = weakRebuildManageCustomEditorsMenu.lock();
				rebuildManageCustomEditorsMenu && *rebuildManageCustomEditorsMenu)
			{
				(*rebuildManageCustomEditorsMenu)();
			}
		}, 0));
	};

	registerEditorButton.ClickedEvent += [editorSettingsFile, rebuildManageCustomEditorsMenu, RefreshCodeEditorSelection, ScheduleManageCustomEditorsRefresh, &nameField, &commandField, &argumentsField]
	{
		(void)rebuildManageCustomEditorsMenu; // Keeps the dynamic refresh callback alive for the menu lifetime.

		std::string editorName = nameField.content;
		std::string editorCommand = commandField.content;
		std::string editorArguments = argumentsField.content;

		OvTools::Utils::String::Trim(editorName);
		OvTools::Utils::String::Trim(editorCommand);
		OvTools::Utils::String::Trim(editorArguments);

		if (editorName.empty() || editorCommand.empty())
		{
			OVLOG_WARNING("Please enter both a code editor name and command before registering.");
			return;
		}

		if (IsBuiltInCodeEditorName(editorName))
		{
			OVLOG_WARNING("Built-in code editor names cannot be overridden. Please choose another name.");
			return;
		}

		if (editorArguments.empty())
		{
			editorArguments = std::string{ kDefaultCodeEditorArguments };
		}

		OvTools::Utils::CodeEditor::Register(editorName, editorCommand, editorArguments);

		UpsertCustomCodeEditor(*editorSettingsFile, CustomCodeEditor{ editorName, editorCommand, editorArguments });
		editorSettingsFile->Rewrite();
		RefreshCodeEditorSelection(editorName, false);
		ScheduleManageCustomEditorsRefresh();
		OVLOG_INFO("Code editor \"" + editorName + "\" has been registered.");

		nameField.content = "";
		commandField.content = "";
		argumentsField.content = std::string{ kDefaultCodeEditorArguments };
	};

	*rebuildManageCustomEditorsMenu = [editorSettingsFile, &manageCustomCodeEditorsMenu, RefreshCodeEditorSelection, ScheduleManageCustomEditorsRefresh]
	{
		manageCustomCodeEditorsMenu.RemoveAllWidgets();

		const auto customCodeEditors = LoadCustomCodeEditors(*editorSettingsFile);
		if (customCodeEditors.empty())
		{
			manageCustomCodeEditorsMenu.CreateWidget<Texts::Text>("No custom code editors");
			return;
		}

		for (const auto& customCodeEditor : customCodeEditors)
		{
			const std::string editorName = customCodeEditor.name;
			auto& customCodeEditorEntry = manageCustomCodeEditorsMenu.CreateWidget<MenuList>(editorName);

			auto& editorNameField = customCodeEditorEntry.CreateWidget<InputFields::InputText>(customCodeEditor.name, "Name");
			auto& editorCommandField = customCodeEditorEntry.CreateWidget<InputFields::InputText>(customCodeEditor.command, "Command");
			auto& editorArgumentsField = customCodeEditorEntry.CreateWidget<InputFields::InputText>(customCodeEditor.arguments, "Arguments");

			auto& saveButton = customCodeEditorEntry.CreateWidget<Buttons::Button>("Save");
			saveButton.lineBreak = false;
			saveButton.idleBackgroundColor = { 0.0f, 0.5f, 0.0f };

			auto& deleteButton = customCodeEditorEntry.CreateWidget<Buttons::Button>("Delete");
			deleteButton.idleBackgroundColor = { 0.5f, 0.0f, 0.0f };

			saveButton.ClickedEvent += [editorSettingsFile, editorName, RefreshCodeEditorSelection, ScheduleManageCustomEditorsRefresh, &editorNameField, &editorCommandField, &editorArgumentsField]
			{
				std::string newName = editorNameField.content;
				std::string newCommand = editorCommandField.content;
				std::string newArguments = editorArgumentsField.content;

				OvTools::Utils::String::Trim(newName);
				OvTools::Utils::String::Trim(newCommand);
				OvTools::Utils::String::Trim(newArguments);

				if (newName.empty() || newCommand.empty())
				{
					OVLOG_WARNING("Please enter both a code editor name and command before saving.");
					return;
				}

				if (newArguments.empty())
				{
					newArguments = std::string{ kDefaultCodeEditorArguments };
				}

				const bool nameChanged = NormalizeCodeEditorName(newName) != NormalizeCodeEditorName(editorName);
				if (nameChanged && IsBuiltInCodeEditorName(newName))
				{
					OVLOG_WARNING("Built-in code editor names cannot be overridden. Please choose another name.");
					return;
				}

				if (nameChanged)
				{
					RemoveCustomCodeEditor(*editorSettingsFile, editorName);

					if (!IsBuiltInCodeEditorName(editorName))
					{
						OvTools::Utils::CodeEditor::Unregister(editorName);
					}
				}

				OvTools::Utils::CodeEditor::Register(newName, newCommand, newArguments);
				UpsertCustomCodeEditor(*editorSettingsFile, CustomCodeEditor{ newName, newCommand, newArguments });
				editorSettingsFile->Rewrite();

				const std::string selectedProjectEditor = editorSettingsFile->GetOrDefault<std::string>(
					std::string{ kCodeEditorSettingKey },
					std::string{ kDefaultCodeEditorName }
				);

				const bool wasSelected = NormalizeCodeEditorName(selectedProjectEditor) == NormalizeCodeEditorName(editorName);
				RefreshCodeEditorSelection(wasSelected ? newName : selectedProjectEditor, false);
				ScheduleManageCustomEditorsRefresh();

				OVLOG_INFO("Code editor \"" + newName + "\" has been updated.");
			};

			deleteButton.ClickedEvent += [editorSettingsFile, editorName, RefreshCodeEditorSelection, ScheduleManageCustomEditorsRefresh]
			{
				if (!RemoveCustomCodeEditor(*editorSettingsFile, editorName))
				{
					OVLOG_WARNING("Failed to remove code editor \"" + editorName + "\" from editor settings.");
					return;
				}

				editorSettingsFile->Rewrite();

				if (!IsBuiltInCodeEditorName(editorName))
				{
					OvTools::Utils::CodeEditor::Unregister(editorName);
				}

				const std::string selectedProjectEditor = editorSettingsFile->GetOrDefault<std::string>(
					std::string{ kCodeEditorSettingKey },
					std::string{ kDefaultCodeEditorName }
				);
				const bool deletedSelected = NormalizeCodeEditorName(selectedProjectEditor) == NormalizeCodeEditorName(editorName);

				RefreshCodeEditorSelection(selectedProjectEditor, deletedSelected);
				ScheduleManageCustomEditorsRefresh();
				OVLOG_INFO("Code editor \"" + editorName + "\" has been removed.");
			};
		}
	};

	(*rebuildManageCustomEditorsMenu)();

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

	auto& fontSizeMenu = m_settingsMenu->CreateWidget<MenuList>("Font Size");
	auto& fontSizeSelector = fontSizeMenu.CreateWidget<Selection::ComboBox>(static_cast<int>(Settings::EditorSettings::FontSize.Get()));
	fontSizeSelector.choices = {
		{ static_cast<int>(Settings::EFontSize::SMALL), "Small"},
		{ static_cast<int>(Settings::EFontSize::MEDIUM), "Medium"},
		{ static_cast<int>(Settings::EFontSize::BIG), "Big"}
	};
	fontSizeSelector.ValueChangedEvent += [this](int p_value) {
		Settings::EditorSettings::FontSize = p_value;
		const auto fontID = std::string{ Settings::GetFontID(static_cast<Settings::EFontSize>(p_value)) };
		EDITOR_CONTEXT(uiManager)->UseFont(fontID);
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
	auto& reflectionProbesScaleSlider = sceneViewBillboardScaleMenu.CreateWidget<Sliders::SliderInt>(0, 100, static_cast<int>(Settings::EditorSettings::ReflectionProbeScale * 100.0f), OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Reflection Probes");
	reflectionProbesScaleSlider.ValueChangedEvent += [this](int p_value) { Settings::EditorSettings::ReflectionProbeScale = p_value / 100.0f; };
	reflectionProbesScaleSlider.format = "%d %%";

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
	
	auto& consoleSettingsMenu = m_settingsMenu->CreateWidget<MenuList>("Console Settings");
	auto& consoleMaxLogsSlider = consoleSettingsMenu.CreateWidget<OvUI::Widgets::Sliders::SliderInt>(1, 1000, Settings::EditorSettings::ConsoleMaxLogs.Get(), OvUI::Widgets::Sliders::ESliderOrientation::HORIZONTAL, "Max Logs");
	consoleMaxLogsSlider.ValueChangedEvent += [this](int p_value) { 
		Settings::EditorSettings::ConsoleMaxLogs = p_value;
		EDITOR_PANEL(Panels::Console, "Console").TruncateLogs();
	};

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

void OvEditor::Panels::MenuBar::CreateToolsMenu()
{
// No Tracy profiler (front-end) on non-Windows platforms at the moment.
// You'll need to build it yourself if you want to use it on other platforms.
// https://github.com/Overload-Technologies/Overload/issues/614
#ifdef _WIN32
	auto& toolsMenu = CreateWidget<MenuList>("Tools");
	toolsMenu.CreateWidget<MenuItem>("Open Profiler").ClickedEvent += EDITOR_BIND(OpenProfiler);
#endif
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
	const std::string repoURL = "https://github.com/Overload-Technologies/Overload";

	auto& helpMenu = CreateWidget<MenuList>("Help");
	helpMenu.CreateWidget<MenuItem>("GitHub").ClickedEvent += [repoURL] {OvTools::Utils::SystemCalls::OpenURL(repoURL); };
	helpMenu.CreateWidget<MenuItem>("Wiki").ClickedEvent += [repoURL] {OvTools::Utils::SystemCalls::OpenURL(repoURL + "/wiki"); };
	helpMenu.CreateWidget<MenuItem>("API Reference").ClickedEvent += [repoURL] {
		// FIXME: Workaround to be removed once the version following 1.8 is released.
		// This ensures the first few commits before the next release still have the "API Reference"
		// button point to a valid URL.
		const std::string tag =
			std::string(OVERLOAD_VERSION) == "1.8" ?
			"main" :
			"v" + std::string(OVERLOAD_VERSION);

		OvTools::Utils::SystemCalls::OpenURL(repoURL + std::format("/tree/{}/API", tag));
	};
	helpMenu.CreateWidget<Visual::Separator>();
	helpMenu.CreateWidget<MenuItem>("Bug Report").ClickedEvent += [repoURL] {OvTools::Utils::SystemCalls::OpenURL(repoURL + "/issues/new?assignees=&labels=Bug&template=bug_report.md&title="); };
	helpMenu.CreateWidget<MenuItem>("Feature Request").ClickedEvent += [repoURL] {OvTools::Utils::SystemCalls::OpenURL(repoURL + "/issues/new?assignees=&labels=Feature&template=feature_request.md&title="); };
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

