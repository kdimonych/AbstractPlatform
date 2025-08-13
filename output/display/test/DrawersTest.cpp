#include <AbstractPlatform/output/display/Drawers.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/buffer/PixelBufferView.hpp>
#include <AbstractPlatform/output/display/buffer/StaticPixelBuffer.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

namespace {

}; // namespace

using TPixelTypeList = testing::Types<TBitPixel, TRGBPixel, TRGBAPixel, TRGB565Pixel>;

template <class taTypeParam>
struct DrawersTest : testing::Test
{
  using TPixel = taTypeParam;
  // Support constexpr evaluation
};

TYPED_TEST_SUITE(DrawersTest, TPixelTypeList);

TYPED_TEST(DrawersTest, DefaultCreated)
{
  using TPixel = TypeParam;
  TStaticPixelBuffer<2, 4, TPixel> buffer;
  auto                             drawer = MakeDrawer(buffer);

  // TODO: implement tests for multiple PixelBuffer types
}
