#pragma once

#include <AbstractPlatform/output/display/AbstractDisplayDriver.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <memory>

namespace AbstractPlatform::PngDisplay {
void RegisterPngDisplayDriver();
} // namespace AbstractPlatform::PngDisplay
