#include <gtest/gtest.h>

#include <AbstractPlatform/tensor/StaticTensorIterator.hpp>

#include <utility>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <tuple>

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

    static_assert(
        std::is_same< decltype( TDimension::Size( ) ), typename TDimension::TSize >::value );
    static_assert( std::is_same< decltype( TDimension{ }.GetPosition( ) ),
                                 typename TDimension::TPosition >::value );
    static_assert( TDimension::kSize == TestFixture::kExpectedSize );
    static_assert( TDimension::Size( ) == TestFixture::kExpectedSize );
    static_assert( TDimension::kIterationDirection == TestFixture::kExpectedIterationDirection );

    constexpr size_t kExpectedForwardPosition = 0;
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, BeginElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = 0;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, LastElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize - 1;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_EQ( dimension.IsBegin( ), TDimension::kSize == 1 );
    EXPECT_TRUE( dimension.IsLast( ) );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( TensorIteratorDimentionTest, EndElementPositionState )
{
    using TDimension = typename TestFixture::TDimension;
    TDimension dimension;

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
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
        dimension.SetPosition( kExpectedForwardPosition );
        EXPECT_EQ( dimension.iDirectionalPosition,
                   TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
        EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    }
}

template < typename T >
struct TwoDimensionTensorIteratorTest : public testing::Test
{
    using TByte = typename T::first_type;
    using TWord = typename T::second_type;
    using TStaticTensorIterator = TStaticTensorIterator< TByte, TWord >;

    static constexpr size_t kByteDimensionSize = TByte::kSize;
    static constexpr size_t kWordDimensionSize = TWord::kSize;

    static constexpr size_t kExpectedTensorSizeSize = kByteDimensionSize * kWordDimensionSize;
    static constexpr size_t kExpectedDimensionCount = 2;

    template < typename taDimension >
    static constexpr auto
    ExpectedPlainPosition( size_t forwardPosition )
    {
        return taDimension::kIterationDirection == TIterationDirection::Forward
                   ? forwardPosition
                   : taDimension::kSize - 1 - forwardPosition;
    };
};

struct TByteTag;
struct TWordTag;

using TTwoDimensionTensorIteratorTestTypes
    = testing::Types< std::pair< TDimension< TByteTag, 1 >, TDimension< TWordTag, 1 > >,
                      std::pair< TDimension< TByteTag, 1, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 1 > >,
                      std::pair< TDimension< TByteTag, 1 >,
                                 TDimension< TWordTag, 1, TIterationDirection::Backward > >,
                      std::pair< TDimension< TByteTag, 1, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 1, TIterationDirection::Backward > >,
                      std::pair< TDimension< TByteTag, 8 >, TDimension< TWordTag, 4 > >,
                      std::pair< TDimension< TByteTag, 8, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 4 > >,
                      std::pair< TDimension< TByteTag, 8 >,
                                 TDimension< TWordTag, 4, TIterationDirection::Backward > >,
                      std::pair< TDimension< TByteTag, 8, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 4, TIterationDirection::Backward > >,
                      std::pair< TDimension< TByteTag, 7 >, TDimension< TWordTag, 5 > >,
                      std::pair< TDimension< TByteTag, 7, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 5 > >,
                      std::pair< TDimension< TByteTag, 7 >,
                                 TDimension< TWordTag, 5, TIterationDirection::Backward > >,
                      std::pair< TDimension< TByteTag, 7, TIterationDirection::Backward >,
                                 TDimension< TWordTag, 5, TIterationDirection::Backward > > >;

TYPED_TEST_SUITE( TwoDimensionTensorIteratorTest, TTwoDimensionTensorIteratorTestTypes );

TYPED_TEST( TwoDimensionTensorIteratorTest, InitialState )
{
    using TByte = typename TestFixture::TByte;
    using TWord = typename TestFixture::TWord;
    using TStaticTensorIterator = typename TestFixture::TStaticTensorIterator;

    static_assert( std::is_same< decltype( TStaticTensorIterator{ }.Size( ) ),
                                 typename TStaticTensorIterator::TSize >::value );
    static_assert( TStaticTensorIterator::DimentsionCount( )
                   == TestFixture::kExpectedDimensionCount );
    static_assert( TStaticTensorIterator::Size( ) == TByte::kSize * TWord::kSize );
    static_assert( TStaticTensorIterator::template SubTensorSize< 0 >( ) == TByte::kSize );
    static_assert( TStaticTensorIterator::template SubTensorSize< 1 >( )
                   == TByte::kSize * TWord::kSize );
    static_assert( TStaticTensorIterator::Size( )
                   == TStaticTensorIterator::template SubTensorSize< 1 >( ) );

    TStaticTensorIterator tesorIterator;
    EXPECT_EQ( tesorIterator.GetPosition( ), 0 );

    EXPECT_EQ( tesorIterator.template Dimension< 0 >( ).iDirectionalPosition,
               TestFixture::template ExpectedPlainPosition< TByte >( 0 ) );
    EXPECT_EQ( tesorIterator.template Dimension< 1 >( ).iDirectionalPosition,
               TestFixture::template ExpectedPlainPosition< TWord >( 0 ) );
    EXPECT_EQ( tesorIterator.template Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.template Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.template Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.template Dimension< 1 >( ).iDirectionalPosition );
}

TYPED_TEST( TwoDimensionTensorIteratorTest, SetGlobalIndex )
{
    using TByte = typename TestFixture::TByte;
    using TWord = typename TestFixture::TWord;
    using TStaticTensorIterator = typename TestFixture::TStaticTensorIterator;

    TStaticTensorIterator tesorIterator;
    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        tesorIterator.SetPosition( globalPositionIndex );

        EXPECT_EQ( tesorIterator.template Dimension< 0 >( ).iDirectionalPosition,
                   TestFixture::template ExpectedPlainPosition< TByte >( ( globalPositionIndex / 1 )
                                                                         % TByte::kSize ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.template Dimension< 1 >( ).iDirectionalPosition,
                   TestFixture::template ExpectedPlainPosition< TWord >(
                       ( globalPositionIndex / TByte::kSize ) % TWord::kSize ) )
            << " for the globalPositionIndex = " << globalPositionIndex;

        EXPECT_EQ( tesorIterator.template Dimension< TByte >( ).iDirectionalPosition,
                   tesorIterator.template Dimension< 0 >( ).iDirectionalPosition )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.template Dimension< TWord >( ).iDirectionalPosition,
                   tesorIterator.template Dimension< 1 >( ).iDirectionalPosition )
            << " for the globalPositionIndex = " << globalPositionIndex;
    };

    for ( size_t i = 0; i < TestFixture::kExpectedTensorSizeSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TYPED_TEST( TwoDimensionTensorIteratorTest, GetGlobalIndex )
{
    using TByte = typename TestFixture::TByte;
    using TWord = typename TestFixture::TWord;
    using TStaticTensorIterator = typename TestFixture::TStaticTensorIterator;

    TStaticTensorIterator tesorIterator;
    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        tesorIterator.SetPosition( globalPositionIndex );
        EXPECT_EQ( tesorIterator.GetPosition( ), globalPositionIndex );
    };

    for ( size_t i = 0; i < TestFixture::kExpectedTensorSizeSize; ++i )
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

    TStaticTensorIterator< TByte, TWord, TWordSet > tesorIterator;

    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        auto expectedPosition = []( size_t forwardPosition, const auto dimension )
        {
            return dimension.kIterationDirection == TIterationDirection::Forward
                       ? forwardPosition
                       : dimension.kSize - 1 - forwardPosition;
        };

        tesorIterator.SetPosition( globalPositionIndex );
        EXPECT_EQ( tesorIterator.GetPosition( ), globalPositionIndex );
        EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition,
                   expectedPosition( ( globalPositionIndex / 1 ) % kByteDimensionSize, TByte{ } ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ(
            tesorIterator.Dimension< 1 >( ).iDirectionalPosition,
            expectedPosition( ( globalPositionIndex / kByteDimensionSize ) % kWordDimensionSize,
                              TWord{ } ) )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ(
            tesorIterator.Dimension< 2 >( ).iDirectionalPosition,
            expectedPosition( ( globalPositionIndex / ( kByteDimensionSize * kWordDimensionSize ) )
                                  % ( kWordSetDimensionSize + 1 ),
                              TWordSet{ } ) )
            << " for the globalPositionIndex = " << globalPositionIndex;

        EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
                   tesorIterator.Dimension< 0 >( ).iDirectionalPosition )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
                   tesorIterator.Dimension< 1 >( ).iDirectionalPosition )
            << " for the globalPositionIndex = " << globalPositionIndex;
        EXPECT_EQ( tesorIterator.Dimension< TWordSet >( ).iDirectionalPosition,
                   tesorIterator.Dimension< 2 >( ).iDirectionalPosition )
            << " for the globalPositionIndex = " << globalPositionIndex;
    };

    for ( size_t i = 0; i <= kExpectedTensorSizeSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TEST( TensorIterator, SetGlobalIndexForThreeDimentionWithReversedIterationElementTest )
{
    struct TWordSetTag;
    struct TWordTag;
    struct TByteTag;

    using TReversedWordSet = TDimension< TWordSetTag, 2, TIterationDirection::Backward >;
    using TWord = TDimension< TWordTag, 4 >;
    using TByte = TDimension< TByteTag, 2 >;

    TStaticTensorIterator< TByte, TWord, TReversedWordSet > tesorIterator;

    tesorIterator.SetPosition( 0 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 0 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 2 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 2 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 5 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 5 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 2 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 9 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 9 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 13 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 2 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 15 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 15 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 1 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 3 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );

    tesorIterator.SetPosition( 16 );
    EXPECT_EQ( tesorIterator.GetPosition( ), 16 );
    EXPECT_EQ( tesorIterator.Dimension< 0 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 1 >( ).iDirectionalPosition, 0 );
    EXPECT_EQ( tesorIterator.Dimension< 2 >( ).iDirectionalPosition, size_t{ } - 1 );
    EXPECT_EQ( tesorIterator.Dimension< TByte >( ).iDirectionalPosition,
               tesorIterator.Dimension< 0 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TWord >( ).iDirectionalPosition,
               tesorIterator.Dimension< 1 >( ).iDirectionalPosition );
    EXPECT_EQ( tesorIterator.Dimension< TReversedWordSet >( ).iDirectionalPosition,
               tesorIterator.Dimension< 2 >( ).iDirectionalPosition );
}