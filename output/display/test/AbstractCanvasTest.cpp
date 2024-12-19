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

// TEST( AbstractCanvasTest, PixelShiftReturnsValidValues )
// {
// }

using AbstractCanvasTestTypeList = testing::Types< TBitPixel, TRGBPixel >;

template < class >
struct AbstractDisplayTest : testing::Test
{
};

TYPED_TEST_SUITE( AbstractDisplayTest, AbstractCanvasTestTypeList );

TYPED_TEST( AbstractDisplayTest, MergeCanvas )
{
}