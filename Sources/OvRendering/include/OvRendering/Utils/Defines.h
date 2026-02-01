/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdlib>

#ifdef _WIN32
#define FORCE_DEDICATED_GPU \
extern "C"\
{\
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;\
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;\
}
#else
// Linux version - use setenv
#define FORCE_DEDICATED_GPU \
{\
	setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);\
    setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);\
    setenv("__VK_LAYER_NV_optimus", "NVIDIA_only", 1);\
	setenv("DRI_PRIME", "1", 1);\
}
#endif
