#pragma once

#include <AbstractPlatform/output/display/Drawer.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

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

  virtual ~AbstractDisplayDriver() = default;
};

} // namespace AbstractPlatform
