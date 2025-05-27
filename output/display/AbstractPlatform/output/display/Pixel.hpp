#pragma once

#include <AbstractPlatform/common/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform {

struct TBitPixel
{
  constexpr TBitPixel() = default;

  constexpr TBitPixel(bool aPixelValue)
    : iPixelValue{aPixelValue}
  {
  }

  constexpr operator bool() const
  {
    return iPixelValue;
  }

  bool iPixelValue = false;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 1;
  }
};

struct TRGBPixel
{
  constexpr TRGBPixel() = default;

  constexpr TRGBPixel(std::uint8_t aRed, std::uint8_t aGreen, std::uint8_t aBlue)
    : iRed{aRed}
    , iGreen{aGreen}
    , iBlue{aBlue}
  {
  }

  std::uint8_t iRed   = 0;
  std::uint8_t iGreen = 0;
  std::uint8_t iBlue  = 0;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 24;
  }
};

struct TRGBAPixel
{
  constexpr TRGBAPixel() = default;

  constexpr TRGBAPixel(std::uint8_t aRed,
                       std::uint8_t aGreen,
                       std::uint8_t aBlue,
                       std::uint8_t aAlpha)
    : iRed{aRed}
    , iGreen{aGreen}
    , iBlue{aBlue}
    , iAlpha{aAlpha}
  {
  }

  std::uint8_t iRed   = 0;
  std::uint8_t iGreen = 0;
  std::uint8_t iBlue  = 0;
  std::uint8_t iAlpha = 0;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 24;
  }
};

union TPixel
{
  TPixel()
    : iRGBAPixel{} // Explicitly initialize the union to avoid undefined behavior
  {
  }

  TRGBAPixel iRGBAPixel;
  TRGBPixel  iRGBPixel;
  TBitPixel  iBitPixel;
};

} // namespace AbstractPlatform
