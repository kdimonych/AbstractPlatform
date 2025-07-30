#include <AbstractPlatform/output/display/buffer/PixelBufferView.hpp>
#include <AbstractPlatform/output/display/buffer/StaticPixelBuffer.hpp>

#include <gtest/gtest.h>

#include <array>
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

template <class taTypeParam>
struct PixelBufferViewTest : testing::Test
{
  using TPixel = taTypeParam;
  // Support constexpr evaluation
};

TYPED_TEST_SUITE(StaticPixelTest, TPixelTypeList);
TYPED_TEST_SUITE(PixelBufferViewTest, TPixelTypeList);

/*================== Tests ====================*/
TYPED_TEST(StaticPixelTest, DefaultCreatedValueIsZero)
{
  // TODO: implement the DefaultCreatedValueIsZero test correctly
  using TPixel = TypeParam;
  TPixel pixel;

  StaticPixelBuffer<2, 2, TPixel> buffer{TPixel{}, TPixel{}, TPixel{}, TPixel{}};
  EXPECT_EQ(buffer.GetBuffer()[0].Pack(), 0u);

  EXPECT_EQ(pixel.Pack(), 0u);
}

TYPED_TEST(PixelBufferViewTest, DefaultCreatedValueIsZero)
{
  // TODO: implement the DefaultCreatedValueIsZero test correctly
  using TPixel = TypeParam;
  TPixel pixel;

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  std::array<TPixel, kWidth * kHeight> rawBuffer{TPixel{}, TPixel{}, TPixel{}, TPixel{}};
  PixelBufferView<TPixel>              buffer{kWidth, kHeight, rawBuffer.data()};
  EXPECT_EQ(buffer.GetBuffer()[0].Pack(), 0u);

  EXPECT_EQ(pixel.Pack(), 0u);
}

// TODO: implement rest of the tests
