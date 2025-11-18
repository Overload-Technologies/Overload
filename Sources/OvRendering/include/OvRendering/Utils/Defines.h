/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#ifdef _WIN32
#define FORCE_DEDICATED_GPU \
extern "C"\
{\
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;\
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;\
}
#else
// Linux version - use visibility attribute instead of dllexport
#define FORCE_DEDICATED_GPU \
extern "C"\
{\
	__attribute__((visibility("default"))) unsigned long NvOptimusEnablement = 0x00000001;\
	__attribute__((visibility("default"))) int AmdPowerXpressRequestHighPerformance = 1;\
}
#endif
