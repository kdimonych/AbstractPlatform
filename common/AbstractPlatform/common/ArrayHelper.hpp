#pragma once
#include <AbstractPlatform/common/BitOperations.hpp>

#include <array>
#include <bitset>
#include <cstddef>

namespace AbstractPlatform {
template <typename taArrayElement, size_t taArrayElemetsCount>
inline constexpr size_t ArrayLength(const taArrayElement (&)[taArrayElemetsCount])
{
  return taArrayElemetsCount;
}

template <typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM>
inline constexpr size_t ArrayLength(const taArrayElement (&)[taArrayElemetsN][taArrayElemetsM])
{
  return taArrayElemetsN * taArrayElemetsM;
}

template <typename taArrayElement, size_t taArrayElemetsN>
inline constexpr size_t ArrayLength(const std::array<taArrayElement, taArrayElemetsN>&)
{
  return taArrayElemetsN;
}

template <size_t taArrayElemetsN>
inline constexpr size_t ArrayLength(const std::bitset<taArrayElemetsN>&)
{
  return taArrayElemetsN;
}

template <typename taArrayElement, size_t taArrayElemetsCount>
inline constexpr size_t ArraySizeBytes(const taArrayElement (&)[taArrayElemetsCount])
{
  return taArrayElemetsCount * sizeof(taArrayElement);
}

template <typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM>
inline constexpr size_t ArraySizeBytes(const taArrayElement (&)[taArrayElemetsN][taArrayElemetsM])
{
  return taArrayElemetsN * taArrayElemetsM * sizeof(taArrayElement);
}

template <typename taArrayElement, size_t taArrayElemetsN>
inline constexpr size_t ArraySizeBytes(const std::array<taArrayElement, taArrayElemetsN>&)
{
  return taArrayElemetsN * sizeof(taArrayElement);
}

template <size_t taArrayElemetsN>
inline constexpr size_t ArraySizeBytes(const std::bitset<taArrayElemetsN>&)
{
  static_assert(sizeof(std::bitset<taArrayElemetsN>)
                == (taArrayElemetsN + kBitsPerByte - 1) / kBitsPerByte);
  return (taArrayElemetsN + kBitsPerByte - 1) / kBitsPerByte;
}

template <typename taArrayElement, size_t taArrayElemetsCount>
inline constexpr size_t BitSize(const taArrayElement (&aArray)[taArrayElemetsCount])
{
  return ArraySizeBytes(aArray) * kBitsPerByte;
}

template <typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM>
inline constexpr size_t BitSize(const taArrayElement (&aArray)[taArrayElemetsN][taArrayElemetsM])
{
  return ArraySizeBytes(aArray) * kBitsPerByte;
}

template <typename taArrayElement, size_t taArrayElemetsN>
inline constexpr size_t BitSize(const std::array<taArrayElement, taArrayElemetsN>& aArray)
{
  return ArraySizeBytes(aArray) * kBitsPerByte;
}

template <size_t taArrayElemetsN>
inline constexpr size_t BitSize(const std::bitset<taArrayElemetsN>& aArray)
{
  return aArray.size();
}

} // namespace AbstractPlatform
