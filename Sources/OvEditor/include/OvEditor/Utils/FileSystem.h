#pragma once
/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>

#include <OvTools/Utils/SystemCalls.h>

namespace OvEditor::Utils::FileSystem
{
	const std::filesystem::path kAppdataPath = OvTools::Utils::SystemCalls::GetPathToAppdata();
	const std::filesystem::path kEditorDataPath = kAppdataPath / "OverloadTech" / "OvEditor";
	const std::filesystem::path kLayoutFilePath = kEditorDataPath / "layout.ini";
	const std::filesystem::path kSettingsFilePath = kEditorDataPath / "settings.ini";
	const std::filesystem::path kProjectRegistryFilePath = kEditorDataPath / "projects.ini";
}
