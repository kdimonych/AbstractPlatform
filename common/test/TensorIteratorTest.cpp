#include <gtest/gtest.h>

#include <AbstractPlatform/common/TensorIterator.hpp>

#include <utility>
#include <cstdint>
#include <limits>

using namespace AbstractPlatform;
namespace
{
}

template < size_t taSize, TIterationDirection taIterationDirection >
struct TDimensionParameters
{
    static constexpr auto kSize = taSize;
    static constexpr auto kIterationDirection = taIterationDirection;
};

struct TSomeTag;

template < typename T >
struct TensorIteratorDimentionTest : public testing::Test
{
    using TDimensionParameters = T;
    static constexpr auto kExpectedSize = TDimensionParameters::kSize;
    static constexpr auto kExpectedIterationDirection = TDimensionParameters::kIterationDirection;

    using TDimension = TDimension< TSomeTag, kExpectedSize, kExpectedIterationDirection >;

    static constexpr size_t
    expectedPlainPosition( size_t forwardPosition )
    {
        return kExpectedIterationDirection == TIterationDirection::Forward
                   ? forwardPosition
                   : kExpectedSize - 1 - forwardPosition;
    }
};

using TTensorIteratorDimentionTypes
    = testing::Types< TDimensionParameters< 1, TIterationDirection::Forward >,
                      TDimensionParameters< 1, TIterationDirection::Backward >,
                      TDimensionParameters< 8, TIterationDirection::Forward >,
                      TDimensionParameters< 8, TIterationDirection::Backward > >;
TYPED_TEST_SUITE( TensorIteratorDimentionTest, TTensorIteratorDimentionTypes );

TYPED_TEST( TensorIteratorDimentionTest, InitialState )
{
    using TDimension = typename TestFixture::TDimension;

    TDimension dimension;

    static_assert( TDimension::kSize == TestFixture::kExpectedSize );
    static_assert( TDimension::kIterationDirection == TestFixture::kExpectedIterationDirection );

    constexpr size_t kExpectedForwardPosition = 0;
    EXPECT_EQ( dimension.iPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetForwardPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, BeginElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = 0;
    dimension.SetForwardPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetForwardPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, LastElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize - 1;
    dimension.SetForwardPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetForwardPosition( ), kExpectedForwardPosition );
    EXPECT_EQ( dimension.IsBegin( ), TDimension::kSize == 1 );
    EXPECT_TRUE( dimension.IsLast( ) );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, EndElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize;
    dimension.SetForwardPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetForwardPosition( ), kExpectedForwardPosition );
    EXPECT_FALSE( dimension.IsBegin( ) );
    EXPECT_FALSE( dimension.IsLast( ) );
    EXPECT_TRUE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, SetForwardIndex )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    for ( size_t i = 0; i < TestFixture::kExpectedSize; ++i )
    {
        const size_t kExpectedForwardPosition = i;
        dimension.SetForwardPosition( kExpectedForwardPosition );
        EXPECT_EQ( dimension.iPosition,
                   TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
        EXPECT_EQ( dimension.GetForwardPosition( ), kExpectedForwardPosition );
    }
}

template < typename... taDimension >
struct TTensorIteratorParameters
{
    using TTensorIterator = TTensorIterator< taDimension... >;
    using TDimensions = std::tuple< taDimension... >;

    static constexpr size_t
    TensorSize( )
    {
        return ( ... * taDimension::kSize );
    }
};

struct TSomeTag;

template < typename T >
struct TensorIteratorTest : public testing::Test
{
    using TTensorIteratorParameters = T;
    using TTensorIterator = typename TTensorIteratorParameters::TTensorIterator;
    using TDimensions = typename TTensorIteratorParameters::TDimensions;

    static constexpr size_t kExpectedDimentsionCount = std::tuple_size< TDimensions >::value;
    static constexpr size_t kExpectedTensorSize = TTensorIteratorParameters::TensorSize( );
};

struct TByteTag;
struct TWordTag;

using TTensorIteratorTestTypes = testing::Types<
    TTensorIteratorParameters< TDimension< TByteTag, 8 >, TDimension< TWordTag, 4 > > >;

TYPED_TEST_SUITE( TensorIteratorTest, TTensorIteratorTestTypes );

TYPED_TEST( TensorIteratorTest, InitialState )
{
    using TTensorIterator = typename TestFixture::TTensorIterator;
    using TDimensions = typename TestFixture::TDimensions;

    static_assert( TTensorIterator::DimentsionCount( ) == TestFixture::kExpectedDimentsionCount );
    static_assert( TTensorIterator::TensorSize( ) == TestFixture::kExpectedTensorSize );
    static_assert(
        TTensorIterator::template SubTensorSize< TestFixture::kExpectedDimentsionCount - 1 >( )
        == TestFixture::kExpectedTensorSize );

    TTensorIterator tesorIterator;
}

TEST( TensorIterator, InitialState )
{
    struct TByteTag;
    struct TWordTag;

    static constexpr size_t kByteDimensionSize = 8;
    static constexpr size_t kWordDimensionSize = 4;

    using TByte = TDimension< TByteTag, kByteDimensionSize >;
    using TWord = TDimension< TWordTag, kWordDimensionSize >;

    TTensorIterator< TByte, TWord > tesorIterator;

    EXPECT_EQ( tesorIterator.DimentsionCount( ), 2 );
    EXPECT_EQ( tesorIterator.TensorSize( ), TByte::kSize * TWord::kSize );
    EXPECT_EQ( tesorIterator.SubTensorSize< 0 >( ), kByteDimensionSize );
    EXPECT_EQ( tesorIterator.SubTensorSize< 1 >( ), kByteDimensionSize * kWordDimensionSize );

    EXPECT_EQ( tesorIterator.SubTensorSize< 1 >( ), tesorIterator.TensorSize( ) );

    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
}

TEST( TensorIterator, InitialStateWithReversedDimensionTest )
{
    struct TByteTag;
    struct TWordTag;

    static constexpr size_t kByteDimensionSize = 8;
    static constexpr size_t kWordDimensionSize = 4;

    using TByte = TDimension< TByteTag, kByteDimensionSize, TIterationDirection::Backward >;
    using TWord = TDimension< TWordTag, kWordDimensionSize, TIterationDirection::Backward >;

    TTensorIterator< TByte, TWord > tesorIterator;

    EXPECT_EQ( tesorIterator.DimentsionCount( ), 2 );
    EXPECT_EQ( tesorIterator.TensorSize( ), TByte::kSize * TWord::kSize );
    EXPECT_EQ( tesorIterator.SubTensorSize< 0 >( ), kByteDimensionSize );
    EXPECT_EQ( tesorIterator.SubTensorSize< 1 >( ), kByteDimensionSize * kWordDimensionSize );

    EXPECT_EQ( tesorIterator.SubTensorSize< 1 >( ), tesorIterator.TensorSize( ) );

    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), kByteDimensionSize - 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), kWordDimensionSize - 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
}

TEST( TensorIterator, SetGlobalIndexTest )
{
    struct TByteTag;
    struct TWordTag;

    static constexpr size_t kByteDimensionSize = 8;
    static constexpr size_t kWordDimensionSize = 4;
    static constexpr size_t kExpectedTensorSizeSize = kByteDimensionSize * kWordDimensionSize;

    using TByte = TDimension< TByteTag, kByteDimensionSize >;
    using TWord = TDimension< TWordTag, kWordDimensionSize >;

    TTensorIterator< TByte, TWord > tesorIterator;
    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        auto expectedPosition = []( size_t forwardPosition, TIterationDirection iterationDirection
                                                            = TIterationDirection::Forward )
        {
            return iterationDirection == TIterationDirection::Forward
                       ? forwardPosition
                       : kByteDimensionSize - 1 - forwardPosition;
        };

        tesorIterator.SetGlobalPosition( globalPositionIndex );
        EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ),
                   expectedPosition( globalPositionIndex / 1 % kByteDimensionSize,
                                     TByte::kIterationDirection ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ),
                   expectedPosition( globalPositionIndex / kByteDimensionSize
                                         % ( kByteDimensionSize * kWordDimensionSize ),
                                     TWord::kIterationDirection ) )
            << " for the globalPositionIndex = " << globalPositionIndex;

        EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
                   tesorIterator.DimensionPosition< 0 >( ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
                   tesorIterator.DimensionPosition< 1 >( ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
    };

    for ( size_t i = 0; i < kExpectedTensorSizeSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TEST( TensorIterator, SetGlobalIndexWithReversedIterationElementTest )
{
    struct TByteTag;
    struct TWordTag;

    static constexpr size_t kByteDimensionSize = 8;
    static constexpr size_t kWordDimensionSize = 4;
    static constexpr size_t kExpectedTensorSizeSize = kByteDimensionSize * kWordDimensionSize;

    using TByte = TDimension< TByteTag, kByteDimensionSize, TIterationDirection::Backward >;
    using TWord = TDimension< TWordTag, kWordDimensionSize >;

    TTensorIterator< TByte, TWord > tesorIterator;

    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        auto expectedPosition = []( size_t forwardPosition, TIterationDirection iterationDirection
                                                            = TIterationDirection::Forward )
        {
            return iterationDirection == TIterationDirection::Forward
                       ? forwardPosition
                       : kByteDimensionSize - 1 - forwardPosition;
        };

        tesorIterator.SetGlobalPosition( globalPositionIndex );
        EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ),
                   expectedPosition( globalPositionIndex / 1 % kByteDimensionSize,
                                     TByte::kIterationDirection ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ),
                   expectedPosition( globalPositionIndex / kByteDimensionSize
                                         % ( kByteDimensionSize * kWordDimensionSize ),
                                     TWord::kIterationDirection ) )
            << " for the globalPositionIndex = " << globalPositionIndex;

        EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
                   tesorIterator.DimensionPosition< 0 >( ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
                   tesorIterator.DimensionPosition< 1 >( ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
    };

    for ( size_t i = 0; i < kExpectedTensorSizeSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TEST( TensorIterator, SetGlobalIndexForThreeDimentionTest )
{
    struct TByteTag;
    struct TWordTag;
    struct TWordSetTag;

    static constexpr size_t kByteDimensionSize = 2;
    static constexpr size_t kWordDimensionSize = 4;
    static constexpr size_t kWordSetDimensionSize = 2;
    static constexpr size_t kExpectedTensorSizeSize
        = kByteDimensionSize * kWordDimensionSize * kWordSetDimensionSize;

    using TByte = TDimension< TByteTag, kByteDimensionSize >;
    using TWord = TDimension< TWordTag, kWordDimensionSize >;
    using TWordSet = TDimension< TWordSetTag, kWordSetDimensionSize >;

    TTensorIterator< TByte, TWord, TWordSet > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 5 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 9 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 13 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 15 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWordSet >( ),
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

    TTensorIterator< TByte, TWord, TReversedWordSet > tesorIterator;

    tesorIterator.SetGlobalPosition( 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
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
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 13 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 2 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );

    tesorIterator.SetGlobalPosition( 15 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 0 >( ), 1 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 1 >( ), 3 );
    EXPECT_EQ( tesorIterator.DimensionPosition< 2 >( ), 0 );
    EXPECT_EQ( tesorIterator.DimensionPosition< TByte >( ),
               tesorIterator.DimensionPosition< 0 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TWord >( ),
               tesorIterator.DimensionPosition< 1 >( ) );
    EXPECT_EQ( tesorIterator.DimensionPosition< TReversedWordSet >( ),
               tesorIterator.DimensionPosition< 2 >( ) );
}