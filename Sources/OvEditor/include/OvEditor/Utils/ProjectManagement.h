/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>

namespace OvEditor::Utils::ProjectManagement
{
	/**
	* Return a set with all registered projects
	*/
	std::unordered_set<std::filesystem::path> GetRegisteredProjects();

	/**
	* Remove all invalid projects from the registry
	*/
	void SanitizeProjectRegistry();

	/**
	* Return the path to the project file given the project folder
	* @note This function should never fail, make sure to call IsValidProjectDirectory before.
	* @param p_projectFolder
	*/
	std::filesystem::path GetProjectFile(const std::filesystem::path& p_projectFolder);

	/**
	* Add the given project to the registry
	* @param p_path
	*/
	void RegisterProject(const std::filesystem::path& p_path);

	/**
	* Remove the given project from the registry
	* @param p_path
	*/
	void UnregisterProject(const std::filesystem::path& p_path);

	/**
	* Return true if the given path is a valid project path
	* @param p_path
	*/
	bool IsValidProjectDirectory(const std::filesystem::path& p_path);

	/**
	* Create a project folder with the given name at the given path
	* @param p_projectPath
	*/
	bool CreateProject(const std::filesystem::path& p_projectPath);

	/**
	* Open the given project
	*/
	void OpenProject(const std::filesystem::path& p_projectPath);
}
