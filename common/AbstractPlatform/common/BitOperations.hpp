#pragma once

#include <AbstractPlatform/common/Platform.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>

#ifdef STL_BITOPS_AVAILABLE
#include <bit>
#endif

namespace AbstractPlatform {

static constexpr size_t kBitsPerByte = 8;

#ifndef STL_ENDIAN_AVAILABLE
/**
 * @brief Determines scalar type endianness
 *
 *  The 32 bit value a = 0x0A0B0C0D will be arranged in the memory the following way:
 */
enum class Endian
{
  Little = 0, // a[0] = 0D, a[1] = 0C, ... a[3] = 0A
  Big    = 1, // a[0] = 0A, a[1] = 0B, ... a[3] = 0D
// Pick current platform native endianness
#if defined(PLATFORM_BIG_ENDIAN)
  Native = Big
#elif defined(PLATFORM_LITTLE_ENDIAN)
  Native = Little
#else
#error "Cannot determine platform endianness"
#endif // defined(PLATFORM_BIG_ENDIAN)
};
#else  // STL_ENDIAN_AVAILABLE
using Endian = std::endian;
#endif // STL_ENDIAN_AVAILABLE

#ifdef STL_BYTESWAP_AVAILABLE
template <typename taT>
[[deprecated("ByteSwap is deprecated. Use std::byteswap instead.")]]
constexpr auto ByteSwap<taT> = std::byteswap<taT>;
#else
// Implementation of the ByteSwap function for non-integral types.
template <typename taT, std::enable_if_t<std::is_integral<taT>::value, int> = 0>
static constexpr taT ByteSwap(taT aValue) NOEXCEPT
{
  static_assert(std::has_unique_object_representations_v<taT>, "taT may not have padding bits");

  // TODO: Reimlement more optimized version of the ByteSwap function
  // for constexp usage.
  using TProxyArray = std::uint8_t (&)[sizeof(taT)];
  auto& byteArray   = reinterpret_cast<TProxyArray&>(aValue);
  std::reverse(std::begin(byteArray), std::end(byteArray));
  return aValue;
}
#endif

template <typename taValue>
inline static constexpr auto BitSize(taValue)
{
  return sizeof(taValue) * kBitsPerByte;
}

/**
 * @brief Returns the size of a buffer minimally required to fit aBit bits.
 *
 * @param aBits The bit number;
 * @return size_t The size of a buffer minimally required to fit aBit bits.
 */
inline static constexpr size_t BufferSize(size_t aBits)
{
  constexpr size_t kMaxBitsPerByteValue = (std::numeric_limits<size_t>::max() - kBitsPerByte) + 1;
  assert(aBits <= kMaxBitsPerByteValue);
  return (aBits + (kBitsPerByte - 1)) / kBitsPerByte;
}

template <Endian taToEndian = Endian::Native, Endian taFromEndian = Endian::Native>
struct EndianConverter
{
  // The combinations: Endian::Little -> Endian::Little and Endian::Big ->
  // Endian::Big

  template <typename taValueType>
  inline static constexpr taValueType Convert(taValueType&& aSourceValue) NOEXCEPT
  {
    return std::forward<taValueType>(aSourceValue);
  }
};

template <>
struct EndianConverter<Endian::Big, Endian::Little>
{
  // The combinations: Endian::Little -> Endian::Big and Endian::Big ->
  // Endian::Little;

  template <typename taValueType>
  inline static taValueType Convert(taValueType aSourceValue) NOEXCEPT
  {
    return ByteSwap(aSourceValue);
  }
};

template <>
struct EndianConverter<Endian::Little, Endian::Big>
{
  template <typename taValueType>
  inline static taValueType Convert(taValueType aSourceValue) NOEXCEPT
  {
    return ByteSwap(aSourceValue);
  }
};

template <size_t taSize>
struct SizeCompatibleImpl
{
  static_assert(taSize <= 8);
  using TType = std::uint64_t;
};

template <>
struct SizeCompatibleImpl<1>
{
  using TType = std::uint8_t;
};

template <>
struct SizeCompatibleImpl<2>
{
  using TType = std::uint16_t;
};

template <>
struct SizeCompatibleImpl<4>
{
  using TType = std::uint32_t;
};

template <typename taValue>
struct SizeCompatible
{
  using TType = typename SizeCompatibleImpl<sizeof(taValue)>::TType;
};

} // namespace AbstractPlatform
