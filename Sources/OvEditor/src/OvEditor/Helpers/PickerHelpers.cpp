/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Helpers/PickerHelpers.h>

using namespace OvTools::Utils;

namespace
{
	void CollectFromDirectory(
		OvCore::Helpers::GUIDrawer::PickerItemList& p_list,
		const std::filesystem::path& p_directory,
		bool p_isEngine,
		PathParser::EFileType p_fileType,
		const std::function<void(std::string)>& p_onSelected)
	{
		if (!std::filesystem::exists(p_directory))
			return;

		std::error_code ec;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(
			p_directory, std::filesystem::directory_options::skip_permission_denied, ec))
		{
			if (!entry.is_regular_file())
				continue;

			const std::string path = entry.path().string();
			const PathParser::EFileType fileType = PathParser::GetFileType(path);

			if (fileType == PathParser::EFileType::UNKNOWN)
				continue;

			if (p_fileType != PathParser::EFileType::UNKNOWN && fileType != p_fileType)
				continue;

			const std::string resourcePath = EDITOR_EXEC(GetResourcePath(path, p_isEngine));
			const std::string filename = PathParser::GetElementName(resourcePath);
			const uint32_t iconID = EDITOR_CONTEXT(editorResources)->GetFileIcon(path)->GetTexture().GetID();

			p_list.Add({
				resourcePath,
				filename,
				resourcePath,
				iconID,
				[onSelected = p_onSelected, resourcePath] { onSelected(resourcePath); }
			});
		}
	}
}

void OvEditor::Helpers::PickerHelpers::AddFileItems(
	OvCore::Helpers::GUIDrawer::PickerItemList& p_list,
	PathParser::EFileType p_fileType,
	std::function<void(std::string)> p_onSelected,
	bool p_searchProject,
	bool p_searchEngine)
{
	if (p_searchProject)
		CollectFromDirectory(p_list, EDITOR_CONTEXT(projectAssetsPath), false, p_fileType, p_onSelected);

	if (p_searchEngine)
		CollectFromDirectory(p_list, EDITOR_CONTEXT(engineAssetsPath), true, p_fileType, p_onSelected);
}
