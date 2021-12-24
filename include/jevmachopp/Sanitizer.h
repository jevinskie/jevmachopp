#pragma once

#include "jevmachopp/Common.h"

#if __has_include(<sanitizer/asan_interface.h>)

#include <sanitizer/asan_interface.h>

#else

extern "C" {

void __asan_poison_memory_region(void const volatile *addr, size_t size);

void __asan_unpoison_memory_region(void const volatile *addr, size_t size);

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define ASAN_POISON_MEMORY_REGION(addr, size) __asan_poison_memory_region((addr), (size))

#define ASAN_UNPOISON_MEMORY_REGION(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#define ASAN_POISON_MEMORY_REGION(addr, size) ((void)(addr), (void)(size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) ((void)(addr), (void)(size))
#endif

} // extern "C"

#endif
