#include <AbstractPlatform/output/display/png_display/PngDisplayDriver.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <utility>

using namespace AbstractPlatform;

namespace {

}; // namespace

// TEST( AbstractCanvasTest, PixelShiftReturnsValidValues )
// {
// }

using AbstractCanvasTestTypeList = testing::Types<TBitPixel, TRGBPixel>;

template <class>
struct AbstractDisplayTest : testing::Test
{
};

TYPED_TEST_SUITE(AbstractDisplayTest, AbstractCanvasTestTypeList);

TYPED_TEST(AbstractDisplayTest, MergeCanvas) { }
