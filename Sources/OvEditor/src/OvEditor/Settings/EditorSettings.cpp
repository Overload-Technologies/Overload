/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvEditor/Settings/EditorSettings.h>
#include <OvEditor/Utils/FileSystem.h>
#include <OvTools/Filesystem/IniFile.h>

template<class T>
void LoadIniEntry(OvTools::Filesystem::IniFile& iniFile, const std::string& entry, OvEditor::Settings::EditorSettings::Property<T>& out)
{
	if (iniFile.IsKeyExisting(entry))
	{
		out = iniFile.Get<T>(entry);
	}
}

template<class T>
void SetOrAddIniEntry(OvTools::Filesystem::IniFile& iniFile, const std::string& entry, const T& value)
{
	if (!iniFile.Set<T>(entry, value))
	{
		iniFile.Add<T>(entry, value);
	}
}

OvTools::Filesystem::IniFile GetEditorIniFile()
{
	std::filesystem::create_directories(OvEditor::Utils::FileSystem::kEditorDataPath);
	const auto filePath = OvEditor::Utils::FileSystem::kEditorDataPath / "editor.ini";

	return OvTools::Filesystem::IniFile{ filePath.string() };
}

void OvEditor::Settings::EditorSettings::Save()
{
	OvTools::Filesystem::IniFile iniFile = GetEditorIniFile();

	SetOrAddIniEntry<bool>(iniFile, "show_geometry_bounds", ShowGeometryBounds.Get());
	SetOrAddIniEntry<bool>(iniFile, "show_light_bounds", ShowLightBounds.Get());
	SetOrAddIniEntry<bool>(iniFile, "editor_frustum_geometry_culling", EditorFrustumGeometryCulling.Get());
	SetOrAddIniEntry<bool>(iniFile, "editor_frustum_light_culling", EditorFrustumLightCulling.Get());
	SetOrAddIniEntry<float>(iniFile, "light_billboard_scale", LightBillboardScale.Get());
	SetOrAddIniEntry<float>(iniFile, "reflection_probe_scale", ReflectionProbeScale.Get());
	SetOrAddIniEntry<float>(iniFile, "translation_snap_unit", TranslationSnapUnit.Get());
	SetOrAddIniEntry<float>(iniFile, "rotation_snap_unit", RotationSnapUnit.Get());
	SetOrAddIniEntry<float>(iniFile, "scaling_snap_unit", ScalingSnapUnit.Get());
	SetOrAddIniEntry<int>(iniFile, "color_theme", ColorTheme.Get());
	SetOrAddIniEntry<int>(iniFile, "console_max_logs", ConsoleMaxLogs.Get());
	SetOrAddIniEntry<int>(iniFile, "font_size", FontSize.Get());
	iniFile.Rewrite();
}

void OvEditor::Settings::EditorSettings::Load()
{
	OvTools::Filesystem::IniFile iniFile = GetEditorIniFile();

	LoadIniEntry<bool>(iniFile, "show_geometry_bounds", ShowGeometryBounds);
	LoadIniEntry<bool>(iniFile, "show_light_bounds", ShowLightBounds);
	LoadIniEntry<bool>(iniFile, "show_geometry_frustum_culling_in_scene_view", EditorFrustumGeometryCulling);
	LoadIniEntry<bool>(iniFile, "show_light_frustum_culling_in_scene_view", EditorFrustumLightCulling);
	LoadIniEntry<float>(iniFile, "light_billboard_scale", LightBillboardScale);
	LoadIniEntry<float>(iniFile, "reflection_probe_scale", ReflectionProbeScale);
	LoadIniEntry<float>(iniFile, "translation_snap_unit", TranslationSnapUnit);
	LoadIniEntry<float>(iniFile, "rotation_snap_unit", RotationSnapUnit);
	LoadIniEntry<float>(iniFile, "scaling_snap_unit", ScalingSnapUnit);
	LoadIniEntry<int>(iniFile, "color_theme", ColorTheme);
	LoadIniEntry<int>(iniFile, "console_max_logs", ConsoleMaxLogs);
	LoadIniEntry<int>(iniFile, "font_size", FontSize);
}

