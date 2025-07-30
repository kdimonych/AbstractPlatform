#include <AbstractPlatform/output/display/Pixel.hpp>

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
struct PixelGroupedTest : testing::Test
{
  using TPixel = taTypeParam;
  // Support constexpr evaluation
  static_assert(std::is_default_constructible_v<TPixel>, "TPixel should be default constructible");
  static_assert(std::is_trivially_destructible_v<TPixel>,
                "TPixel should be trivially destructible");
  static_assert(std::is_trivially_copy_constructible_v<TPixel>,
                "TPixel should be trivially copy constructible");
  static_assert(std::is_trivially_copy_assignable_v<TPixel>,
                "TPixel should be trivially copy assignable");
  static_assert(std::is_trivially_move_constructible_v<TPixel>,
                "TPixel should be trivially move constructible");
  static_assert(std::is_trivially_move_assignable_v<TPixel>,
                "TPixel should be trivially move assignable");

  static_assert(std::is_same_v<decltype(TPixel::Bits()), size_t>,
                "TPixel::Bits() should return size_t");
  static_assert(std::is_same_v<decltype(TPixel::Mask()), std::uint32_t>,
                "TPixel::Mask() should return std::uint32_t");
  static_assert(std::is_same_v<decltype(std::declval<TPixel>().Pack()), std::uint32_t>,
                "TPixel::Pack() should return std::uint32_t");
  static_assert(std::is_same_v<decltype(std::declval<TPixel>().Unpack(0u)), void>,
                "TPixel::Unpack() should return void");
};

TYPED_TEST_SUITE(PixelGroupedTest, TPixelTypeList);

TYPED_TEST(PixelGroupedTest, DefaultCreatedValueIsZero)
{
  using TPixel = TypeParam;
  TPixel pixel;

  EXPECT_EQ(pixel.Pack(), 0u);
}

TYPED_TEST(PixelGroupedTest, PixelTest_PackUnpackAreReversable)
{
  using TPixel = TypeParam;
  TPixel pixel;

  std::vector<std::uint32_t> testValues = {0x00000000u,
                                           0x00000001u,
                                           0x00000002u,
                                           0x00000003u,
                                           0x12345678u,
                                           0x87654321u,
                                           0xFFFFFFFEu,
                                           0xFFFFFFFFu,
                                           0xABCDEF12u};

  for (const auto& value : testValues)
  {
    pixel.Unpack(value);
    EXPECT_EQ(pixel.Pack(), (value & TPixel::Mask()))
      << " for value: " << std::hex << value << std::dec << " (" << value << ")";
  }
}

TEST(PixelTest, BitPixelPack)
{
  TBitPixel bitPixel;

  bitPixel.iPixelValue = true;
  EXPECT_EQ(bitPixel.Pack(), true) << "Packing true should return true";

  bitPixel.iPixelValue = false;
  EXPECT_EQ(bitPixel.Pack(), false) << "Packing true should return true";
}

TEST(PixelTest, BitPixelUnpack)
{
  TBitPixel bitPixel;

  bitPixel.Unpack(0x1u);
  EXPECT_EQ(bitPixel.iPixelValue, true) << "Unpacking 0x1 should set pixel value to true";
  bitPixel.Unpack(0x0u);
  EXPECT_EQ(bitPixel.iPixelValue, false) << "Unpacking 0x0 should set pixel value to false";
  bitPixel.Unpack(0xFFFFFFFFu);
  EXPECT_EQ(bitPixel.iPixelValue, true) << "Unpacking 0xFFFFFFFF should set pixel value to true";
  bitPixel.Unpack(0xFFFFFFFEu);
  EXPECT_EQ(bitPixel.iPixelValue, false) << "Unpacking 0xFFFFFFFE should set pixel value to false";
}

// TEST(PixelTest, RGBPixelPack)
// {
//   TRGBPixel bitPixel;
//   auto testCollor = [](std::uint8_t& aColor, size_t shift){

//     return ;
//   };

//   bitPixel.iPixelValue = true;
//   EXPECT_EQ(bitPixel.Pack(), true) << "Packing true should return true";

//   bitPixel.iPixelValue = false;
//   EXPECT_EQ(bitPixel.Pack(), false) << "Packing true should return true";
// }

// TEST(PixelTest, RGBPixelUnpack)
// {
//   TRGBPixel bitPixel;

//   bitPixel.Unpack(0x1u);
//   EXPECT_EQ(bitPixel.iPixelValue, true) << "Unpacking 0x1 should set pixel value to true";
//   bitPixel.Unpack(0x0u);
//   EXPECT_EQ(bitPixel.iPixelValue, false) << "Unpacking 0x0 should set pixel value to false";
//   bitPixel.Unpack(0xFFFFFFFFu);
//   EXPECT_EQ(bitPixel.iPixelValue, true) << "Unpacking 0xFFFFFFFF should set pixel value to true";
//   bitPixel.Unpack(0xFFFFFFFEu);
//   EXPECT_EQ(bitPixel.iPixelValue, false) << "Unpacking 0xFFFFFFFE should set pixel value to
//   false";
// }
