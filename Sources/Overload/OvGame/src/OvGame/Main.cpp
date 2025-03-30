/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Utils/Defines.h>

#include "OvGame/Core/Application.h"

FORCE_DEDICATED_GPU

#ifdef _DEBUG
int main()
#else
#undef APIENTRY
#include "Windows.h"
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#endif
{
	OvGame::Core::Application app;
	app.Run();

	return EXIT_SUCCESS;
}