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

enum class TPixelCompression
{
  None,
  Compressed,
};

template <typename taPixel>
struct PixelBufferView;

template <typename taPixel>
struct PixelBufferTraits<PixelBufferView<taPixel>>
{
  using TPixel         = taPixel;
  using TIndex         = TPosition::TIndex;
  using TBuffer        = TPixel* const;
  using TConstBuffer   = const TPixel* const;
  using TIterator      = TPixel*;
  using TConstIterator = const TPixel*;
};

template <typename taPixel>
struct PixelBufferView : public PixelBufferImpl<PixelBufferView<taPixel>>
{
  using TThis          = PixelBufferView<taPixel>;
  using TTraits        = PixelBufferTraits<TThis>;
  using TPixel         = typename TTraits::TPixel;
  using TBuffer        = typename TTraits::TBuffer;
  using TConstBuffer   = typename TTraits::TConstBuffer;
  using TIterator      = typename TTraits::TIterator;
  using TConstIterator = typename TTraits::TConstIterator;

  PixelBufferView(size_t aWidth, size_t aHeight, TBuffer& buffer)
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
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBuffer& GetBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr TBuffer& GetBuffer() NOEXCEPT
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

  inline constexpr TConstIterator begin() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  inline constexpr TConstIterator cbegin() const NOEXCEPT
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

  inline constexpr TConstIterator end() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  inline constexpr TConstIterator cend() const NOEXCEPT
  {
    return iPixelBuffer + (iWidth * iHeight);
  }

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

} // namespace AbstractPlatform
