#pragma once

// Determine available C++ features.
#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
#define STL_BYTESWAP_AVAILABLE 1
#endif

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
#define STL_BITOPS_AVAILABLE 1
#endif

#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L
#define STL_ENDIAN_AVAILABLE 1
#endif

// The THREADSAFE marker is used to indicate that the code is safe to use in a multithreaded
// context.
#define THREADSAFE

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

// Determine word size.
#if defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8
#define PLATFORM_WORD_SIZE 8
#elif defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4
#define PLATFORM_WORD_SIZE 4
#else
#error "Cannot determine platform word size"
#endif

#include <limits>

namespace AbstractPlatform {
static constexpr size_t kPlatformWordSize     = PLATFORM_WORD_SIZE;
static constexpr size_t kPlatformLongLongSize = sizeof(long long);
static constexpr size_t kWordAlignment        = PLATFORM_WORD_SIZE;
static constexpr size_t kNoAlignment          = std::numeric_limits<size_t>::max();

inline bool IsAligned(const void* aPtr, size_t aAlignment) NOEXCEPT
{
  return (reinterpret_cast<std::uintptr_t>(aPtr) % aAlignment) == 0;
}

constexpr bool IsAlignmentAware(size_t aAlignment) NOEXCEPT
{
  return (aAlignment != kNoAlignment);
}

} // namespace AbstractPlatform
