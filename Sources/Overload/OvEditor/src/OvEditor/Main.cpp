/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvTools/Utils/String.h>

#include <OvRendering/Utils/Defines.h>

#include "OvEditor/Core/Application.h"
#include "OvEditor/Core/ProjectHub.h"

#undef APIENTRY
#include "Windows.h"

FORCE_DEDICATED_GPU

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

int main(int argc, char** argv);
static void TryRun(const std::string& projectPath, const std::string& projectName);

#ifndef _DEBUG
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	main(__argc, __argv);
}
#endif

int main(int argc, char** argv)
{
	UpdateWorkingDirectory(argv[0]);

	bool ready = false;
	std::string projectPath;
	std::string projectName;

	{
		OvEditor::Core::ProjectHub hub;

		if (argc < 2)
		{
			// No project file given as argument ==> Open the ProjectHub
			std::tie(ready, projectPath, projectName) = hub.Run();
		}
		else
		{
			// Project file given as argument ==> Open the project
			std::string projectFile = argv[1];

			if (OvTools::Utils::PathParser::GetExtension(projectFile) == "ovproject")
			{
				ready = true;
				projectPath = OvTools::Utils::PathParser::GetContainingFolder(projectFile);
				projectName = OvTools::Utils::PathParser::GetElementName(projectFile);
				OvTools::Utils::String::Replace(projectName, ".ovproject", "");
			}

			hub.RegisterProject(projectPath);
		}
	}

	if (ready)
		TryRun(projectPath, projectName);

	return EXIT_SUCCESS;
}

static void TryRun(const std::string& projectPath, const std::string& projectName)
{
	auto errorEvent =
		[](OvWindowing::Context::EDeviceError, std::string errMsg)
		{
			errMsg = "Overload requires OpenGL 4.5 or newer.\r\n" + errMsg;
			MessageBox(0, errMsg.c_str(), "Overload", MB_OK | MB_ICONSTOP);
		};

	std::unique_ptr<OvEditor::Core::Application> app;

	try
	{
		auto listenerId = OvWindowing::Context::Device::ErrorEvent += errorEvent;
		app = std::make_unique<OvEditor::Core::Application>(projectPath, projectName);
		OvWindowing::Context::Device::ErrorEvent -= listenerId;
	}
	catch (...) {}

	if (app)
		app->Run();
}
