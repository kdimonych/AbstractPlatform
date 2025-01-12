#include <gtest/gtest.h>

#include <AbstractPlatform/tensor/StaticTensorIterator.hpp>

#include <cassert>
#include <utility>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <tuple>

using namespace AbstractPlatform::Tensor;
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
struct StaticDimensionTest : public testing::Test
{
    using TDimensionParameters = T;
    static constexpr auto kExpectedSize = TDimensionParameters::kSize;
    static constexpr auto kExpectedIterationDirection = TDimensionParameters::kIterationDirection;

    using TStaticDimension
        = TStaticDimension< TSomeTag, kExpectedSize, kExpectedIterationDirection >;

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
TYPED_TEST_SUITE( StaticDimensionTest, TTensorIteratorDimentionTypes );

TYPED_TEST( StaticDimensionTest, InitialState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;

    TStaticDimension dimension;

    static_assert( std::is_same< decltype( TStaticDimension::Size( ) ),
                                 typename TStaticDimension::TSize >::value );
    static_assert( std::is_same< decltype( TStaticDimension{ }.GetPosition( ) ),
                                 typename TStaticDimension::TPosition >::value );
    static_assert( TStaticDimension::kSize == TestFixture::kExpectedSize );
    static_assert( TStaticDimension::Size( ) == TestFixture::kExpectedSize );
    static_assert( TStaticDimension::kIterationDirection
                   == TestFixture::kExpectedIterationDirection );

    constexpr size_t kExpectedForwardPosition = 0;
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TStaticDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, RefInitialState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;

    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );
    using TTStaticDimensionRef = decltype( dimensionRef );

    static_assert(
        std::is_same< typename TTStaticDimensionRef::TStaticDimension, TStaticDimension >::value );
    static_assert( std::is_same< typename TTStaticDimensionRef::TTag,
                                 typename TStaticDimension::TTag >::value );
    static_assert( std::is_same< typename TTStaticDimensionRef::TSize,
                                 typename TStaticDimension::TSize >::value );
    static_assert( std::is_same< typename TTStaticDimensionRef::TPosition,
                                 typename TStaticDimension::TPosition >::value );
    static_assert( std::is_same< decltype( dimensionRef.iDimension ), TStaticDimension& >::value );
    static_assert(
        std::is_same< decltype( dimensionRef.Size( ) ), decltype( dimension.Size( ) ) >::value );
    static_assert( std::is_same< decltype( dimensionRef.GetPosition( ) ),
                                 decltype( dimension.GetPosition( ) ) >::value );
    // TODO: check SetPosition() signature
    static_assert( std::is_same< decltype( dimensionRef.IsBegin( ) ),
                                 decltype( dimension.IsBegin( ) ) >::value );
    static_assert( std::is_same< decltype( dimensionRef.IsLast( ) ),
                                 decltype( dimension.IsLast( ) ) >::value );
    static_assert(
        std::is_same< decltype( dimensionRef.IsEnd( ) ), decltype( dimension.IsEnd( ) ) >::value );

    EXPECT_EQ( dimensionRef.GetPosition( ), dimension.GetPosition( ) );
    EXPECT_EQ( dimensionRef.IsBegin( ), dimension.IsBegin( ) );
    EXPECT_EQ( dimensionRef.IsLast( ), dimension.IsLast( ) );
    EXPECT_EQ( dimensionRef.IsEnd( ), dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, BeginElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;

    constexpr size_t kExpectedForwardPosition = 0;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_TRUE( dimension.IsBegin( ) );
    EXPECT_EQ( dimension.IsLast( ), TStaticDimension::kSize == 1 );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, RefBeginElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;

    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );

    constexpr size_t kExpectedForwardPosition = 0;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimensionRef.GetPosition( ), dimension.GetPosition( ) );
    EXPECT_EQ( dimensionRef.IsBegin( ), dimension.IsBegin( ) );
    EXPECT_EQ( dimensionRef.IsLast( ), dimension.IsLast( ) );
    EXPECT_EQ( dimensionRef.IsEnd( ), dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, LastElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize - 1;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_EQ( dimension.IsBegin( ), TStaticDimension::kSize == 1 );
    EXPECT_TRUE( dimension.IsLast( ) );
    EXPECT_FALSE( dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, RefLastElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize - 1;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimensionRef.GetPosition( ), dimension.GetPosition( ) );
    EXPECT_EQ( dimensionRef.IsBegin( ), dimension.IsBegin( ) );
    EXPECT_EQ( dimensionRef.IsLast( ), dimension.IsLast( ) );
    EXPECT_EQ( dimensionRef.IsEnd( ), dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, EndElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimension.iDirectionalPosition,
               TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
    EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    EXPECT_FALSE( dimension.IsBegin( ) );
    EXPECT_FALSE( dimension.IsLast( ) );
    EXPECT_TRUE( dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, RefEndElementPositionState )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );

    constexpr size_t kExpectedForwardPosition = TestFixture::kExpectedSize;
    dimension.SetPosition( kExpectedForwardPosition );
    EXPECT_EQ( dimensionRef.GetPosition( ), dimension.GetPosition( ) );
    EXPECT_EQ( dimensionRef.IsBegin( ), dimension.IsBegin( ) );
    EXPECT_EQ( dimensionRef.IsLast( ), dimension.IsLast( ) );
    EXPECT_EQ( dimensionRef.IsEnd( ), dimension.IsEnd( ) );
}

TYPED_TEST( StaticDimensionTest, SetForwardIndex )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;

    for ( size_t i = 0; i <= TestFixture::kExpectedSize; ++i )
    {
        const size_t kExpectedForwardPosition = i;
        dimension.SetPosition( kExpectedForwardPosition );
        EXPECT_EQ( dimension.iDirectionalPosition,
                   TestFixture::expectedPlainPosition( kExpectedForwardPosition ) );
        EXPECT_EQ( dimension.GetPosition( ), kExpectedForwardPosition );
    }
}

TYPED_TEST( StaticDimensionTest, RefSetForwardIndex )
{
    using TStaticDimension = typename TestFixture::TStaticDimension;
    TStaticDimension dimension;
    auto dimensionRef = MakeStaticDimensionRef( dimension );

    for ( size_t i = 0; i <= TestFixture::kExpectedSize; ++i )
    {
        const size_t kExpectedForwardPosition = i;
        dimensionRef.SetPosition( kExpectedForwardPosition );
        EXPECT_EQ( dimensionRef.GetPosition( ), dimension.GetPosition( ) );
        EXPECT_EQ( dimensionRef.IsBegin( ), dimension.IsBegin( ) );
        EXPECT_EQ( dimensionRef.IsLast( ), dimension.IsLast( ) );
        EXPECT_EQ( dimensionRef.IsEnd( ), dimension.IsEnd( ) );
    }
}

template < typename T >
struct StaticTwoDimensionTensorIteratorTest : public testing::Test
{
    using TByte = typename T::first_type;
    using TWord = typename T::second_type;
    using TStaticTensorIterator = TStaticTensorIterator< TByte, TWord >;

    static constexpr size_t kByteDimensionSize = TByte::kSize;
    static constexpr size_t kWordDimensionSize = TWord::kSize;

    static constexpr size_t kExpectedTensorSize = kByteDimensionSize * kWordDimensionSize;
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

using TStaticTwoDimensionTensorIteratorTestTypes
    = testing::Types< std::pair< TStaticDimension< TByteTag, 1 >, TStaticDimension< TWordTag, 1 > >,
                      std::pair< TStaticDimension< TByteTag, 1, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 1 > >,
                      std::pair< TStaticDimension< TByteTag, 1 >,
                                 TStaticDimension< TWordTag, 1, TIterationDirection::Backward > >,
                      std::pair< TStaticDimension< TByteTag, 1, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 1, TIterationDirection::Backward > >,
                      std::pair< TStaticDimension< TByteTag, 8 >, TStaticDimension< TWordTag, 4 > >,
                      std::pair< TStaticDimension< TByteTag, 8, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 4 > >,
                      std::pair< TStaticDimension< TByteTag, 8 >,
                                 TStaticDimension< TWordTag, 4, TIterationDirection::Backward > >,
                      std::pair< TStaticDimension< TByteTag, 8, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 4, TIterationDirection::Backward > >,
                      std::pair< TStaticDimension< TByteTag, 7 >, TStaticDimension< TWordTag, 5 > >,
                      std::pair< TStaticDimension< TByteTag, 7, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 5 > >,
                      std::pair< TStaticDimension< TByteTag, 7 >,
                                 TStaticDimension< TWordTag, 5, TIterationDirection::Backward > >,
                      std::pair< TStaticDimension< TByteTag, 7, TIterationDirection::Backward >,
                                 TStaticDimension< TWordTag, 5, TIterationDirection::Backward > > >;

TYPED_TEST_SUITE( StaticTwoDimensionTensorIteratorTest,
                  TStaticTwoDimensionTensorIteratorTestTypes );

TYPED_TEST( StaticTwoDimensionTensorIteratorTest, InitialState )
{
    using TByte = typename TestFixture::TByte;
    using TWord = typename TestFixture::TWord;
    using TStaticTensorIterator = typename TestFixture::TStaticTensorIterator;

    static_assert( std::is_same< decltype( std::declval< TStaticTensorIterator >( ).Size( ) ),
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

TYPED_TEST( StaticTwoDimensionTensorIteratorTest, SetGlobalIndex )
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

    for ( size_t i = 0; i < TestFixture::kExpectedTensorSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TYPED_TEST( StaticTwoDimensionTensorIteratorTest, GetGlobalIndex )
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

    for ( size_t i = 0; i < TestFixture::kExpectedTensorSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TEST( StaticTensorIteratorTest, SetGlobalIndexForThreeDimention )
{
    struct TByteTag;
    struct TWordTag;
    struct TWordSetTag;

    static constexpr size_t kByteDimensionSize = 2;
    static constexpr size_t kWordDimensionSize = 4;
    static constexpr size_t kWordSetDimensionSize = 2;
    static constexpr size_t kExpectedTensorSize
        = kByteDimensionSize * kWordDimensionSize * kWordSetDimensionSize;

    using TByte = TStaticDimension< TByteTag, kByteDimensionSize >;
    using TWord = TStaticDimension< TWordTag, kWordDimensionSize >;
    using TWordSet = TStaticDimension< TWordSetTag, kWordSetDimensionSize >;

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

    for ( size_t i = 0; i <= kExpectedTensorSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}

TEST( StaticTensorIteratorTest, SetGlobalIndexForThreeDimentionWithReversedIterationElement )
{
    struct TWordSetTag;
    struct TWordTag;
    struct TByteTag;

    using TReversedWordSet = TStaticDimension< TWordSetTag, 2, TIterationDirection::Backward >;
    using TWord = TStaticDimension< TWordTag, 4 >;
    using TByte = TStaticDimension< TByteTag, 2 >;

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

TEST( StaticTensorIteratorTest, TStaticTensorIteratorAsStaticDimension )
{
    struct TBaseDimensionTag;
    struct TSecondaryDimensionTag;

    static constexpr size_t kBaseDimensionSize = 5;
    static constexpr size_t kExpectedTensorSize = kBaseDimensionSize;

    using TBaseDimension = TStaticDimension< TBaseDimensionTag, kBaseDimensionSize >;
    using TBaseIterator = TStaticTensorIterator< TBaseDimension >;

    using TSecondaryIterator = TStaticTensorIterator< TBaseIterator >;

    static_assert( std::is_same<
                   decltype( std::declval< TSecondaryIterator >( ).Dimension< TBaseIterator >( ) ),
                   TBaseIterator& >::value );

    static_assert( TSecondaryIterator::Size( ) == TBaseIterator::Size( ) );
    static_assert( TSecondaryIterator::Size( ) == TBaseDimension::Size( ) );

    TSecondaryIterator tesorIterator;
    auto setGlobalPositionTest = [ & ]( size_t globalPositionIndex )
    {
        tesorIterator.SetPosition( globalPositionIndex );

        EXPECT_EQ( tesorIterator.template Dimension< 0 >( ).GetPosition( ),
                   tesorIterator.GetPosition( ) );
        EXPECT_EQ(
            tesorIterator.template Dimension< 0 >( ).template Dimension< 0 >( ).GetPosition( ),
            tesorIterator.GetPosition( ) );
    };

    for ( size_t i = 0; i < kExpectedTensorSize; ++i )
    {
        setGlobalPositionTest( i );
    }
}