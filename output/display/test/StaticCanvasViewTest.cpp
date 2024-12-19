#include <gtest/gtest.h>

#include <AbstractPlatform/output/display/StaticCanvasView.hpp>

#include <utility>
#include <cstdint>

using namespace AbstractPlatform;
namespace
{
template < size_t taPixelPerPage >
struct PixelPerPageMock
{
    static constexpr size_t kPixelPerPage = taPixelPerPage;
    static constexpr size_t
    PixelsPerPage( )
    {
        return kPixelPerPage;
    }
};
};  // namespace

TEST( PageHelperBaseSeveritiTest, PixelShiftReturnsValidValues )
{
}

using TPageHelperBaseTestTestTypelist = testing::Types< PixelPerPageMock< 8 >,
                                                        PixelPerPageMock< 4 >,
                                                        PixelPerPageMock< 2 >,
                                                        PixelPerPageMock< 1 > >;
template < class >
struct PageHelperBaseTestMassTest : testing::Test
{
};

TYPED_TEST_SUITE( PageHelperBaseTestMassTest, TPageHelperBaseTestTestTypelist );

TYPED_TEST( PageHelperBaseTestMassTest, PixelShiftReturnsValidValues )
{
}

using TPageHelperTestTypelist
    = testing::Types< std::pair< AbstractPlatform::TBitPixel, std::uint8_t >,
                      std::pair< AbstractPlatform::TRGBPixel, std::uint8_t >,
                      std::pair< AbstractPlatform::TBitPixel, std::uint16_t >,
                      std::pair< AbstractPlatform::TRGBPixel, std::uint16_t >,
                      std::pair< AbstractPlatform::TBitPixel, std::uint32_t >,
                      std::pair< AbstractPlatform::TRGBPixel, std::uint32_t > >;
template < class >
struct PageHelperTest : testing::Test
{
};

TYPED_TEST_SUITE( PageHelperTest, TPageHelperTestTypelist );

// Demonstrate some basic assertions.
TYPED_TEST( PageHelperTest, PageHelper )
{
}
