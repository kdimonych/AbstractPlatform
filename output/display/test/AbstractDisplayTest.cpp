#include <gtest/gtest.h>

#include <AbstractPlatform/output/display/CanvasView.hpp>

#include <utility>
#include <cstdint>

using namespace AbstractPlatform;
namespace
{

static constexpr size_t kTestWidth = kBitsPerByte;
static constexpr size_t kTestHeight = kBitsPerByte;

static constexpr std::uint8_t kTestPicture[ kTestWidth ][ kTestHeight ]
    = { { 1, 0, 0, 0, 0, 0, 1, 1 },  //
        { 0, 1, 1, 1, 1, 1, 0, 1 },  //
        { 1, 1, 0, 0, 0, 1, 1, 1 },  //
        { 1, 1, 1, 1, 1, 0, 1, 0 },  //
        { 1, 0, 1, 1, 0, 0, 1, 0 },  //
        { 1, 1, 0, 1, 1, 1, 0, 1 },  //
        { 1, 1, 1, 0, 0, 0, 1, 1 },  //
        { 1, 1, 1, 1, 1, 1, 1, 1 } };

};  // namespace

TEST( PageHelperBaseSeveritiTest, PixelShiftReturnsValidValues )
{
}

using AbstractDisplayTestTypeList = testing::Types< TBitPixel, TRGBPixel >;
template < class >
struct AbstractDisplayTest : testing::Test
{
};

TYPED_TEST_SUITE( AbstractDisplayTest, AbstractDisplayTestTypeList );

TYPED_TEST( AbstractDisplayTest, MergeCanvas )
{
}