#pragma once
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
  using TIterator       = typename TBuffer::iterator;
  using TConstIterator  = typename TBuffer::const_iterator;

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
  using TIterator        = typename TTraits::TIterator;
  using TConstIterator   = typename TTraits::TConstIterator;

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
   * @brief Returns the pointer to the pixel buffer.
   *
   * @return TPixel* The pointer to the pixel buffer.
   */
  inline constexpr TBufferRef GetBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferRef GetBuffer() const NOEXCEPT
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
    return iPixelBuffer.begin();
  }

  inline constexpr TConstIterator cbegin() const NOEXCEPT
  {
    return iPixelBuffer.cbegin();
  }

  inline constexpr TConstIterator begin() const NOEXCEPT
  {
    return iPixelBuffer.begin();
  }

  /**
   * @brief Get the end iterator
   *
   * @return constexpr TIterator The end iterator of the pixel buffer.
   * @note These method return an iterator to the end of the pixel buffer.
   */
  inline constexpr TIterator end() NOEXCEPT
  {
    return iPixelBuffer.end();
  }

  inline constexpr TConstIterator cend() const NOEXCEPT
  {
    return iPixelBuffer.cend();
  }

  inline constexpr TConstIterator end() const NOEXCEPT
  {
    return iPixelBuffer.end();
  }

  TBuffer iPixelBuffer;
};

// TODO: implement support of compressed buffers

} // namespace AbstractPlatform
