#pragma once
#include <AbstractPlatform/common/BitOperations.hpp>

#include <array>
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

template <typename taArrayElement, size_t taArrayElemetsCount>
inline constexpr size_t BitSize(const taArrayElement (&)[taArrayElemetsCount])
{
  return sizeof(taArrayElement) * kBitsPerByte * taArrayElemetsCount;
}

template <typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM>
inline constexpr size_t BitSize(const taArrayElement (&)[taArrayElemetsN][taArrayElemetsM])
{
  return taArrayElemetsN * taArrayElemetsM * sizeof(taArrayElement) * kBitsPerByte;
}

} // namespace AbstractPlatform
