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
#include <OvEditor/Utils/ProjectManagement.h>

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

	bool TryRun(const std::filesystem::path& projectPath)
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
			return true;
		}

		return false;
	}
}

int main(int argc, char** argv)
{
	UpdateWorkingDirectory(argv[0]);

	OvEditor::Settings::EditorSettings::Load();

	std::optional<std::filesystem::path> projectPath;

	if (argc < 2)
	{
		// No project file given as argument ==> Open the ProjectHub
		OvEditor::Core::ProjectHub hub;

		if (auto result = hub.Run())
		{
			projectPath = result->projectPath;
		}
	}
	else
	{
		// Project file given as argument ==> Open the project
		projectPath = argv[1];
	}

	if (projectPath)
	{
		if (!OvEditor::Utils::ProjectManagement::IsValidProjectDirectory(projectPath.value()))
		{
			// Invalid project path
			return EXIT_FAILURE;
		}

		// Make sure the project is registered in the project registry
		OvEditor::Utils::ProjectManagement::RegisterProject(projectPath.value());

		if (!TryRun(projectPath.value()))
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

#ifndef _DEBUG
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	main(__argc, __argv);
}
#endif
