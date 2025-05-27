#include <AbstractPlatform/output/display/AbstractCanvas.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/PixelBuffer.hpp>

#include <cstring>

namespace AbstractPlatform {
template <typename taPixel>
class TPixelBufferCanvas : public TAbstractCanvas
{
public:
  using TPixel = taPixel;

  TPixelBufferCanvas(size_t aWidth, size_t aHeight)
    : iPixelBuffer{aWidth, aHeight}
  {
  }

  int PixelWidth() const NOEXCEPT override
  {
    return static_cast<int>(iPixelBuffer.Width());
  }

  int PixelHeight() const NOEXCEPT override
  {
    return static_cast<int>(iPixelBuffer.Height());
  }

  void GetPixel(const TPosition&          aPosition,
                AbstractPlatform::TPixel& aPixelValue) const NOEXCEPT override
  {
    aPixelValue = iPixelBuffer.GetPixel(aPosition.iX, aPosition.iY);
  }

  void SetPixel(const TPosition&                aPosition,
                const AbstractPlatform::TPixel& aPixelValue) NOEXCEPT override
  {
    iPixelBuffer.SetPixel(aPosition.iX, aPosition.iY, aPixelValue);
  }

  void FillWith(const TPixel& aPixelValue) NOEXCEPT override
  {
    TPixel*       aPixelBuffer    = iPixelBuffer.GetBuffer();
    const TPixel* aPixelBufferEnd = aPixelBuffer + iPixelBuffer.GetPixels();
    for (; aPixelBuffer < aPixelBufferEnd; ++aPixelBuffer)
    {
      *aPixelBuffer = aPixelValue;
    }
  }

  void Clear() NOEXCEPT override
  {
    std::memset(iPixelBuffer.GetBuffer(), 0, iPixelBuffer.Size());
  }

  TPixelBuffer<TPixel> iPixelBuffer;
};

} // namespace AbstractPlatform
