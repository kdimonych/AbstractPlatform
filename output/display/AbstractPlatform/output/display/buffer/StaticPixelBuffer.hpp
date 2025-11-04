#pragma once
#include <AbstractPlatform/common/BitBuffer.hpp>
#include <AbstractPlatform/common/SimpleMath.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/output/display/buffer/PixelBufferImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <new>
#include <variant>

namespace AbstractPlatform {
/**
 * @brief The PixelBufer structure is used to store pixel data.
 * It contains the width and height of the pixel buffer and a unique pointer to the pixel data.
 * The buffer structure:
 * @code
 *  First element of the buffer.
 *      |
 *      V
 * | x:0, y:0          | x:1, y:0         | ... | x:iWidth-1, y:0         |
 * | x:1, y:0          | x:1, y:0         | ... | x:iWidth-1, y:1         |
 * | x:1, y:0          | x:1, y:0         | ... | x:iWidth-1, y:2         |
 *                                    ...
 * | x:01, y:iHeight-1 | x:1, y:iHeight-1 | ... | x:iWidth-1, y:iHeight-1 |
 *                                                           ^
 *                                                           |
 *                                              Last element of the buffer.
 * @endcode
 */
template <size_t taWidth,
          size_t taHeight,
          typename taPixel,
          TBufferOrientation taOrientation = TBufferOrientation::Horizontal>
struct TStaticPixelBuffer;

template <size_t taWidth, size_t taHeight, typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TStaticPixelBuffer<taWidth, taHeight, taPixel, taOrientation>>
{
  using TPixel          = taPixel;
  using TBuffer         = std::array<TPixel, taWidth * taHeight>;
  using TBufferRef      = TBuffer&;
  using TConstBufferRef = const TBuffer&;

  static constexpr size_t             kWidth       = taWidth;
  static constexpr size_t             kHeight      = taHeight;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <size_t taWidth, size_t taHeight, typename taPixel, TBufferOrientation taOrientation>
struct TStaticPixelBuffer
  : public TPixelBufferImpl<TStaticPixelBuffer<taWidth, taHeight, taPixel, taOrientation>>
{
  using TThis            = TStaticPixelBuffer<taWidth, taHeight, taPixel, taOrientation>;
  using TPixelBufferImpl = TPixelBufferImpl<TThis>;
  using TTraits          = TPixelBufferTraits<TThis>;
  using TPixel           = typename TTraits::TPixel;
  using TBuffer          = typename TTraits::TBuffer;
  using TBufferRef       = typename TTraits::TBufferRef;
  using TConstBufferRef  = typename TTraits::TConstBufferRef;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  static constexpr size_t             kWidth       = TTraits::kWidth;
  static constexpr size_t             kHeight      = TTraits::kHeight;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  static_assert(kWidth > 0 && kHeight > 0 && "Invalid pixel buffer dimensions");

  /**
   * @brief Construct a new Static Pixel Buffer object
   * NOTE: This will create a pixel buffer with default-initialized pixels.
   *       If you want to initialize the pixels, use the constructor with parameters.
   */
  inline constexpr TStaticPixelBuffer()
    : TPixelBufferImpl()
    , iPixelBuffer{}
  {
  }

  inline constexpr TStaticPixelBuffer(TPixel aDefaultPixel)
    : TPixelBufferImpl()
    , iPixelBuffer{}
  {
    std::fill(iPixelBuffer.data(), iPixelBuffer.data() + this->Size(), aDefaultPixel);
  }

  template <typename... taPixels>
  inline constexpr TStaticPixelBuffer(taPixels&&... aPixels)
    : TPixelBufferImpl()
    , iPixelBuffer{std::forward<taPixels>(aPixels)...}
  {
    static_assert(sizeof...(taPixels) == kWidth * kHeight, "Invalid number of pixels");
  }

  TStaticPixelBuffer(TStaticPixelBuffer&&)            = default;
  TStaticPixelBuffer& operator=(TStaticPixelBuffer&&) = default;

  // Copy deliberately prohibited to enforce more efficient usage patterns.
  // This is to ensure that the pixel buffer is not copied, but rather moved or initialized
  TStaticPixelBuffer(const TStaticPixelBuffer&)            = delete;
  TStaticPixelBuffer& operator=(const TStaticPixelBuffer&) = delete;

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return kWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return kHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return iPixelBuffer.size();
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified absolute index.
   */
  inline constexpr TPixel GetPixel(size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    return iPixelBuffer[aIndex];
  }

  /**
   * @brief Set the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified index.
   */
  inline constexpr void SetPixel(size_t aIndex, TPixel aPixel) NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    iPixelBuffer[aIndex] = aPixel;
  }

  inline constexpr void Clear() NOEXCEPT
  {
    const auto pixelBufferEnd = iPixelBuffer + Size();
    for (auto p = iPixelBuffer; p < pixelBufferEnd; ++p)
    {
      *p = TPixel{};
    }
  }

  inline constexpr void SetAll(TPixel aPixel) NOEXCEPT
  {
    const auto end = iPixelBuffer + Size();
    for (auto p = iPixelBuffer; p < end; ++p)
    {
      *p = aPixel;
    }
  }

  /**
   * @brief Returns the pointer to the pixel buffer.
   *
   * @return TPixel* The pointer to the pixel buffer.
   */
  inline constexpr TBufferRef GetInnerBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferRef GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  TBuffer iPixelBuffer;
};

/****************** Compressed bit buffer ******************/
inline static constexpr size_t BlockSize(size_t aBitSize)
{
  return size_t{1} << GetClosestPowerOfTwoDivider((aBitSize + kBitsPerByte - 1) / kBitsPerByte,
                                                  sizeof(size_t));
}

template <size_t taWidth, size_t taHeight, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TStaticPixelBuffer<taWidth, taHeight, TBitPixel, taOrientation>>
{
  using TPixel = TBitPixel;

  static constexpr size_t             kWidth       = taWidth;
  static constexpr size_t             kHeight      = taHeight;
  static constexpr TBufferOrientation kOrientation = taOrientation;

  using TBlockType      = typename SizeCompatibleType<BlockSize(kWidth* kHeight)>::TType;
  using TBuffer         = TBitBuffer<kWidth * kHeight, TBlockType, Endian::Native>;
  using TBufferRef      = TBuffer&;
  using TConstBufferRef = const TBuffer&;
};

template <size_t taWidth, size_t taHeight, TBufferOrientation taOrientation>
struct TStaticPixelBuffer<taWidth, taHeight, TBitPixel, taOrientation>
  : public TPixelBufferImpl<TStaticPixelBuffer<taWidth, taHeight, TBitPixel, taOrientation>>
{
  using TThis            = TStaticPixelBuffer<taWidth, taHeight, TBitPixel, taOrientation>;
  using TPixelBufferImpl = TPixelBufferImpl<TThis>;
  using TTraits          = TPixelBufferTraits<TThis>;
  using TPixel           = typename TTraits::TPixel;
  using TBlockType       = typename TTraits::TBlockType;
  using TBuffer          = typename TTraits::TBuffer;
  using TBufferRef       = typename TTraits::TBufferRef;
  using TConstBufferRef  = typename TTraits::TConstBufferRef;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  static constexpr size_t             kWidth       = TTraits::kWidth;
  static constexpr size_t             kHeight      = TTraits::kHeight;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  static_assert(kWidth > 0 && kHeight > 0 && "Invalid pixel buffer dimensions");

  /**
   * @brief Construct a new Static Pixel Buffer object
   * NOTE: This will create a pixel buffer with default-initialized pixels.
   *       If you want to initialize the pixels, use the constructor with parameters.
   */
  inline constexpr TStaticPixelBuffer()
    : TPixelBufferImpl()
    , iPixelBuffer{}
  {
  }

  inline constexpr TStaticPixelBuffer(TPixel aDefaultPixel)
    : TPixelBufferImpl()
    , iPixelBuffer{}
  {
    SetAll(aDefaultPixel);
  }

  // TODO: implement static constructor
  // template <typename... taPixels>
  // inline constexpr TStaticPixelBuffer(taPixels&&... aPixels)
  //   : TPixelBufferImpl()
  //   , iPixelBuffer{std::forward<taPixels>(aPixels)...}
  // {
  //   static_assert(sizeof...(taPixels) == kWidth * kHeight, "Invalid number of pixels");
  // }

  TStaticPixelBuffer(TStaticPixelBuffer&&)            = default;
  TStaticPixelBuffer& operator=(TStaticPixelBuffer&&) = default;

  // Copy deliberately prohibited to enforce more efficient usage patterns.
  // This is to ensure that the pixel buffer is not copied, but rather moved or initialized
  TStaticPixelBuffer(const TStaticPixelBuffer&)            = delete;
  TStaticPixelBuffer& operator=(const TStaticPixelBuffer&) = delete;

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return kWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return kHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return iPixelBuffer.Size();
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified absolute index.
   */
  inline constexpr TPixel GetPixel(size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    return TPixel{iPixelBuffer.Test(aIndex)};
  }

  /**
   * @brief Set the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified index.
   */
  inline constexpr void SetPixel(size_t aIndex, TPixel aPixel) NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    aPixel ? iPixelBuffer.SetBit(aIndex) : iPixelBuffer.ClearBit(aIndex);
  }

  inline constexpr void Clear() NOEXCEPT
  {
    iPixelBuffer.Clear();
  }

  inline constexpr void SetAll(TPixel aPixel) NOEXCEPT
  {
    if (aPixel)
    {
      iPixelBuffer.Set();
    }
    else
    {
      iPixelBuffer.Clear();
    }
  }

  /**
   * @brief Returns the pointer to the pixel buffer.
   *
   * @return TPixel* The pointer to the pixel buffer.
   */
  inline constexpr TBufferRef GetInnerBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferRef GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  TBuffer iPixelBuffer;
};
} // namespace AbstractPlatform
