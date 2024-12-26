#include <gtest/gtest.h>

#include <AbstractPlatform/common/BlockIterator.hpp>

#include <utility>
#include <cstdint>

using namespace AbstractPlatform;
namespace
{
}

TEST( BlockIterator, InitialStateTest )
{
    struct TByteTag;
    struct TWordTag;
    TBlockIterator< TBlock< TWordTag, 4 >, TBlock< TByteTag, 8 > > bi;

    EXPECT_EQ( bi.BlocksCount( ), 2 );
    EXPECT_EQ( bi.FullElementsCount( ), 4 * 8 );
    EXPECT_EQ( bi.BlockElementsCount< 0 >( ), 4 * 8 );
    EXPECT_EQ( bi.BlockElementsCount< 1 >( ), 8 );

    EXPECT_EQ( bi.BlockIndex< 0 >( ), 0 );
    EXPECT_EQ( bi.BlockIndex< 1 >( ), 0 );
}

TEST( BlockIterator, SetGlobalIndexTest )
{
    struct TByteTag;
    struct TWordTag;
    TBlockIterator< TBlock< TWordTag, 4 >, TBlock< TByteTag, 8 > > bi;

    // bi.SetGlobalIndex( 5 );

    EXPECT_EQ( bi.BlockIndex< 0 >( ), 0 );
    EXPECT_EQ( bi.BlockIndex< 1 >( ), 5 );
}