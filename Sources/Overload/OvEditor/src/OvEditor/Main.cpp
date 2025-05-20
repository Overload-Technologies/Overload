/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <fstream>

#include <OvEditor/Core/Application.h>
#include <OvEditor/Core/ProjectHub.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvEditor/Utils/FileSystem.h>

#include <OvRendering/Utils/Defines.h>

#include <OvTools/Profiling/TracyAllocators.h>
#include <OvTools/Utils/String.h>

#undef APIENTRY
#include "Windows.h"

FORCE_DEDICATED_GPU

namespace
{
	/**
	* When Overload is launched from a project file, we should consider the executable path as
	* the current working directory
	* @param p_executablePath
	*/
	void UpdateWorkingDirectory(const std::string& p_executablePath)
	{
		if (!IsDebuggerPresent())
		{
			std::filesystem::current_path(OvTools::Utils::PathParser::GetContainingFolder(p_executablePath));
		}
	}

	void RegisterProject(const std::filesystem::path& p_path)
	{
		bool pathAlreadyRegistered = false;

		{
			std::string line;
			std::ifstream myfile(OvEditor::Utils::FileSystem::kProjectRegistryFilePath);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (line == p_path)
					{
						pathAlreadyRegistered = true;
						break;
					}
				}
				myfile.close();
			}
		}

		if (!pathAlreadyRegistered)
		{
			std::ofstream projectsFile(OvEditor::Utils::FileSystem::kProjectRegistryFilePath, std::ios::app);
			projectsFile << p_path.string() << std::endl;
		}
	}

	void TryRun(const std::filesystem::path& projectPath)
	{
		const auto errorEvent = [](OvWindowing::Context::EDeviceError, std::string errMsg) {
			errMsg = "Overload requires OpenGL 4.5 or newer.\r\n" + errMsg;
			MessageBox(0, errMsg.c_str(), "Overload", MB_OK | MB_ICONSTOP);
		};

		std::unique_ptr<OvEditor::Core::Application> app;

		try
		{
			auto listenerId = OvWindowing::Context::Device::ErrorEvent += errorEvent;
			app = std::make_unique<OvEditor::Core::Application>(projectPath);
			OvWindowing::Context::Device::ErrorEvent -= listenerId;
		}
		catch (...) {}

		if (app)
		{
			app->Run();
		}
	}
}

int main(int argc, char** argv)
{
	UpdateWorkingDirectory(argv[0]);

	OvEditor::Settings::EditorSettings::Load();

	std::optional<OvEditor::Core::ProjectHubResult> projectHubResult;

	{
		OvEditor::Core::ProjectHub hub;

		if (argc < 2)
		{
			// No project file given as argument ==> Open the ProjectHub
			projectHubResult = hub.Run();
		}
		else
		{
			// Project file given as argument ==> Open the project
			std::filesystem::path projectFile = argv[1];

			if (std::filesystem::exists(projectFile))
			{
				if (std::filesystem::is_directory(projectFile))
				{
					// Find the first .ovproject file in the directory
				}
				else if (projectFile.extension() == ".ovproject")
				{
					projectHubResult = {
						.projectPath = projectFile
					};
				}
			}
		}
	}

	if (projectHubResult.has_value())
	{
		// Make sure the project is registered in the project registry
		RegisterProject(projectHubResult->projectPath);
		TryRun(projectHubResult->projectPath);
	}

	return EXIT_SUCCESS;
}

#ifndef _DEBUG
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	main(__argc, __argv);
}
#endif
