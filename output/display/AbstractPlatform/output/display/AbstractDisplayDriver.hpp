#pragma once

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

  // TODO: Implement the rest of the display driver interface
};

} // namespace AbstractPlatform
