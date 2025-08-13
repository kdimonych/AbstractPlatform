#pragma once
#include <AbstractPlatform/common/Buffer.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/output/display/buffer/PixelBufferImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <new>
#include <variant>

namespace AbstractPlatform {
template <typename taPixel, TBufferOrientation taOrientation = TBufferOrientation::Horizontal>
struct TPixelBufferView;

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TPixelBufferView<taPixel, taOrientation>>
{
  using TPixel                                     = taPixel;
  using TBuffer                                    = TPixel* const;
  using TBufferPtr                                 = TPixel* const;
  using TConstBufferPtr                            = const TPixel* const;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferView : public TPixelBufferImpl<TPixelBufferView<taPixel, taOrientation>>
{
  using TThis            = TPixelBufferView<taPixel, taOrientation>;
  using TPixelBufferImpl = TPixelBufferImpl<TThis>;
  using TTraits          = TPixelBufferTraits<TThis>;

  using TPixel                                     = typename TTraits::TPixel;
  using TBuffer                                    = typename TTraits::TBuffer;
  using TBufferPtr                                 = typename TTraits::TBufferPtr;
  using TConstBufferPtr                            = typename TTraits::TConstBufferPtr;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  TPixelBufferView(size_t aWidth, size_t aHeight, TBufferPtr buffer)
    : TPixelBufferImpl()
    , iPixelBuffer{buffer}
    , iWidth{aWidth}
    , iHeight{aHeight}
  {
    assert(buffer != nullptr);
    assert(aWidth > 0 && aHeight > 0);
  }

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return iWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return iHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return Width() * Height();
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
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferPtr GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr TBufferPtr GetInnerBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TPixelBufferView<const taPixel, taOrientation>>
{
  using TPixel                                     = taPixel;
  using TPixelRef                                  = TBitPixel&;
  using TPixelConstRef                             = const TBitPixel&;
  using TBuffer                                    = const TPixel* const;
  using TConstBufferPtr                            = const TPixel* const;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferView<const taPixel, taOrientation>
  : public PixelBufferConstImpl<TPixelBufferView<const taPixel, taOrientation>>
{
  using TThis                                      = TPixelBufferView<const taPixel, taOrientation>;
  using TTraits                                    = TPixelBufferTraits<TThis>;
  using TPixelBufferImpl                           = TPixelBufferImpl<TThis>;
  using TPixel                                     = typename TTraits::TPixel;
  using TPixelRef                                  = typename TTraits::TPixelRef;
  using TPixelConstRef                             = typename TTraits::TPixelConstRef;
  using TBuffer                                    = typename TTraits::TBuffer;
  using TConstBufferPtr                            = typename TTraits::TConstBufferPtr;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  TPixelBufferView(size_t aWidth, size_t aHeight, TConstBufferPtr buffer)
    : iPixelBuffer{buffer}
    , iWidth{aWidth}
    , iHeight{aHeight}
  {
    assert(aWidth > 0 && aHeight > 0);
  }

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return iWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return iHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return Width() * Height();
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified absolute index.
   * @note The method asserts that the coordinates are within the bounds of the pixel buffer.
   *       If the coordinates are out of bounds, it will trigger an assertion failure.
   * @note The method is noexcept, meaning it does not throw exceptions.
   *       It is expected to be used in performance-critical code where exceptions are not desired.
   * @note The method uses assertions to ensure that the coordinates are valid.
   *       This is useful during development to catch errors early.
   */
  inline constexpr TPixel GetPixel(size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    return iPixelBuffer[aIndex];
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferPtr GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

// TODO: implement support of compressed buffers

} // namespace AbstractPlatform
