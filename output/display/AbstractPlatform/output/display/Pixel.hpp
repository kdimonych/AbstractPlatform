#pragma once

#include <AbstractPlatform/common/BitOperations.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform {

template <typename taPixel>
struct TPixelTraits;

struct TBitPixel
{
  bool iPixelValue = false;

  constexpr operator bool() const
  {
    return iPixelValue;
  }

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

  inline static constexpr TBitPixel FromPacked(std::uint32_t aValue)
  {
    TBitPixel pixel;
    pixel.Unpack(aValue);
    return pixel;
  }

  inline constexpr bool operator==(const TBitPixel& aOther) const
  {
    return iPixelValue == aOther.iPixelValue;
  }
};

template <>
struct TPixelTraits<TBitPixel>
{
  using TPixel = TBitPixel;

  static constexpr bool IsCompressable()
  {
    return TPixel::Bits() < BitSize(TPixel{});
  }
};

struct TRGBPixel
{
  using TValue  = std::uint8_t;
  TValue iRed   = 0u;
  TValue iGreen = 0u;
  TValue iBlue  = 0u;

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
  inline constexpr std::uint32_t Pack() const
  {
    // Use the most common Little-endian representation of RGB pixel: 0xBBGGRR
    return static_cast<std::uint32_t>((iBlue << 16u) | (iGreen << 8u) | iRed);
  }

  /**
   * @brief Unpack the pixel value from a given value.
   *
   * @param aValue The value to unpack the pixel from.
   */
  inline constexpr void Unpack(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel: 0xBBGGRR
    iRed   = static_cast<TValue>(aValue & 0x000000ffu);
    iGreen = static_cast<TValue>((aValue & 0x0000ff00u) >> 8u);
    iBlue  = static_cast<TValue>((aValue & 0x00ff0000u) >> 16u);
  }

  inline static constexpr TRGBPixel FromPacked(std::uint32_t aValue)
  {
    TRGBPixel pixel;
    pixel.Unpack(aValue);
    return pixel;
  }

  inline constexpr bool operator==(const TRGBPixel& aOther) const
  {
    return iRed == aOther.iRed && iGreen == aOther.iGreen && iBlue == aOther.iBlue;
  }
};

template <>
struct TPixelTraits<TRGBPixel>
{
  using TPixel = TRGBPixel;

  static constexpr bool IsCompressable()
  {
    return TPixel::Bits() < BitSize(TPixel{});
  }
};

struct TRGBAPixel
{
  using TValue = std::uint8_t;

  TValue iRed   = 0u;
  TValue iGreen = 0u;
  TValue iBlue  = 0u;
  TValue iAlpha = 0u;

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
    if constexpr (!HasPadding() && sizeof(TRGBAPixel) == sizeof(std::uint32_t)
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // Use the most common Little-endian representation of RGB pixel: 0xAABBGGRR
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
    if constexpr (!HasPadding() && sizeof(TRGBAPixel) == sizeof(std::uint32_t)
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // If the structure is tightly packed and the platform is Little-endian,
      // we can reinterpret the memory directly. This is more efficient way to get the value.
      // This is safe because the structure is tightly packed and the endianness matches.
      *reinterpret_cast<std::uint32_t*>(this) = aValue;
    }
    else
    {
      iRed   = static_cast<TValue>(aValue & 0x000000ffu);
      iGreen = static_cast<TValue>((aValue & 0x0000ff00u) >> 8u);
      iBlue  = static_cast<TValue>((aValue & 0x00ff0000u) >> 16u);
      iAlpha = static_cast<TValue>((aValue & 0xff000000u) >> 24u);
    }
  }

  inline static TRGBAPixel FromPacked(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel: 0xAABBGGRR
    if constexpr (!HasPadding() && sizeof(TRGBAPixel) == sizeof(std::uint32_t)
                  && (AbstractPlatform::Endian::Native == AbstractPlatform::Endian::Little))
    {
      // If the structure is tightly packed and the platform is Little-endian,
      // we can reinterpret the memory directly. This is more efficient way to get the value.
      // This is safe because the structure is tightly packed and the endianness matches.
      return reinterpret_cast<TRGBAPixel&>(aValue);
    }
    else
    {
      TRGBAPixel pixel;
      pixel.iRed   = static_cast<TValue>(aValue & 0x000000ffu);
      pixel.iGreen = static_cast<TValue>((aValue & 0x0000ff00u) >> 8u);
      pixel.iBlue  = static_cast<TValue>((aValue & 0x00ff0000u) >> 16u);
      pixel.iAlpha = static_cast<TValue>((aValue & 0xff000000u) >> 24u);
      return pixel;
    }
  }

  inline constexpr bool operator==(const TRGBAPixel& aOther) const
  {
    return iRed == aOther.iRed && iGreen == aOther.iGreen && iBlue == aOther.iBlue
           && iAlpha == aOther.iAlpha;
  }
};

template <>
struct TPixelTraits<TRGBAPixel>
{
  using TPixel = TRGBAPixel;

  static constexpr bool IsCompressable()
  {
    return TPixel::Bits() < BitSize(TPixel{});
  }
};

struct TRGB565Pixel
{
  using TValue  = std::uint8_t;
  TValue iRed   = 0u;
  TValue iGreen = 0u;
  TValue iBlue  = 0u;

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
  inline constexpr void Unpack(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel:
    // |    0x00   |    0x01   |
    // | RRRRR GGG | GGG BBBBB |
    iRed   = static_cast<TValue>((aValue >> 11u) & 0x1Fu);
    iGreen = static_cast<TValue>((aValue >> 5u) & 0x3Fu);
    iBlue  = static_cast<TValue>(aValue & 0x1Fu);
    // Note: Alpha channel is not used in TRGBPixel, so it is not set.
  }

  inline static constexpr TRGB565Pixel FromPacked(std::uint32_t aValue)
  {
    // Use the most common Little-endian representation of RGB pixel:
    // |    0x00   |    0x01   |
    // | RRRRR GGG | GGG BBBBB |
    TRGB565Pixel pixel;
    pixel.Unpack(aValue);
    return pixel;
  }

  inline constexpr bool operator==(const TRGB565Pixel& aOther) const
  {
    return iRed == aOther.iRed && iGreen == aOther.iGreen && iBlue == aOther.iBlue;
  }
};

template <>
struct TPixelTraits<TRGB565Pixel>
{
  using TPixel = TRGB565Pixel;

  static constexpr bool IsCompressable()
  {
    return TPixel::Bits() < BitSize(TPixel{});
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
