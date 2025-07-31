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
struct PixelBufferView;

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferTraits<PixelBufferView<taPixel, taOrientation>>
{
  using TPixel                                     = taPixel;
  using TBuffer                                    = TPixel* const;
  using TBufferPtr                                 = TPixel* const;
  using TConstBufferPtr                            = const TPixel* const;
  using TIterator                                  = TPixel*;
  using TConstIterator                             = const TPixel*;
  static constexpr TBufferOrientation kOrientation = TBufferOrientation::Horizontal;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct PixelBufferView : public PixelBufferImpl<PixelBufferView<taPixel, taOrientation>>
{
  using TThis                                      = PixelBufferView<taPixel, taOrientation>;
  using TTraits                                    = TPixelBufferTraits<TThis>;
  using TPixel                                     = typename TTraits::TPixel;
  using TBuffer                                    = typename TTraits::TBuffer;
  using TBufferPtr                                 = typename TTraits::TBufferPtr;
  using TConstBufferPtr                            = typename TTraits::TConstBufferPtr;
  using TIterator                                  = typename TTraits::TIterator;
  using TConstIterator                             = typename TTraits::TConstIterator;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using PixelBufferImpl<TThis>::PixelBufferImpl;

  PixelBufferView(size_t aWidth, size_t aHeight, TBufferPtr buffer)
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
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferPtr GetBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr TBufferPtr GetBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the pixel buffer data pointer
   *
   * @return const TPixel* Pointer to the pixel buffer data.
   */
  inline constexpr const TPixel* GetData() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  inline constexpr const TPixel* GetData() NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the begin iterator
   *
   * @return TIterator The begin iterator of the pixel buffer.
   * @note These method return an iterator to the first pixel in the buffer.
   */
  inline constexpr TIterator begin() NOEXCEPT
  {
    return iPixelBuffer;
  }

  inline constexpr TConstIterator cbegin() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  inline constexpr TConstIterator begin() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the end iterator
   *
   * @return constexpr TIterator The end iterator of the pixel buffer.
   * @note These method return an iterator to the end of the pixel buffer.
   */
  inline constexpr TIterator end() NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  inline constexpr TConstIterator cend() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  inline constexpr TConstIterator end() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct TPixelBufferTraits<PixelBufferView<const taPixel, taOrientation>>
{
  using TPixel                                     = taPixel;
  using TBuffer                                    = const TPixel* const;
  using TConstBufferPtr                            = const TPixel* const;
  using TConstIterator                             = const TPixel*;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <typename taPixel, TBufferOrientation taOrientation>
struct PixelBufferView<const taPixel, taOrientation>
  : public PixelBufferConstImpl<PixelBufferView<const taPixel, taOrientation>>
{
  using TThis                                      = PixelBufferView<const taPixel, taOrientation>;
  using TTraits                                    = TPixelBufferTraits<TThis>;
  using TPixel                                     = typename TTraits::TPixel;
  using TBuffer                                    = typename TTraits::TBuffer;
  using TConstBufferPtr                            = typename TTraits::TConstBufferPtr;
  using TConstIterator                             = typename TTraits::TConstIterator;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using PixelBufferConstImpl<TThis>::PixelBufferImpl;

  PixelBufferView(size_t aWidth, size_t aHeight, TConstBufferPtr buffer)
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
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferPtr GetBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the pixel buffer data pointer
   *
   * @return const TPixel* Pointer to the pixel buffer data.
   */
  inline constexpr const TPixel* GetData() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the begin iterator
   *
   * @return TIterator The begin iterator of the pixel buffer.
   * @note These method return an iterator to the first pixel in the buffer.
   */
  inline constexpr TConstIterator cbegin() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  inline constexpr TConstIterator begin() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the end iterator
   *
   * @return constexpr TIterator The end iterator of the pixel buffer.
   * @note These method return an iterator to the end of the pixel buffer.
   */
  inline constexpr TConstIterator cend() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  inline constexpr TConstIterator end() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

// TODO: implement support of compressed buffers

} // namespace AbstractPlatform
