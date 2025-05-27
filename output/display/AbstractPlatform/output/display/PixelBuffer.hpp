#pragma once
#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>

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

template <typename taPixel = AbstractPlatform::TPixel>
struct TPixelBuffer
{
  using TPixel = taPixel;

  TPixelBuffer(size_t aWidth, size_t aHeight)
    : iWidth{aWidth}
    , iHeight{aHeight}
    , iPixelBuffer{std::make_unique<TPixel[]>(aWidth * aHeight)}
  {
    assert(aWidth > 0 && aHeight > 0);
    assert(iPixelBuffer != nullptr);
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
  inline constexpr TPixel& GetPixel(size_t aX, size_t aY) NOEXCEPT
  {
    assert(aX < iWidth);
    assert(aY < iHeight);
    return iPixelBuffer[aY * iWidth + aX];
  }

  inline constexpr TPixel& GetPixel(const TPosition& aPosition) NOEXCEPT
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < static_cast<int>(iWidth));
    assert(aPosition.iY < static_cast<int>(iHeight));

    return iPixelBuffer[aPosition.iY * iWidth + aPosition.iX];
  }

  inline constexpr const TPixel& GetPixel(size_t aX, size_t aY) const NOEXCEPT
  {
    assert(aX < iWidth);
    assert(aY < iHeight);
    return iPixelBuffer[aY * iWidth + aX];
  }

  inline constexpr const TPixel& GetPixel(const TPosition& aPosition) const NOEXCEPT
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < static_cast<int>(iWidth));
    assert(aPosition.iY < static_cast<int>(iHeight));

    return iPixelBuffer[aPosition.iY * iWidth + aPosition.iX];
  }

  inline constexpr TPixel& operator()(size_t aX, size_t aY) NOEXCEPT
  {
    return GetPixel(aX, aY);
  }

  inline constexpr TPixel& operator()(const TPosition& aPosition) NOEXCEPT
  {
    return GetPixel(aPosition);
  }

  inline constexpr const TPixel& operator()(size_t aX, size_t aY) const NOEXCEPT
  {
    return GetPixel(aX, aY);
  }

  inline constexpr const TPixel& operator()(const TPosition& aPosition) const NOEXCEPT
  {
    return GetPixel(aPosition);
  }

  inline constexpr const TPixel& operator[](size_t aIndex) NOEXCEPT
  {
    assert(aIndex < iWidth * iHeight);
    return iPixelBuffer[aIndex];
  }

  inline constexpr const TPixel& operator[](size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < iWidth * iHeight);
    return iPixelBuffer[aIndex];
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
   * @brief Returns the size of the pixel buffer in bytes.
   *
   * @return size_t The size of the pixel buffer in bytes.
   */
  inline constexpr size_t Size() const NOEXCEPT
  {
    return iWidth * iHeight * sizeof(TPixel);
  }

  /**
   * @brief Gets the number of pixels in the pixel buffer.
   *
   * @return constexpr int The number of pixels in the pixel buffer.
   */
  inline constexpr size_t GetPixels() const NOEXCEPT
  {
    return iWidth * iHeight;
  }

  /**
   * @brief Returns the pointer to the pixel buffer.
   *
   * @return TPixel* The pointer to the pixel buffer.
   */
  inline constexpr TPixel* GetBuffer() NOEXCEPT
  {
    assert(iPixelBuffer != nullptr);
    return iPixelBuffer.get();
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TPixel* GetBuffer() const NOEXCEPT
  {
    return iPixelBuffer.get();
  }

  const size_t                    iWidth  = 0u;
  const size_t                    iHeight = 0u;
  const std::unique_ptr<TPixel[]> iPixelBuffer{nullptr};
};

} // namespace AbstractPlatform
