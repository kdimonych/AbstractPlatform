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
class AbstractDisplayDriver;

template <typename taPixelValue>
class AbstractDisplayDriver
{
public:
  using TPixelValue = taPixelValue;
  using TCanvas     = AbstractPlatform::TCanvas<TPixelValue>;

  virtual ~AbstractDisplayDriver() = default;

  virtual TCanvas CreateCanvas(size_t aWidth, size_t aHeight) const NOEXCEPT = 0;

  virtual void Show(TCanvas aCanvas) NOEXCEPT = 0;
};

} // namespace AbstractPlatform
