#pragma once

#include <AbstractPlatform/common/BitOperations.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform {

struct TBitPixel
{
  constexpr TBitPixel() NOEXCEPT = default;

  constexpr TBitPixel(bool aPixelValue) NOEXCEPT : iPixelValue{aPixelValue} { }

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
    return 1u;
  }

  /**
   * @brief The mask is used to compressed the pixel value.
   *
   * @return constexpr std::uint32_t
   */
  static constexpr std::uint32_t Mask()
  {
    return 0x1u; // 1 bit mask
  }

  /**
   * @brief Pack the pixel value tightly.
   *
   * @return constexpr std::uint32_t Returns the unified pixel value.
   */
  inline constexpr std::uint32_t Pack() const
  {
    return static_cast<std::uint32_t>(iPixelValue);
  }

  /**
   * @brief Unpack the pixel value from a given value.
   *
   * @param aValue The value to unpack the pixel from.
   */
  inline constexpr void Unpack(std::uint32_t aValue)
  {
    iPixelValue = static_cast<bool>(aValue & Mask());
  }
};

struct TRGBPixel
{
  constexpr TRGBPixel() NOEXCEPT = default;

  constexpr TRGBPixel(std::uint8_t aRed, std::uint8_t aGreen, std::uint8_t aBlue) NOEXCEPT
    : iRed{aRed},
      iGreen{aGreen},
      iBlue{aBlue}
  {
  }

  std::uint8_t iRed   = 0u;
  std::uint8_t iGreen = 0u;
  std::uint8_t iBlue  = 0u;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 24u;
  }

  /**
   * @brief The mask is used to compressed the pixel value.
   *
   * @return constexpr std::uint32_t
   */
  static constexpr std::uint32_t Mask()
  {
    return 0x00ffffffu; // 24 bit mask
  }

  /**
   * @brief Checks if padding was applied by the compiler.
   *
   * @return constexpr bool True if padding was applied, false otherwise.
   */
  static constexpr bool HasPadding()
  {
    return sizeof(TRGBPixel) != (sizeof(iRed) + sizeof(iGreen) + sizeof(iBlue));
  }

  /**
   * @brief Pack the pixel value tightly.
   *
   * @return constexpr std::uint32_t Returns the unified pixel value.
   */
  inline std::uint32_t Pack() const
  {
    // Use the most common Little-endian representation of RGB pixel: 0xBBGGRR
    if constexpr (!HasPadding()
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // If the structure is tightly packed and the platform is Little-endian,
      // we can reinterpret the memory directly. This is more efficient way to get the value.
      // This is safe because the structure is tightly packed and the endianness matches.
      // This is a hack to avoid the need for bitwise operations.
      return *reinterpret_cast<const std::uint32_t*>(this) & Mask();
    }
    else
    {
      return static_cast<std::uint32_t>((iBlue << 16u) | (iGreen << 8u) | iRed);
    }
  }

  /**
   * @brief Unpack the pixel value from a given value.
   *
   * @param aValue The value to unpack the pixel from.
   */
  inline void Unpack(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel: 0xBBGGRR
    iRed   = static_cast<std::uint8_t>(aValue & 0x000000ffu);
    iGreen = static_cast<std::uint8_t>((aValue & 0x0000ff00u) >> 8u);
    iBlue  = static_cast<std::uint8_t>((aValue & 0x00ff0000u) >> 16u);
    // Note: Alpha channel is not used in TRGBPixel, so it is not set.
  }
};

struct TRGBAPixel
{
  constexpr TRGBAPixel() NOEXCEPT = default;

  constexpr TRGBAPixel(std::uint8_t aRed,
                       std::uint8_t aGreen,
                       std::uint8_t aBlue,
                       std::uint8_t aAlpha) NOEXCEPT : iRed{aRed},
                                                       iGreen{aGreen},
                                                       iBlue{aBlue},
                                                       iAlpha{aAlpha}
  {
  }

  std::uint8_t iRed   = 0u;
  std::uint8_t iGreen = 0u;
  std::uint8_t iBlue  = 0u;
  std::uint8_t iAlpha = 0u;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 32u;
  }

  /**
   * @brief The mask is used to compressed the pixel value.
   *
   * @return constexpr std::uint32_t
   */
  static constexpr std::uint32_t Mask()
  {
    return 0xffffffffu; // 32 bit mask
  }

  /**
   * @brief Checks if padding was applied by the compiler.
   *
   * @return constexpr bool True if padding was applied, false otherwise.
   */
  static constexpr bool HasPadding()
  {
    return sizeof(TRGBAPixel) != (sizeof(iAlpha) + sizeof(iRed) + sizeof(iGreen) + sizeof(iBlue));
  }

  /**
   * @brief Pack the pixel value tightly.
   *
   * @return constexpr std::uint32_t Returns the unified pixel value.
   */
  inline std::uint32_t Pack() const
  {
    // Use the most common Little-endian representation of RGB pixel: 0xAABBGGRR
    if constexpr (!HasPadding()
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // If the structure is tightly packed and the platform is Little-endian,
      // we can reinterpret the memory directly. This is more efficient way to get the value.
      // This is safe because the structure is tightly packed and the endianness matches.
      return *reinterpret_cast<const std::uint32_t*>(this);
    }
    else
    {
      return static_cast<std::uint32_t>((iAlpha << 24u) | (iBlue << 16u) | (iGreen << 8u) | iRed);
    }
  }

  /**
   * @brief Unpack the pixel value from a given value.
   *
   * @param aValue The value to unpack the pixel from.
   */
  inline void Unpack(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel: 0xAABBGGRR
    if constexpr (!HasPadding()
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // If the structure is tightly packed and the platform is Little-endian,
      // we can reinterpret the memory directly. This is more efficient way to get the value.
      // This is safe because the structure is tightly packed and the endianness matches.
      *reinterpret_cast<std::uint32_t*>(this) = aValue;
    }
    else
    {
      iRed   = static_cast<std::uint8_t>(aValue & 0x000000ffu);
      iGreen = static_cast<std::uint8_t>((aValue & 0x0000ff00u) >> 8u);
      iBlue  = static_cast<std::uint8_t>((aValue & 0x00ff0000u) >> 16u);
      iAlpha = static_cast<std::uint8_t>((aValue & 0xff000000u) >> 24u);
    }
  }
};

struct TRGB565Pixel
{
  constexpr TRGB565Pixel() NOEXCEPT = default;

  constexpr TRGB565Pixel(std::uint8_t aRed, std::uint8_t aGreen, std::uint8_t aBlue) NOEXCEPT
    : iRed{aRed},
      iGreen{aGreen},
      iBlue{aBlue}
  {
  }

  std::uint8_t iRed   = 0u;
  std::uint8_t iGreen = 0u;
  std::uint8_t iBlue  = 0u;

  /**
   * @brief Returns the bit count used to represent the pixel value.
   *
   * @return constexpr size_t The bit count used to represent the pixel value.
   */
  static constexpr size_t Bits()
  {
    return 16u;
  }

  /**
   * @brief The mask is used to compressed the pixel value.
   *
   * @return constexpr std::uint32_t
   */
  static constexpr std::uint32_t Mask()
  {
    return 0x0000ffffu; // 16 bit mask
  }

  /**
   * @brief Pack the pixel value tightly.
   *
   * @return constexpr std::uint32_t Returns the unified pixel value.
   */
  inline constexpr std::uint32_t Pack() const
  {
    // Use the most common Little-endian representation of RGB pixel:
    // |    0x00   |    0x01   |
    // | RRRRR GGG | GGG BBBBB |
    return static_cast<std::uint32_t>(((iRed & 0x1fu) << 11u) |  // Red: 5 bits
                                      ((iGreen & 0x3fu) << 5u) | // Green: 6 bits
                                      (iBlue & 0x1fu)            // Blue: 5 bits
    );
  }

  /**
   * @brief Unpack the pixel value from a given value.
   *
   * @param aValue The value to unpack the pixel from.
   */
  inline void Unpack(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel:
    // |    0x00   |    0x01   |
    // | RRRRR GGG | GGG BBBBB |
    iRed   = static_cast<std::uint8_t>((aValue >> 11u) & 0x1Fu);
    iGreen = static_cast<std::uint8_t>((aValue >> 5u) & 0x3Fu);
    iBlue  = static_cast<std::uint8_t>(aValue & 0x1Fu);
    // Note: Alpha channel is not used in TRGBPixel, so it is not set.
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
