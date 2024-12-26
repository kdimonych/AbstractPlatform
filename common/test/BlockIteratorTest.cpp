#include <gtest/gtest.h>

#include <AbstractPlatform/common/BlockIterator.hpp>

#include <utility>
#include <cstdint>
#include <limits>

using namespace AbstractPlatform;
namespace
{
}

TEST( BlockIterator, BlockInitialStateTest )
{
    static constexpr size_t kBlockElements = 8;
    struct TByteTag;
    TBlock< TByteTag, kBlockElements > bl;

    EXPECT_EQ( bl.iIndex, 0 );
    EXPECT_EQ( bl.kDirection, TBlockIteratorDirection::BlockForwardIterator );
    EXPECT_EQ( bl.kSize, kBlockElements );
    EXPECT_EQ( bl.GetForwardIndex( ), 0 );
}

TEST( BlockIterator, BackwardBlockInitialStateTest )
{
    static constexpr size_t kBlockElements = 8;
    struct TByteTag;
    TBlock< TByteTag, kBlockElements, TBlockIteratorDirection::BlockBackwardIterator > bl;

    EXPECT_EQ( bl.iIndex, kBlockElements - 1 );
    EXPECT_EQ( bl.kDirection, TBlockIteratorDirection::BlockBackwardIterator );
    EXPECT_EQ( bl.kSize, kBlockElements );
    EXPECT_EQ( bl.GetForwardIndex( ), 0 );
}

TEST( BlockIterator, ForwardBlockSetForwardIndexTest )
{
    static constexpr size_t kBlockElements = 8;
    static constexpr size_t kIndex5 = 5;
    static constexpr size_t kIndexMin = std::numeric_limits< size_t >::min( );
    static constexpr size_t kIndexMax = std::numeric_limits< size_t >::max( );

    struct TByteTag;
    TBlock< TByteTag, kBlockElements, TBlockIteratorDirection::BlockForwardIterator > bl;

    bl.SetForwardIndex( kIndex5 );
    EXPECT_EQ( bl.iIndex, kIndex5 );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndex5 );

    bl.SetForwardIndex( kBlockElements );
    EXPECT_EQ( bl.iIndex, kBlockElements );
    EXPECT_EQ( bl.GetForwardIndex( ), kBlockElements );

    bl.SetForwardIndex( kIndexMin );
    EXPECT_EQ( bl.iIndex, kIndexMin );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndexMin );

    bl.SetForwardIndex( kIndexMax );
    EXPECT_EQ( bl.iIndex, kIndexMax );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndexMax );
}

TEST( BlockIterator, BackwardBlockSetForwardIndexTest )
{
    static constexpr size_t kBlockElements = 8;
    static constexpr size_t kIndex5 = 5;
    static constexpr size_t kIndexMin = std::numeric_limits< size_t >::min( );
    static constexpr size_t kIndexMax = std::numeric_limits< size_t >::max( );

    struct TByteTag;
    TBlock< TByteTag, kBlockElements, TBlockIteratorDirection::BlockBackwardIterator > bl;

    bl.SetForwardIndex( kIndex5 );
    EXPECT_EQ( bl.iIndex, ( kBlockElements - 1 ) - kIndex5 );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndex5 );

    bl.SetForwardIndex( kBlockElements );
    EXPECT_EQ( bl.iIndex, ( kBlockElements - 1 ) - kBlockElements );
    EXPECT_EQ( bl.GetForwardIndex( ), kBlockElements );

    bl.SetForwardIndex( kIndexMin );
    EXPECT_EQ( bl.iIndex, ( kBlockElements - 1 ) - kIndexMin );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndexMin );

    bl.SetForwardIndex( kIndexMax );
    EXPECT_EQ( bl.iIndex, ( kBlockElements - 1 ) - kIndexMax );
    EXPECT_EQ( bl.GetForwardIndex( ), kIndexMax );
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

    // EXPECT_EQ( bi.BlockIndex< 0 >( ), 0 );
    // EXPECT_EQ( bi.BlockIndex< 1 >( ), 5 );
}