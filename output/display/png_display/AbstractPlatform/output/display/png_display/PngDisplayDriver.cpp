#include <AbstractPlatform/output/display/png_display/PngDisplayDriver.hpp>

#include <iostream>

namespace AbstractPlatform::PngDisplay {
void RegisterPngDisplayDriver()
{
  std::cout << "Registering PNG Display Driver" << std::endl;
}
} // namespace AbstractPlatform::PngDisplay
