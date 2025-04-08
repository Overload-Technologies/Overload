/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <tracy/Tracy.hpp>

#define FrameEnd FrameMark
#define CPUZone ZoneScoped
#define CPUZoneN(...) ZoneScopedN(__VA_ARGS__)
