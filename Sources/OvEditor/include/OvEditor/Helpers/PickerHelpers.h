/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <functional>
#include <string>

#include <OvCore/Helpers/GUIHelpers.h>
#include <OvTools/Utils/PathParser.h>

namespace OvEditor::Helpers::PickerHelpers
{
	/**
	* Scan the project and/or engine asset directories for files of the given type
	* and append a PickerItem for each one to p_list.
	* Items whose key (resource path) is already present in the list are skipped.
	*
	* @param p_list             Target list to append to
	* @param p_fileType         File type filter (UNKNOWN = all known types)
	* @param p_onSelected       Called with the resource path when the item is selected
	* @param p_searchProject    Include project assets
	* @param p_searchEngine     Include engine assets
	*/
	void AddFileItems(
		OvCore::Helpers::GUIHelpers::PickerItemList& p_list,
		OvTools::Utils::PathParser::EFileType p_fileType,
		std::function<void(std::string)> p_onSelected,
		bool p_searchProject = true,
		bool p_searchEngine = true
	);
}
