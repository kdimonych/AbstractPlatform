#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/PixelBuffer.hpp>
#include <AbstractPlatform/output/display/Position.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <new>

namespace AbstractPlatform {

enum class TPlottingOrigin
{
  TopLeftCorner,
  BottomLeftCorner
};

class TAbstractCanvas
{
public:
  using TPixel       = AbstractPlatform::TPixel;
  using TPosition    = AbstractPlatform::TPosition;
  using TPixelBuffer = AbstractPlatform::TPixelBuffer<TPixel>;

  virtual ~TAbstractCanvas() = default;

  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  virtual int PixelWidth() const NOEXCEPT = 0;

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  virtual int PixelHeight() const NOEXCEPT = 0;

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @return TPixel The value of the pixel.
   */
  virtual void GetPixel(const TPosition& aPosition, TPixel& pixel) const NOEXCEPT = 0;

  /**
   * @brief Sets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @param TPixel A pixel value to set.
   *
   */
  virtual void SetPixel(const TPosition& aPosition, TPixel aPixelValue) NOEXCEPT = 0;

  /**
   * @brief Fills entire canvas with provided pixel value.
   *
   * @param TPixel A pixel value to fill the canvas with.
   */
  virtual void FillWith(TPixel aPixelValue) NOEXCEPT
  {
    TPosition  position{0, 0};
    const auto pixelWidth  = PixelWidth();
    const auto pixelHeight = PixelHeight();
    for (; position.iX < pixelWidth; ++position.iX)
    {
      for (; position.iY < pixelHeight; ++position.iY)
      {
        SetPixel(position, aPixelValue);
      }
    }
  }

  /**
   * @brief Clears the canvas
   */
  virtual void Clear() NOEXCEPT
  {
    FillWith(TPixel{});
  }

  virtual void FillFrom(const TPosition& aFillPosition, const TPixelBuffer& aPixelBuffer) NOEXCEPT
  {
    const auto pixelBufferWidth  = static_cast<int>(aPixelBuffer.Width());
    const auto pixelBufferHeight = static_cast<int>(aPixelBuffer.Height());

    assert(aFillPosition.iX < std::numeric_limits<int>::max() - 1 - pixelBufferWidth);
    assert(aFillPosition.iY < std::numeric_limits<int>::max() - 1 - pixelBufferHeight);
    assert(pixelBufferWidth + aFillPosition.iX >= std::numeric_limits<int>::min());
    assert(pixelBufferHeight + aFillPosition.iY >= std::numeric_limits<int>::min());

    auto overlapWidth  = std::min(std::min(pixelBufferWidth, pixelBufferWidth + aFillPosition.iX),
                                 PixelWidth() - aFillPosition.iX);
    auto overlapHeight = std::min(std::min(pixelBufferHeight, pixelBufferHeight + aFillPosition.iY),
                                  PixelHeight() - aFillPosition.iY);

    TPosition aSourceStart{std::max(0, -aFillPosition.iX), std::max(0, -aFillPosition.iY)};
    TPosition aSourceEnd{aSourceStart.iX + overlapWidth, aSourceStart.iY + overlapHeight};

    TPosition position = aFillPosition;

    for (; aSourceStart.iY < aSourceEnd.iY; ++position.iY, ++aSourceStart.iY)
    {
      for (; aSourceStart.iX < aSourceEnd.iX; ++position.iX, ++aSourceStart.iX)
      {
        SetPixel(position, aPixelBuffer.GetPixel(aSourceStart));
      }
    }
  }

  virtual void FillFrom(const TPosition& aFillPosition, const TAbstractCanvas& aCanvas) NOEXCEPT
  {
    const auto anotherCanvasWidth  = aCanvas.PixelWidth();
    const auto anotherCanvasHeight = aCanvas.PixelHeight();

    assert(aFillPosition.iX < std::numeric_limits<int>::max() - 1 - anotherCanvasWidth);
    assert(aFillPosition.iY < std::numeric_limits<int>::max() - 1 - anotherCanvasHeight);
    assert(anotherCanvasWidth + aFillPosition.iX >= std::numeric_limits<int>::min());
    assert(anotherCanvasHeight + aFillPosition.iY >= std::numeric_limits<int>::min());

    auto overlapWidth =
      std::min(std::min(anotherCanvasWidth, anotherCanvasWidth + aFillPosition.iX),
               PixelWidth() - aFillPosition.iX);
    auto overlapHeight =
      std::min(std::min(anotherCanvasHeight, anotherCanvasHeight + aFillPosition.iY),
               PixelHeight() - aFillPosition.iY);

    TPosition aSourceStart{std::max(0, -aFillPosition.iX), std::max(0, -aFillPosition.iY)};
    TPosition aSourceEnd{aSourceStart.iX + overlapWidth, aSourceStart.iY + overlapHeight};

    TPosition position = aFillPosition;
    TPixel    pixel;

    for (; aSourceStart.iY < aSourceEnd.iY; ++position.iY, ++aSourceStart.iY)
    {
      for (; aSourceStart.iX < aSourceEnd.iX; ++position.iX, ++aSourceStart.iX)
      {
        aCanvas.GetPixel(aSourceStart, pixel);
        SetPixel(position, pixel);
      }
    }
  }

  virtual void FillTo(const TPosition& aFillPosition, TPixelBuffer& aPixelBuffer) NOEXCEPT
  {
    const auto pixelBufferWidth  = static_cast<int>(aPixelBuffer.Width());
    const auto pixelBufferHeight = static_cast<int>(aPixelBuffer.Height());

    assert(aFillPosition.iX < std::numeric_limits<int>::max() - 1 - pixelBufferWidth);
    assert(aFillPosition.iY < std::numeric_limits<int>::max() - 1 - pixelBufferHeight);
    assert(pixelBufferWidth + aFillPosition.iX >= std::numeric_limits<int>::min());
    assert(pixelBufferHeight + aFillPosition.iY >= std::numeric_limits<int>::min());

    auto overlapWidth  = std::min(std::min(pixelBufferWidth, pixelBufferWidth + aFillPosition.iX),
                                 PixelWidth() - aFillPosition.iX);
    auto overlapHeight = std::min(std::min(pixelBufferHeight, pixelBufferHeight + aFillPosition.iY),
                                  PixelHeight() - aFillPosition.iY);

    TPosition aSourceStart{std::max(0, -aFillPosition.iX), std::max(0, -aFillPosition.iY)};
    TPosition aSourceEnd{aSourceStart.iX + overlapWidth, aSourceStart.iY + overlapHeight};

    TPosition position = aFillPosition;
    TPixel    pixel;

    for (; aSourceStart.iY < aSourceEnd.iY; ++position.iY, ++aSourceStart.iY)
    {
      for (; aSourceStart.iX < aSourceEnd.iX; ++position.iX, ++aSourceStart.iX)
      {
        GetPixel(position, pixel);
        aPixelBuffer.GetPixel(aSourceStart) = pixel;
      }
    }
  }
};

} // namespace AbstractPlatform
