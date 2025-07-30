#pragma once

#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

namespace AbstractPlatform {

template <typename taPixelBuffer>
class CDrawer
{
public:
  using TPixelBuffer = taPixelBuffer;
  using TPixel       = typename TPixelBuffer::TPixel;
  using TPosition    = typename TPixelBuffer::TPosition;

  TPixel iPixelValue;

  CDrawer(TPixelBuffer& aBuffer)
    : iPixelValue{}
    , iBuffer{aBuffer}
    , iPosition{}
  {
  }

  /**
   * @brief Clear the canvas via set all its pixel values to the default value.
   */
  inline void Clear()
  {
    iBuffer.Clear();
  }

  /**
   * @brief Fills entire canvas with provided pixel value.
   *
   * @param TPixel A pixel value to fill the canvas with.
   */
  void Fill()
  {
    iBuffer.FillWith(iPixelValue);
  }

  template <typename taPosition>
  void SetPosition(taPosition&& aPosition)
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < iBuffer.PixelWidth());
    assert(aPosition.iY < iBuffer.PixelHeight());

    iPosition = std::forward<taPosition>(aPosition);
  }

  TPosition GetPosition() const
  {
    return iPosition;
  }

  template <typename taPosition>
  void SetPixelValue(taPosition&& aPosition)
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < iBuffer.PixelWidth());
    assert(aPosition.iY < iBuffer.PixelHeight());

    iPosition = std::forward<taPosition>(aPosition);
  }

  /**
   * @brief Draws a line from point aFromPosition to aToPosition with a pixel value
   *        aPixelValue
   *
   * @param aFromPosition The {x, y} coordinates of the line origin.
   * @param aToPosition The {x, y} coordinates of the line destination.
   * @param aPixelValue A pixel value.
   */
  void DrawLineTo(TPosition aToPosition)
  {
    assert(aToPosition.iX >= 0);
    assert(aToPosition.iY >= 0);
    assert(aToPosition.iX < iBuffer.PixelWidth());
    assert(aToPosition.iY < iBuffer.PixelHeight());

    if (aToPosition.iX < iPosition.iX)
    {
      std::swap(aToPosition.iX, iPosition.iX);
    }
    if (aToPosition.iY < iPosition.iY)
    {
      std::swap(aToPosition.iX, iPosition.iX);
    }

    int dx  = std::abs(aToPosition.iX - iPosition.iX);
    int sx  = iPosition.iX < aToPosition.iX ? 1 : -1;
    int dy  = -std::abs(aToPosition.iY - iPosition.iY);
    int sy  = iPosition.iY < aToPosition.iY ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
      iBuffer.SetPixel(iPosition, iPixelValue);
      if (iPosition.iX == aToPosition.iX && iPosition.iY == aToPosition.iY)
      {
        break;
      }
      e2 = 2 * err;

      if (e2 >= dy)
      {
        err += dy;
        iPosition.iX += sx;
      }
      if (e2 <= dx)
      {
        err += dx;
        iPosition.iY += sy;
      }
    }
  }

private:
  TPixelBuffer& iBuffer;
  TPosition     iPosition;
};

} // namespace AbstractPlatform
