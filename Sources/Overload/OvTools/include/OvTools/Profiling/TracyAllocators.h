/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <tracy/Tracy.hpp>

// Platform-specific aligned allocations
#if defined(_WIN32)
#include <malloc.h>
#define CUSTOM_ALIGNED_ALLOC(alignment, size) _aligned_malloc(size, alignment)
#define CUSTOM_ALIGNED_FREE(ptr) _aligned_free(ptr)
#else
// For POSIX systems
#include <cstdlib>
#define CUSTOM_ALIGNED_ALLOC(alignment, size) aligned_alloc(alignment, size)
#define CUSTOM_ALIGNED_FREE(ptr) free(ptr)
#endif

#if defined(TRACY_ENABLE) && defined(TRACY_MEMORY_ENABLE)
void* operator new(std::size_t count)
{
	auto ptr = malloc(count);
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

void* operator new[](std::size_t count)
{
	auto ptr = malloc(count);
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete[](void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

void operator delete(void* ptr, std::size_t count) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

void operator delete[](void* ptr, std::size_t count) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

void* operator new(std::size_t count, std::align_val_t alignment)
{
	size_t alignedSize = (count + static_cast<size_t>(alignment) - 1) & ~(static_cast<size_t>(alignment) - 1);
	auto ptr = CUSTOM_ALIGNED_ALLOC(static_cast<size_t>(alignment), alignedSize);
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete(void* ptr, std::align_val_t) noexcept
{
	TracyFree(ptr);
	CUSTOM_ALIGNED_FREE(ptr);
}

void* operator new[](std::size_t count, std::align_val_t alignment)
{
	size_t alignedSize = (count + static_cast<size_t>(alignment) - 1) & ~(static_cast<size_t>(alignment) - 1);
	auto ptr = CUSTOM_ALIGNED_ALLOC(static_cast<size_t>(alignment), alignedSize);
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete[](void* ptr, std::align_val_t) noexcept
{
	TracyFree(ptr);
	CUSTOM_ALIGNED_FREE(ptr);
}

void operator delete(void* ptr, std::size_t count, std::align_val_t) noexcept
{
	TracyFree(ptr);
	CUSTOM_ALIGNED_FREE(ptr);
}

void operator delete[](void* ptr, std::size_t count, std::align_val_t) noexcept
{
	TracyFree(ptr);
	CUSTOM_ALIGNED_FREE(ptr);
}
#endif // defined(TRACY_ENABLE) && defined(TRACY_MEMORY_ENABLE)
