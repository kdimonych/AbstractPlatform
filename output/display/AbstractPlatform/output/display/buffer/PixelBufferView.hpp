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
  using TPixel          = taPixel;
  using TIndex          = TPosition::TIndex;
  using TBuffer         = TPixel* const;
  using TBufferPtr      = TPixel* const;
  using TConstBufferPtr = const TPixel* const;
  using TIterator       = TPixel*;
  using TConstIterator  = const TPixel*;
};

template <typename taPixel>
struct PixelBufferView : public PixelBufferImpl<PixelBufferView<taPixel>>
{
  using TThis           = PixelBufferView<taPixel>;
  using TTraits         = PixelBufferTraits<TThis>;
  using TPixel          = typename TTraits::TPixel;
  using TBuffer         = typename TTraits::TBuffer;
  using TBufferPtr      = typename TTraits::TBufferPtr;
  using TConstBufferPtr = typename TTraits::TConstBufferPtr;
  using TIterator       = typename TTraits::TIterator;
  using TConstIterator  = typename TTraits::TConstIterator;

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
    return iPixelBuffer.data();
  }

  inline constexpr const TPixel* GetData() NOEXCEPT
  {
    return iPixelBuffer.data();
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

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

template <typename taPixel>
struct PixelBufferTraits<PixelBufferView<const taPixel>>
{
  using TPixel          = taPixel;
  using TIndex          = TPosition::TIndex;
  using TBuffer         = const TPixel* const;
  using TConstBufferPtr = const TPixel* const;
  using TConstIterator  = const TPixel*;
};

template <typename taPixel>
struct PixelBufferView<const taPixel> : public PixelBufferConstImpl<PixelBufferView<const taPixel>>
{
  using TThis           = PixelBufferView<const taPixel>;
  using TTraits         = PixelBufferTraits<TThis>;
  using TPixel          = typename TTraits::TPixel;
  using TBuffer         = typename TTraits::TBuffer;
  using TConstBufferPtr = typename TTraits::TConstBufferPtr;
  using TConstIterator  = typename TTraits::TConstIterator;

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
    return iPixelBuffer.data();
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

  TBuffer      iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

} // namespace AbstractPlatform
