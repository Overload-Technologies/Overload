/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvGame/Core/Application.h>
#include <OvRendering/Utils/Defines.h>
#include <OvTools/Profiling/TracyAllocators.h>

FORCE_DEDICATED_GPU

#ifdef _DEBUG
int main()
#else
	#ifdef _WIN32
		#undef APIENTRY
		#include "Windows.h"
		INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
	#else
		int main()
	#endif
#endif
{
	OvGame::Core::Application app;
	app.Run();

	return EXIT_SUCCESS;
}