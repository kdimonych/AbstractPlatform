#include <gtest/gtest.h>

#include <AbstractPlatform/common/TensorIterator.hpp>

#include <utility>
#include <cstdint>
#include <limits>

using namespace AbstractPlatform;
namespace
{
}

TEST( TensorIterator, DimentionInitialStateTest )
{
    static constexpr size_t kDimensionSize = 8;
    struct TByteTag;
    TDimension< TByteTag, kDimensionSize > dimension;

    EXPECT_EQ( dimension.iPosition, 0 );
    EXPECT_EQ( dimension.kIterationDirection, TIterationDirection::Forward );
    EXPECT_EQ( dimension.kSize, kDimensionSize );
    EXPECT_EQ( dimension.GetForwardPosition( ), 0 );
}

TEST( TensorIterator, BackwardDimentionInitialStateTest )
{
    static constexpr size_t kDimensionSize = 8;
    struct TByteTag;
    TDimension< TByteTag, kDimensionSize, TIterationDirection::Backward > dimension;

    EXPECT_EQ( dimension.iPosition, kDimensionSize - 1 );
    EXPECT_EQ( dimension.kIterationDirection, TIterationDirection::Backward );
    EXPECT_EQ( dimension.kSize, kDimensionSize );
    EXPECT_EQ( dimension.GetForwardPosition( ), 0 );
}

TEST( TensorIterator, ForwardDimentionSetForwardIndexTest )
{
    static constexpr size_t kDimensionSize = 8;
    static constexpr size_t kIndex5 = 5;
    static constexpr size_t kIndexMin = std::numeric_limits< size_t >::min( );
    static constexpr size_t kIndexMax = std::numeric_limits< size_t >::max( );

    struct TByteTag;
    TDimension< TByteTag, kDimensionSize, TIterationDirection::Forward > dimension;

    dimension.SetForwardPosition( kIndex5 );
    EXPECT_EQ( dimension.iPosition, kIndex5 );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndex5 );

    dimension.SetForwardPosition( kDimensionSize );
    EXPECT_EQ( dimension.iPosition, kDimensionSize );
    EXPECT_EQ( dimension.GetForwardPosition( ), kDimensionSize );

    dimension.SetForwardPosition( kIndexMin );
    EXPECT_EQ( dimension.iPosition, kIndexMin );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndexMin );

    dimension.SetForwardPosition( kIndexMax );
    EXPECT_EQ( dimension.iPosition, kIndexMax );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndexMax );
}

TEST( TensorIterator, BackwardDimentionSetForwardIndexTest )
{
    static constexpr size_t kDimensionSize = 8;
    static constexpr size_t kIndex5 = 5;
    static constexpr size_t kIndexMin = std::numeric_limits< size_t >::min( );
    static constexpr size_t kIndexMax = std::numeric_limits< size_t >::max( );

    struct TByteTag;
    TDimension< TByteTag, kDimensionSize, TIterationDirection::Backward > dimension;

    dimension.SetForwardPosition( kIndex5 );
    EXPECT_EQ( dimension.iPosition, ( kDimensionSize - 1 ) - kIndex5 );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndex5 );

    dimension.SetForwardPosition( kDimensionSize );
    EXPECT_EQ( dimension.iPosition, ( kDimensionSize - 1 ) - kDimensionSize );
    EXPECT_EQ( dimension.GetForwardPosition( ), kDimensionSize );

    dimension.SetForwardPosition( kIndexMin );
    EXPECT_EQ( dimension.iPosition, ( kDimensionSize - 1 ) - kIndexMin );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndexMin );

    dimension.SetForwardPosition( kIndexMax );
    EXPECT_EQ( dimension.iPosition, ( kDimensionSize - 1 ) - kIndexMax );
    EXPECT_EQ( dimension.GetForwardPosition( ), kIndexMax );
}

TEST( TensorIterator, InitialStateTest )
{
    struct TWordTag;
    struct TByteTag;
    using TWord = TDimension< TWordTag, 4 >;
    using TByte = TDimension< TByteTag, 8 >;

    TTensorIterator< TWord, TByte > tesorIterator;

    EXPECT_EQ( tesorIterator.DimentsionCount( ), 2 );
    EXPECT_EQ( tesorIterator.TensorSize( ), 4 * 8 );
    EXPECT_EQ( tesorIterator.SubTensorSize< 0 >( ), 4 * 8 );
    EXPECT_EQ( tesorIterator.SubTensorSize< 1 >( ), 8 );
    EXPECT_EQ( tesorIterator.SubTensorSize< 0 >( ), tesorIterator.TensorSize( ) );

    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
}

TEST( TensorIterator, SetGlobalIndexTest )
{
    struct TWordTag;
    struct TByteTag;
    using TWord = TDimension< TWordTag, 4 >;
    using TByte = TDimension< TByteTag, 8 >;

    TTensorIterator< TWord, TByte > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 22 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 6 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 31 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 7 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
}

TEST( TensorIterator, SetGlobalIndexWithReversedIterationElementTest )
{
    struct TWordTag;
    struct TByteTag;
    using TWord = TDimension< TWordTag, 4 >;
    using TReversedByte = TDimension< TByteTag, 8, TIterationDirection::Backward >;

    TTensorIterator< TWord, TReversedByte > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 7 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 22 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );

    tesorIterator.SetGlobalPosition( 31 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedByte >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
}

TEST( TensorIterator, SetGlobalIndexForThreeDimentionTest )
{
    struct TWordSetTag;
    struct TWordTag;
    struct TByteTag;

    using TWordSet = TDimension< TWordSetTag, 2 >;
    using TWord = TDimension< TWordTag, 4 >;
    using TByte = TDimension< TByteTag, 2 >;

    TTensorIterator< TWordSet, TWord, TByte > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 9 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 13 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 15 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );
}

TEST( TensorIterator, SetGlobalIndexForThreeDimentionWithReversedIterationElementTest )
{
    struct TWordSetTag;
    struct TWordTag;
    struct TByteTag;

    using TReversedWordSet = TDimension< TWordSetTag, 2, TIterationDirection::Backward >;
    using TWord = TDimension< TWordTag, 4 >;
    using TByte = TDimension< TByteTag, 2 >;

    TTensorIterator< TReversedWordSet, TWord, TByte > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 9 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 13 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 15 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 2 >( ) );
}