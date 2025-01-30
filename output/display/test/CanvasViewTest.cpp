#include <gtest/gtest.h>

#include <AbstractPlatform/output/display/CanvasView.hpp>

#include <memory>
#include <utility>
#include <type_traits>
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

// template < typename taPixel, typename taPixelMatrix, size_t taWidth, size_t taHeight >
// constexpr std::unique_ptr< std::uint8_t[] >
// CreateRawBuffer( const taPixelMatrix ( & )[ taWidth ][ taHeight ] )
// {
//     // auto sixeOfBuffer = []( ) constexpr -> size_t
//     // {
//     //     return std::is_same< taPixel, TBitPixel >::value ? BufferSize( taWidth ) * taHeight;
//     //         : taWidth * 24/kBitsPerByte * taHeight;
//     // };
//     // return;
// }

// TEST( PageHelperBaseSeveritiTest, PixelShiftReturnsValidValues )
// {
// }

using TCanvasViewTestTypeList = testing::Types< TBitPixel, TRGBPixel >;
template < class >
struct CanvasViewTest : testing::Test
{
};

TYPED_TEST_SUITE( CanvasViewTest, TCanvasViewTestTypeList );

TYPED_TEST( CanvasViewTest, PixelShiftReturnsValidValues )
{
}