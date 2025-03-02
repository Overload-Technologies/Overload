/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#define FORCE_DEDICATED_GPU \
extern "C"\
{\
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;\
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;\
}
