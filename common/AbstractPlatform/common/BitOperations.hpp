#pragma once

#include <AbstractPlatform/common/SimpleMath.hpp>
#include <AbstractPlatform/common/impl/BitOperationsImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

#ifdef STL_BITOPS_AVAILABLE
#include <bit>
#endif

namespace AbstractPlatform {

#ifndef STL_ENDIAN_AVAILABLE
/**
 * @brief Determines scalar type endianness
 *
 *  The 32 bit value a = 0x0A0B0C0D will be arranged in the memory the following way:
 */
enum class Endian
{
  Little = 0, // a[0] = 0D, a[1] = 0C, ... a[3] = 0A (LSB->, MSB<-)
  Big    = 1, // a[0] = 0A, a[1] = 0B, ... a[3] = 0D (LSB<-, MSB->)
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
using ByteSwap = std::byteswap<taT>;
#else
/**
 * @brief The function reverses the byte order of the given value.
 *
 * @tparam taT The type of the value to reverse bytes.
 * @param aValue The value to reverse bytes.
 * @return constexpr taT The value with reversed bytes.
 * @note This function is intended for integral types only.
 */
template <typename taT, std::enable_if_t<std::is_integral<taT>::value, int> = 0>
static constexpr taT ByteSwap(taT aValue) NOEXCEPT
{
  static_assert(std::has_unique_object_representations_v<taT>, "taT may not have padding bits");

  return Impl::ByteSwapImpl<taT, sizeof(taT)>::Apply(aValue);
}
#endif

/**
 * @brief Returns the bit count used to represent the value.
 *
 * @tparam taValue The type of the value.
 * @param aValue The value to get the bit count for.
 * @return constexpr size_t The bit count used to represent the value.
 */
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

/**
 * @brief Returns an integral unsigned type that can fully fit the taValue type
 *
 */
template <typename taValue>
struct SizeCompatible
{
  using TType = typename Impl::SizeCompatibleImpl<sizeof(taValue)>::TType;
};

/**
 * @brief Returns an integral unsigned type that can fit the taSize bytes
 *
 */
template <size_t taSize>
struct SizeCompatibleType
{
  using TType = typename Impl::SizeCompatibleImpl<taSize>::TType;
};

/**
 * @brief Casts a value to a proxy type that is compatible with the size of the value.
 *
 * This function is used to ensure that the value is cast to a type that can hold its size
 * without loss of information.
 *
 * @tparam taT The type of the value to cast.
 * @param aValue The value to cast.
 * @return constexpr auto The casted value in a compatible proxy type.
 */
template <typename taT>
inline static constexpr auto CastToProxy(taT aValue) NOEXCEPT
{
  static_assert(std::is_integral_v<taT>, "aValue must be an integral type");
  return Impl::CastToProxyImpl(aValue);
}

/**
 * @brief Returns a mask for a specific byte index.
 * This function creates a mask where byte with specified index has all bits set to 1.
 *
 * @tparam taMaskType The type of the mask to create.
 *                    It should be an integral type.
 * @param aByteIndex The index of byte to set its bits.
 * @return constexpr taMaskType A mask with the specified set to 0xFF.
 */
template <typename taMaskType>
inline static constexpr taMaskType ByteMask(size_t aByteIndex) NOEXCEPT
{
  static_assert(std::is_integral_v<taMaskType>, "taMaskType must be an integral type");

  using TProxyType = typename SizeCompatible<taMaskType>::TType;
  return static_cast<taMaskType>(TProxyType{0xff} << (aByteIndex * 8u));
}

/**
 * @brief Returns a mask where all bits set to 1 except the byte specified by index.
 * Is equivalent to `~ByteMask(aByteIndex)`.
 *
 * @tparam taMaskType The type of the mask to create.
 *                    It should be an integral type.
 * @param aAtByteIndex The index of byte to set its bits to 0.
 * @return constexpr taMaskType A mask with the specified byte set to 0x00.
 */
template <typename taMaskType>
inline static constexpr taMaskType ByteInverseMask(size_t aByteIndex) NOEXCEPT
{
  return ~ByteMask<taMaskType>(aByteIndex);
}

/**
 * @brief Inverts the bits of a value.
 *
 * @tparam taType The type of the value to invert.
 * @param aByteIndex The value to invert.
 * @return constexpr taType The inverted value.
 */
template <typename taType>
inline static constexpr taType InvertBits(taType aByteIndex) NOEXCEPT
{
  static_assert(std::is_integral_v<taType>, "taType must be an integral type");

  using TProxyType = typename SizeCompatible<taType>::TType;
  return static_cast<taType>(~static_cast<TProxyType>(aByteIndex));
}

/**
 * @brief Sets a specific bit in the value.
 * This function sets the bit at the specified index to 1.
 *
 * @tparam taValue The type of the value to modify.
 *                 It should be an integral type.
 * @param aValue The value to modify.
 * @param aBitIndex The index of the bit to set (0-based).
 * @return constexpr taValue The modified value with the specified bit set.
 */
template <typename taDataType, typename taByteType>
inline static constexpr taDataType
SetByte(taDataType aOfData, size_t aAtByteIndex, taByteType aToValue) NOEXCEPT
{
  static_assert(std::is_integral_v<taDataType>, "taDataType must be an integral type");
  static_assert(std::is_integral_v<taByteType>, "taByteType must be an integral type");

  using TProxyType = typename SizeCompatible<taDataType>::TType;

  return static_cast<taDataType>(
    (static_cast<TProxyType>(aOfData) & ByteInverseMask<TProxyType>(aAtByteIndex))
    | ((static_cast<TProxyType>(aToValue) << (aAtByteIndex * kBitsPerByte))
       & ByteMask<TProxyType>(aAtByteIndex)));
}

inline static constexpr bool IsValidBlockSize(size_t aBlockSize)
{
  return aBlockSize > 1 && IsPowerOfTwo(aBlockSize) && aBlockSize <= kPlatformLongLongSize;
}

template <size_t taBlockSize, Endian taBlockEndian, class taEnable = void>
struct TEndianBitIndexMapper
{
  static_assert(taBlockSize > 0, "taBlockSize must be greater than zero");
  static_assert(taBlockSize <= kPlatformLongLongSize,
                "taBlockSize must be less or equal to kPlatformLongLongSize");
};

template <Endian taBlockEndian, class taEnable>
struct TEndianBitIndexMapper<1, taBlockEndian, taEnable>
{
  static constexpr size_t kBlockSize = 1;
  static constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;

  template <typename taIndexType>
  inline static constexpr void InplaceMapIndex(taIndexType& aInBlockBitIndex)
  {
    assert(aInBlockBitIndex < kBlockBits);
    assert(aInBlockBitIndex >= taIndexType{0});
    // Do nothing for 1 byte blocks
  }

  template <typename taIndexType>
  inline static constexpr taIndexType MapIndex(taIndexType aInBlockBitIndex)
  {
    InplaceMapIndex(aInBlockBitIndex);
    // Do nothing for 1 byte blocks
    return aInBlockBitIndex;
  }
};

template <size_t taBlockSize, Endian taBlockEndian>
struct TEndianBitIndexMapper<taBlockSize,
                             taBlockEndian,
                             std::enable_if_t<IsValidBlockSize(taBlockSize)>>
{
  static constexpr size_t kBlockSize = taBlockSize;
  static constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;

  template <typename taIndexType>
  inline static constexpr void InplaceMapIndex(taIndexType& aInBlockBitIndex)
  {
    assert(aInBlockBitIndex < kBlockBits);
    assert(aInBlockBitIndex >= taIndexType{0});

    if constexpr (taBlockEndian != Endian::Native)
    {
      // For non-native endian, we need to reverse the byte order within the block.
      // The in-byte index stays untouched.
      // For 4 byte blocks, the index map is as follows:
      // | BitIndex in | ByteIndex in | ByteIndex out | BitIndex out |
      // |     0..7    |      0       |      3        |     24..31   |
      // |     8..15   |      1       |      2        |     16..23   |
      // |    16..23   |      2       |      1        |      8..15   |
      // |    24..31   |      3       |      0        |      0..7    |
      // The mapping is done by reversing the byte order and keeping the bit index within the byte.
      aInBlockBitIndex =
        (static_cast<taIndexType>(kBlockBits - 1 - aInBlockBitIndex) & (~taIndexType{0} << 3))
        | (aInBlockBitIndex & 0x7);
    }
  }

  template <typename taIndexType>
  inline static constexpr taIndexType MapIndex(taIndexType aInBlockBitIndex)
  {
    InplaceMapIndex(aInBlockBitIndex);
    return aInBlockBitIndex;
  }
};

} // namespace AbstractPlatform
