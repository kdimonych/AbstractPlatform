#include <AbstractPlatform/output/display/StaticPixelBuffer.hpp>

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
struct StaticPixelTest : testing::Test
{
  using TPixel = taTypeParam;
  // Support constexpr evaluation
};

TYPED_TEST_SUITE(StaticPixelTest, TPixelTypeList);

TYPED_TEST(StaticPixelTest, DefaultCreatedValueIsZero)
{
  using TPixel = TypeParam;
  TPixel pixel;

  StaticPixelBuffer<2, 2, TPixel> buffer{TPixel{}, TPixel{}, TPixel{}, TPixel{}};
  EXPECT_EQ(buffer.GetBuffer()[0].Pack(), 0u);

  EXPECT_EQ(pixel.Pack(), 0u);
}
