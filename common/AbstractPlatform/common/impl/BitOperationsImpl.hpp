#pragma once

#include <AbstractPlatform/platform/Platform.hpp>

#include <cstdint>

namespace AbstractPlatform::Impl {

/************** SizeCompatible implementation **************/
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

template <typename taT>
inline constexpr auto CastToProxyImpl(taT aValue) NOEXCEPT
{
  using TProxyType = typename SizeCompatibleImpl<sizeof(taT)>::TType;
  return static_cast<TProxyType>(aValue);
}

/************** ByteSwap implementation **************/
#ifndef STL_BYTESWAP_AVAILABLE
template <typename taT, size_t /*taValueSize*/>
struct ByteSwapImpl
{
  // Default implementation.
  static_assert(sizeof(taT) <= 8, "ByteSwap supports only types with size <= 8 bytes");
};

template <typename taT>
struct ByteSwapImpl<taT, 1>
{
  // Optimized implementation.
  inline static constexpr taT Apply(taT aValue) NOEXCEPT
  {
    return aValue;
  }
};

template <typename taT>
struct ByteSwapImpl<taT, 2>
{
  // Optimized implementation.
  inline static constexpr taT Apply(taT aValue) NOEXCEPT
  {
    return static_cast<taT>((CastToProxyImpl(aValue) << 8) | (CastToProxyImpl(aValue) >> 8));
  }
};

template <typename taT>
struct ByteSwapImpl<taT, 4>
{
  // Optimized implementation.
  inline static constexpr taT Apply(taT aValue) NOEXCEPT
  {
    return static_cast<taT>(
      (CastToProxyImpl(aValue) << 24) | ((CastToProxyImpl(aValue) & 0x00FF0000) >> 8)
      | ((CastToProxyImpl(aValue) & 0x0000FF00) << 8) | (CastToProxyImpl(aValue) >> 24));
  }
};

template <typename taT>
struct ByteSwapImpl<taT, 8>
{
  // Optimized implementation.
  inline static constexpr taT Apply(taT aValue) NOEXCEPT
  {
    return static_cast<taT>(
      (CastToProxyImpl(aValue) << 56) | ((CastToProxyImpl(aValue) & 0x00FF000000000000) >> 40)
      | ((CastToProxyImpl(aValue) & 0x0000FF0000000000) >> 24)
      | ((CastToProxyImpl(aValue) & 0x000000FF00000000) >> 8)
      | ((CastToProxyImpl(aValue) & 0x00000000FF000000) << 8)
      | ((CastToProxyImpl(aValue) & 0x0000000000FF0000) << 24)
      | ((CastToProxyImpl(aValue) & 0x000000000000FF00) << 40) | (CastToProxyImpl(aValue) >> 56));
  }
};
#endif // STL_BYTESWAP_AVAILABLE

} // namespace AbstractPlatform::Impl
