/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <random>

namespace OvTools::Utils
{
	/**
	* Generates a random 64-bit globally unique identifier.
	* Uses a thread-local Mersenne Twister seeded from std::random_device.
	*/
	inline uint64_t GenerateGUID()
	{
		thread_local std::mt19937_64 engine{ std::random_device{}() };
		return engine();
	}
}
