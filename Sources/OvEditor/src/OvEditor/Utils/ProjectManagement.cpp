/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <fstream>

#include <OvEditor/Core/Application.h>
#include <OvEditor/Utils/ProjectManagement.h>
#include <OvEditor/Utils/FileSystem.h>
#include <OvWindowing/Dialogs/MessageBox.h>

namespace
{
	std::filesystem::path _FormatPath(const std::filesystem::path& p_path)
	{
		auto formattedPath = std::filesystem::absolute(p_path).make_preferred();

		// Remove trailing separator if it exists
		if (formattedPath.string().ends_with(std::filesystem::path::preferred_separator))
		{
			formattedPath = formattedPath.parent_path();
		}

		return formattedPath;
	}

	std::optional<std::filesystem::path> _FindProjectFileInDirectory(const std::filesystem::path& p_path)
	{
		for (const auto& entry : std::filesystem::directory_iterator(p_path))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".ovproject")
			{
				return entry.path();
			}
		}

		return std::nullopt;
	}
}

namespace OvEditor::Utils::ProjectManagement
{

	class ProjectRegistry
	{
	public:
		ProjectRegistry(const std::filesystem::path& p_path) : m_path(p_path)
		{
			_Load();
		}

		void Add(const std::filesystem::path& p_path)
		{
			m_projects.insert(_FormatPath(p_path));
		}

		void Remove(const std::filesystem::path& p_path)
		{
			m_projects.erase(_FormatPath(p_path));
		}

		void Save()
		{
			_CreateDirectories();

			std::ofstream registry(m_path, std::ios::trunc);

			if (registry)
			{
				for (const auto& path : m_projects)
				{
					registry << path.string() << '\n';
				}
			}
		}

		const std::unordered_set<std::filesystem::path>& GetProjects() const
		{
			return m_projects;
		}

	private:
		void _CreateDirectories()
		{
			std::filesystem::create_directories(m_path.parent_path());
		}

		void _Load()
		{
			_CreateDirectories();

			if (std::ifstream registry(FileSystem::kProjectRegistryFilePath); registry)
			{
				std::string line;
				while (std::getline(registry, line))
				{
					if (!line.empty())
					{
						Add(line);
					}
				}
			}
		}

	private:
		std::filesystem::path m_path;
		std::unordered_set<std::filesystem::path> m_projects;
	};

	std::unordered_set<std::filesystem::path> GetRegisteredProjects()
	{
		return ProjectRegistry{ FileSystem::kProjectRegistryFilePath }.GetProjects();
	}

	void SanitizeProjectRegistry()
	{
		ProjectRegistry registry{ FileSystem::kProjectRegistryFilePath };
		auto projects = registry.GetProjects();
		for (const auto& project : projects)
		{
			if (!IsValidProjectDirectory(project))
			{
				registry.Remove(project);
			}
		}
		registry.Save();
	}

	std::filesystem::path GetProjectFile(const std::filesystem::path& p_projectFolder)
	{
		const auto projectFile = _FindProjectFileInDirectory(p_projectFolder);
		OVASSERT(projectFile.has_value(), "Project file not found in the given directory");
		return projectFile.value();
	}

	void RegisterProject(const std::filesystem::path& p_path)
	{
		ProjectRegistry registry{ FileSystem::kProjectRegistryFilePath };
		registry.Add(p_path);
		registry.Save();
	}

	void UnregisterProject(const std::filesystem::path& p_path)
	{
		ProjectRegistry registry{ FileSystem::kProjectRegistryFilePath };
		registry.Remove(p_path);
		registry.Save();
	}

	bool IsValidProjectDirectory(const std::filesystem::path& p_path)
	{
		const auto directory = _FormatPath(p_path);

		return
			std::filesystem::exists(directory) &&
			_FindProjectFileInDirectory(directory) != std::nullopt;
	}

	bool CreateProject(const std::filesystem::path& p_projectPath)
	{
		const auto projectPath = _FormatPath(p_projectPath);

		if (!std::filesystem::exists(p_projectPath))
		{
			if (!std::filesystem::create_directory(p_projectPath))
			{
				return false;
			}

			if (!std::filesystem::create_directory(p_projectPath / "Assets"))
			{
				return false;
			}

			if (!std::filesystem::create_directory(p_projectPath / "Scripts"))
			{
				return false;
			}

			const auto projectFile = p_projectPath / std::format("{}.ovproject", p_projectPath.stem().string());

			std::ofstream{ projectFile };

			if (!std::filesystem::exists(projectFile))
			{
				return false;
			}

			return true;
		}

		return false;
	}

	void OpenProject(const std::filesystem::path& p_projectPath)
	{
		// Find the project file (*.ovproject) in the given path.
		// It can be named anything, but it should have the .ovproject extension.
		auto findProjectFile = [](const std::filesystem::path& p_path) -> std::optional<std::filesystem::path> {
			for (const auto& entry : std::filesystem::directory_iterator(p_path))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".ovproject")
				{
					return entry.path();
				}
			}
			return std::nullopt;
		};

		const auto projectFile = findProjectFile(p_projectPath);

		if (projectFile)
		{
			OvEditor::Core::Application app(projectFile.value());
			app.Run();
		}
		else
		{
			OvWindowing::Dialogs::MessageBox errorMessage(
				"Project not found",
				"The selected project does not exist",
				OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
				OvWindowing::Dialogs::MessageBox::EButtonLayout::OK
			);
		}
	}
}
