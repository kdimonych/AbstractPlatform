#pragma once

// Determine the whether the platform supports exceptions or not.
#ifdef __EXCEPTIONS
#define NOEXCEPT noexcept
#include <exception>
#else
#define NOEXCEPT
#endif

// Determine the platform endianness.
#if defined(__PLATFORM_BIG_ENDIAN)
#define PLATFORM_BIG_ENDIAN
#elif defined(__PLATFORM_LITTLE_ENDIAN)
#define PLATFORM_LITTLE_ENDIAN
#else
// If the platform endianness is not defined, we try to determine it at compile time.
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define PLATFORM_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define PLATFORM_BIG_ENDIAN
#else
#error "Cannot determine platform endianness"
#endif
#endif
