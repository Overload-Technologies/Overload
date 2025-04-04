/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdlib>

#include <tracy/Tracy.hpp>

#if defined(TRACY_ENABLE)
#define TRACY_CUSTOM_NEW_ALLOCATOR			\
void* operator new(std::size_t count)		\
{											\
	auto ptr = malloc(count);				\
	TracyAlloc(ptr, count);					\
	return ptr;								\
}

#define TRACY_CUSTOM_DELETE_ALLOCATOR		\
void operator delete(void* ptr) noexcept	\
{											\
	TracyFree(ptr);							\
	free(ptr);								\
}

#define TRACY_CUSTOM_NEW_ARRAY_ALLOCATOR	\
void* operator new[](std::size_t count)		\
{											\
	auto ptr = malloc(count);				\
	TracyAlloc(ptr, count);					\
	return ptr;								\
}

#define TRACY_CUSTOM_DELETE_ARRAY_ALLOCATOR \
void operator delete[](void* ptr) noexcept	\
{											\
	TracyFree(ptr);							\
	free(ptr);								\
}
#else
#define TRACY_CUSTOM_NEW_ALLOCATOR
#define TRACY_CUSTOM_DELETE_ALLOCATOR
#define TRACY_CUSTOM_NEW_ARRAY_ALLOCATOR
#define TRACY_CUSTOM_DELETE_ARRAY_ALLOCATOR
#endif
