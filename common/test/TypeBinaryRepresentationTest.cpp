#include <gtest/gtest.h>

#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>

#include <utility>
#include <cstdint>
#include <limits>
#include <array>
#include <type_traits>
#include <tuple>

using namespace AbstractPlatform;
namespace
{
static_assert( kBitsPerByte == 8 );
}

template < typename T >
struct TypeBinaryRepresentationTestTest : public testing::Test
{
    using TType = T;
};

using TTypeBinaryRepresentationTestTypes = testing::Types< std::uint8_t,
                                                           std::uint16_t,
                                                           std::uint32_t,
                                                           std::uint64_t,
                                                           std::int8_t,
                                                           std::int16_t,
                                                           std::int32_t,
                                                           std::int64_t,
                                                           std::size_t >;
TYPED_TEST_SUITE( TypeBinaryRepresentationTestTest, TTypeBinaryRepresentationTestTypes );

// template < typename taT >
// static constexpr inline taT
// SetBitTest( const size_t aBit )
// {
//     return taT{ 1 } << aBit;
// }

template < typename taDataType, size_t taIndex >
static constexpr inline bool
ByteSwap_StaticBitOperationsTest( )
{
    // constexpr taDataType kAllSetValue = AllBitsSet< taDataType >( );
    // constexpr taDataType kZeroValue = taDataType{ };
    // constexpr taDataType kExpectedBit = SetBitTest< taDataType >( taIndex );
    // constexpr taDataType kExpectedBitMask = kAllSetValue ^ kExpectedBit;

    // static_assert( CheckBit( kExpectedBit, taIndex ) == true );
    // static_assert( CheckBit( kZeroValue, taIndex ) == false );
    // static_assert( CheckBit( kAllSetValue, taIndex ) == true );

    // static_assert( SetBit( kZeroValue, taIndex ) == kExpectedBit );
    // static_assert( SetBit( kAllSetValue, taIndex ) == kAllSetValue );
    // static_assert( SetBit( kExpectedBitMask, taIndex ) == kAllSetValue );
    // static_assert( SetBit( kExpectedBit, taIndex ) == kExpectedBit );

    // static_assert( ClearBit( kZeroValue, taIndex ) == kZeroValue );
    // static_assert( ClearBit( kAllSetValue, taIndex ) == kExpectedBitMask );
    // static_assert( ClearBit( kExpectedBitMask, taIndex ) == kExpectedBitMask );
    // static_assert( ClearBit( kExpectedBit, taIndex ) == kZeroValue );

    // static_assert( ToggleBit( kZeroValue, taIndex ) == kExpectedBit );
    // static_assert( ToggleBit( kAllSetValue, taIndex ) == kExpectedBitMask );
    // static_assert( ToggleBit( kExpectedBitMask, taIndex ) == kAllSetValue );
    // static_assert( ToggleBit( kExpectedBit, taIndex ) == kZeroValue );

    return true;
}

template < typename taDataType, typename taIndexType, taIndexType... taIndexes >
static inline std::pair< taDataType, taDataType >
ByteSwap_TestPairImpl( std::integer_sequence< taIndexType, taIndexes... > )
{
    std::uint8_t forwardData[ sizeof( taDataType ) ];
    std::uint8_t reverseData[ sizeof( taDataType ) ];

    const auto setElement = []( std::uint8_t* aArray, size_t aIndex )
    {
        aArray[ aIndex ] = static_cast< std::uint8_t >( aIndex );
        return 1;
    };

    int a = ( setElement( forwardData, taIndexes ) + ... );

    return { reinterpret_cast< taDataType& >( forwardData ),
             reinterpret_cast< taDataType& >( reverseData ) };
}

template < typename taDataType >
static inline std::pair< taDataType, taDataType >
ByteSwap_TestPair( )
{
    return ByteSwap_TestPairImpl< taDataType >(
        std::make_index_sequence< sizeof( taDataType ) >{ } );
}

template < typename taDataType, typename taIndexType, taIndexType... taIndexes >
static constexpr inline bool
ByteSwap_StaticOperationsTest( std::integer_sequence< taIndexType, taIndexes... > )
{
    return ( ByteSwap_StaticBitOperationsTest< taDataType, taIndexes >( ), ... ), true;
}

TYPED_TEST( TypeBinaryRepresentationTestTest, ByteSwap )
{
    using TType = typename TestFixture::TType;
    // static_assert( ByteSwap_StaticOperationsTest< TType >(
    //     std::make_index_sequence< BitSize( TType{ } ) >{ } ) );

    // auto checkForBit = []( size_t aIndex )
    // {
    //     const TType kAllSetValue = AllBitsSet< TType >( );
    //     const TType kZeroValue = TType{ };
    //     const TType kExpectedBit = SetBitTest< TType >( aIndex );
    //     const TType kExpectedBitMask = kAllSetValue ^ kExpectedBit;

    //     EXPECT_TRUE( CheckBit( kExpectedBit, aIndex ) );
    //     EXPECT_FALSE( CheckBit( kZeroValue, aIndex ) );
    //     EXPECT_TRUE( CheckBit( kAllSetValue, aIndex ) );

    //     EXPECT_EQ( SetBit( kZeroValue, aIndex ), kExpectedBit );
    //     EXPECT_EQ( SetBit( kAllSetValue, aIndex ), kAllSetValue );
    //     EXPECT_EQ( SetBit( kExpectedBitMask, aIndex ), kAllSetValue );
    //     EXPECT_EQ( SetBit( kExpectedBit, aIndex ), kExpectedBit );

    //     EXPECT_EQ( ClearBit( kZeroValue, aIndex ), kZeroValue );
    //     EXPECT_EQ( ClearBit( kAllSetValue, aIndex ), kExpectedBitMask );
    //     EXPECT_EQ( ClearBit( kExpectedBitMask, aIndex ), kExpectedBitMask );
    //     EXPECT_EQ( ClearBit( kExpectedBit, aIndex ), kZeroValue );

    //     EXPECT_EQ( ToggleBit( kZeroValue, aIndex ), kExpectedBit );
    //     EXPECT_EQ( ToggleBit( kAllSetValue, aIndex ), kExpectedBitMask );
    //     EXPECT_EQ( ToggleBit( kExpectedBitMask, aIndex ), kAllSetValue );
    //     EXPECT_EQ( ToggleBit( kExpectedBit, aIndex ), kZeroValue );
    // };

    // for ( size_t bit = 0; bit < BitSize( TType{ } ); ++bit )
    // {
    //     checkForBit( bit );
    // }
}