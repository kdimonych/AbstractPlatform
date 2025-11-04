#pragma once

#include <AbstractPlatform/common/BitOperations.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>

namespace AbstractPlatform {

inline static constexpr std::uint8_t InvertColorChannel(std::uint8_t aPixelChannel)
{
  return 255 - aPixelChannel;
}

inline static constexpr bool InvertColorChannel(bool aPixelChannel)
{
  return !aPixelChannel;
}

template <typename taPixel>
inline static constexpr taPixel InvertColor(taPixel aPixel)
{
  return taPixel::FromPacked(InvertBits(aPixel.Pack()));
}

} // namespace AbstractPlatform
