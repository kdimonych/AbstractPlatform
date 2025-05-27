#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/AbstractCanvas.hpp>
#include <AbstractPlatform/output/display/Drawer.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform {

template <typename taPixelValue>
class AbstarctDisplayDriver;

template <typename taPixelValue>
class TCanvas
{
public:
  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  inline constexpr int PixelWidth() const NOEXCEPT
  {
    assert(iImpl != nullptr);
    return iImpl->PixelWidth();
  }

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  inline constexpr int PixelHeight() const NOEXCEPT
  {
    assert(iImpl != nullptr);
    return iImpl->PixelHeight();
  }

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @return taPixelValue The value of the pixel.
   */
  inline constexpr void GetPixel(const TPosition& aPosition, taPixelValue& pixel) const NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->GetPixel(aPosition, pixel);
  }

  /**
   * @brief Sets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @param taPixelValue A pixel value to set.
   */
  inline constexpr void SetPixel(const TPosition& aPosition, taPixelValue aPixelValue) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->SetPixel(aPosition, aPixelValue);
  }

  inline constexpr void FillWith(taPixelValue aPixelValue) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->FillWith(aPixelValue);
  }

  inline constexpr void Clear() NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->Clear();
  }

  inline constexpr void FillFrom(const TPosition&                  aFillPosition,
                                 const TPixelBuffer<taPixelValue>& aPixelBuffer) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->FillFrom(aFillPosition, aPixelBuffer);
  }

  inline constexpr void FillFrom(const TPosition& aFillPosition, const TCanvas& aCanvas) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->FillFrom(aFillPosition, *aCanvas.iImpl.get());
  }

  inline constexpr void FillTo(const TPosition&            aFillPosition,
                               TPixelBuffer<taPixelValue>& aPixelBuffer) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->FillTo(aFillPosition, aPixelBuffer);
  }

  inline constexpr void FillTo(const TPosition& aFillPosition, TCanvas& aCanvas) NOEXCEPT
  {
    assert(iImpl != nullptr);
    iImpl->FillTo(aFillPosition, *aCanvas.iImpl.get());
  }

private:
  friend class AbstarctDisplayDriver<taPixelValue>;
  std::unique_ptr<TAbstractCanvas> iImpl;
};

template <typename taPixelValue>
class AbstarctDisplayDriver
{
public:
  using TPixelValue = taPixelValue;
  using TCanvas     = AbstractPlatform::TCanvas<TPixelValue>;

  virtual ~AbstarctDisplayDriver() = default;

  virtual TCanvas CreateCanvas(size_t aWidth, size_t aHeight) const NOEXCEPT = 0;

  virtual void Show(TCanvas aCanvas) NOEXCEPT = 0;
};

} // namespace AbstractPlatform
