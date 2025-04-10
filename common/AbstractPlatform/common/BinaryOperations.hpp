#pragma once

#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>

#include <type_traits>

namespace AbstractPlatform {
template <typename taValue>
inline static constexpr taValue SetBit(taValue aValue, size_t aBitIndex)
{
  using TTargetType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<taValue>(static_cast<TTargetType>(aValue) | (TTargetType{1} << aBitIndex));
}

template <typename taValue>
inline static constexpr taValue ClearBit(taValue aValue, size_t aBitIndex)
{
  using TTargetType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<taValue>(static_cast<TTargetType>(aValue) & ~(TTargetType{1} << aBitIndex));
}

template <typename taValue>
inline static constexpr taValue ToggleBit(taValue aValue, size_t aBitIndex)
{
  using TTargetType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<taValue>(static_cast<TTargetType>(aValue) ^ (TTargetType{1} << aBitIndex));
}

template <typename taValue>
inline static constexpr bool CheckBit(taValue aValue, size_t aBitIndex)
{
  using TTargetType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<bool>((static_cast<TTargetType>(aValue) >> aBitIndex) & TTargetType{1});
}

template <typename taValue>
inline static constexpr auto AllBitsSet()
{
  using TProxyType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<taValue>(std::numeric_limits<TProxyType>::max());
}

template <typename taValue>
inline static constexpr auto AllBitsCleared()
{
  using TProxyType = typename SizeCompatible<taValue>::TType;
  static_assert(std::is_integral<taValue>::value);
  return static_cast<taValue>(std::numeric_limits<TProxyType>::max());
}

} // namespace AbstractPlatform
