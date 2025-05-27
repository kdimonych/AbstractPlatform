#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/AbstractCanvas.hpp>
#include <AbstractPlatform/output/display/AbstractDisplayDriver.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform::PngDisplay {
void RegisterPngDisplayDriver();
} // namespace AbstractPlatform::PngDisplay
