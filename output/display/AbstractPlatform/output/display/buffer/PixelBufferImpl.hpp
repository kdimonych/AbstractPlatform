#pragma once
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
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

template <typename taPixelBuffer>
struct PixelBufferTraits;

template <typename taPixelBuffer>
struct PixelBufferConstImpl
{
  using TPixelBuffer   = taPixelBuffer;
  using TTraits        = PixelBufferTraits<taPixelBuffer>;
  using TPixel         = typename TTraits::TPixel;
  using TIndex         = typename TTraits::TIndex;
  using TIterator      = typename TTraits::TIterator;
  using TConstIterator = typename TTraits::TConstIterator;

  inline constexpr const TPixelBuffer* Base() const NOEXCEPT
  {
    return static_cast<const TPixelBuffer*>(this);
  }

  inline constexpr TPixelBuffer* Base() NOEXCEPT
  {
    return static_cast<TPixelBuffer*>(this);
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aX x coordinate of the pixel.
   * @param aY y coordinate of the pixel.
   * @return TPixel& The pixel at the specified coordinates.
   * @note The method asserts that the coordinates are within the bounds of the pixel buffer.
   *       If the coordinates are out of bounds, it will trigger an assertion failure.
   * @note The method is noexcept, meaning it does not throw exceptions.
   *       It is expected to be used in performance-critical code where exceptions are not desired.
   * @note The method uses assertions to ensure that the coordinates are valid.
   *       This is useful during development to catch errors early.
   */
  inline constexpr const TPixel& GetPixel(TIndex aX, TIndex aY) const NOEXCEPT
  {
    assert(aX >= 0);
    assert(aY >= 0);
    assert(aX < static_cast<int>(Base()->Width()));
    assert(aY < static_cast<int>(Base()->Height()));
    return *(Base()->begin() + aY * Base()->Width() + aX);
  }

  inline constexpr const TPixel& GetPixel(const TPosition& aPosition) const NOEXCEPT
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < static_cast<int>(Base()->Width()));
    assert(aPosition.iY < static_cast<int>(Base()->Height()));

    return *(Base()->begin() + aPosition.iY * Base()->Width() + aPosition.iX);
  }

  inline constexpr const TPixel& operator()(TIndex aX, TIndex aY) const NOEXCEPT
  {
    return GetPixel(aX, aY);
  }

  inline constexpr const TPixel& operator()(const TPosition& aPosition) const NOEXCEPT
  {
    return GetPixel(aPosition);
  }

  inline constexpr const TPixel& operator[](size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Base()->Width() * Base()->Height());
    return *(Base()->begin() + aIndex);
  }

  /**
   * @brief Returns the size of the pixel buffer in bytes.
   *
   * @return size_t The size of the pixel buffer in bytes.
   */
  inline constexpr size_t Size() const NOEXCEPT
  {
    return Base()->Width() * Base()->Height() * sizeof(TPixel);
  }

  /**
   * @brief Gets the number of pixels in the pixel buffer.
   *
   * @return constexpr int The number of pixels in the pixel buffer.
   */
  inline constexpr size_t Pixels() const NOEXCEPT
  {
    return Base()->Width() * Base()->Height();
  }

  /**
   * @brief Get the begin iterator for a specific pixel.
   *
   * @param aY The pixel row index.
   * @param aX The pixel column index.
   * @return constexpr TConstIterator The begin iterator for the specified pixel.
   */
  inline constexpr TConstIterator StartFrom(TIndex aX, TIndex aY) const NOEXCEPT
  {
    assert(aX >= 0);
    assert(aX < static_cast<int>(Base()->Width()));
    assert(aY >= 0);
    assert(aY < static_cast<int>(Base()->Height()));
    return Base()->begin() + (aY * Base()->Width()) + aX;
  }
};

template <typename taPixelBuffer>
struct PixelBufferImpl : public PixelBufferConstImpl<taPixelBuffer>
{
  using TPixelBuffer   = taPixelBuffer;
  using TTraits        = PixelBufferTraits<taPixelBuffer>;
  using TPixel         = typename TTraits::TPixel;
  using TIndex         = typename TTraits::TIndex;
  using TIterator      = typename TTraits::TIterator;
  using TConstIterator = typename TTraits::TConstIterator;

  inline constexpr const TPixelBuffer* Base() const NOEXCEPT
  {
    return static_cast<const TPixelBuffer*>(this);
  }

  inline constexpr TPixelBuffer* Base() NOEXCEPT
  {
    return static_cast<TPixelBuffer*>(this);
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aX x coordinate of the pixel.
   * @param aY y coordinate of the pixel.
   * @return TPixel& The pixel at the specified coordinates.
   * @note The method asserts that the coordinates are within the bounds of the pixel buffer.
   *       If the coordinates are out of bounds, it will trigger an assertion failure.
   * @note The method is noexcept, meaning it does not throw exceptions.
   *       It is expected to be used in performance-critical code where exceptions are not desired.
   * @note The method uses assertions to ensure that the coordinates are valid.
   *       This is useful during development to catch errors early.
   */
  inline constexpr TPixel& GetPixel(TIndex aX, TIndex aY) NOEXCEPT
  {
    assert(aX >= 0);
    assert(aY >= 0);
    assert(aX < static_cast<int>(Base()->Width()));
    assert(aY < static_cast<int>(Base()->Height()));
    return *(Base()->begin() + aY * Base()->Width() + aX);
  }

  inline constexpr TPixel& GetPixel(const TPosition& aPosition) NOEXCEPT
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < static_cast<int>(Base()->Width()));
    assert(aPosition.iY < static_cast<int>(Base()->Height()));

    return *(Base()->begin() + aPosition.iY * Base()->Width() + aPosition.iX);
  }

  inline constexpr TPixel& operator()(TIndex aX, TIndex aY) NOEXCEPT
  {
    return GetPixel(aX, aY);
  }

  inline constexpr TPixel& operator()(const TPosition& aPosition) NOEXCEPT
  {
    return GetPixel(aPosition);
  }

  inline constexpr TPixel& operator[](size_t aIndex) NOEXCEPT
  {
    assert(aIndex < Base()->Width() * Base()->Height());
    return *(Base()->begin() + aIndex);
  }

  /**
   * @brief Get the begin iterator for a specific pixel.
   *
   * @param aY The pixel row index.
   * @param aX The pixel column index.
   * @return constexpr TConstIterator The begin iterator for the specified pixel.
   */
  inline constexpr TIterator StartFrom(TIndex aX, TIndex aY) NOEXCEPT
  {
    assert(aX >= 0);
    assert(aX < static_cast<int>(Base()->Width()));
    assert(aY >= 0);
    assert(aY < static_cast<int>(Base()->Height()));
    return Base()->begin() + (aY * Base()->Width()) + aX;
  }
};

} // namespace AbstractPlatform
