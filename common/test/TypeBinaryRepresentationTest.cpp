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

template < typename taDataType, typename taIndexType, taIndexType... taIndexes >
static inline std::pair< taDataType, taDataType >
ByteSwapTestPairImpl( std::integer_sequence< taIndexType, taIndexes... > )
{
    static constexpr auto kTypeSize = sizeof( taDataType );
    using TArray = std::uint8_t[ kTypeSize ];
    union TUnion
    {
        TArray iArray;
        taDataType iValue;

        constexpr
        operator taDataType( ) const
        {
            return iValue;
        }
    };

    constexpr auto kForwardValue
        = TUnion{ .iArray = { static_cast< std::uint8_t >( taIndexes )... } };
    constexpr auto kReversedValue
        = TUnion{ .iArray = { static_cast< std::uint8_t >( ( kTypeSize - 1 ) - taIndexes )... } };

    return { kForwardValue, kReversedValue };
}

template < typename taDataType >
static inline std::pair< taDataType, taDataType >
ByteSwapTestPair( )
{
    return ByteSwapTestPairImpl< taDataType >(
        std::make_index_sequence< sizeof( taDataType ) >{ } );
}
}  // namespace

template < typename T >
struct TypeBinaryRepresentationTest : public testing::Test
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
TYPED_TEST_SUITE( TypeBinaryRepresentationTest, TTypeBinaryRepresentationTestTypes );

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
static constexpr inline bool
ByteSwap_StaticOperationsTest( std::integer_sequence< taIndexType, taIndexes... > )
{
    return ( ByteSwap_StaticBitOperationsTest< taDataType, taIndexes >( ), ... ), true;
}

TYPED_TEST( TypeBinaryRepresentationTest, ByteSwap )
{
    using TType = typename TestFixture::TType;

    const auto [ kForwardValue, kReversedValue ] = ByteSwapTestPair< TType >( );
    EXPECT_EQ( ByteSwap( kForwardValue ), kReversedValue );
    EXPECT_EQ( ByteSwap( kReversedValue ), kForwardValue );
}

TYPED_TEST( TypeBinaryRepresentationTest, EndiannessConverter )
{
    using TType = typename TestFixture::TType;

    const auto [ kForwardValue, kReversedValue ] = ByteSwapTestPair< TType >( );

    {
        SCOPED_TRACE( "The converting between the same endianness must have no effect" );
        EXPECT_EQ( EndiannessConverter< Endianness::Native >::Convert( kForwardValue ),
                   kForwardValue );
        EXPECT_EQ( EndiannessConverter< Endianness::Native >::Convert( kReversedValue ),
                   kReversedValue );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Native, Endianness::Native >::Convert(
                       kForwardValue ) ),
                   kForwardValue );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Native, Endianness::Native >::Convert(
                       kReversedValue ) ),
                   kReversedValue );

        EXPECT_EQ(
            ( EndiannessConverter< Endianness::Big, Endianness::Big >::Convert( kForwardValue ) ),
            kForwardValue );
        EXPECT_EQ(
            ( EndiannessConverter< Endianness::Big, Endianness::Big >::Convert( kReversedValue ) ),
            kReversedValue );
    }

    {
        SCOPED_TRACE( "The converting between the different endianness must reverse byte order" );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Big, Endianness::Little >::Convert(
                       kForwardValue ) ),
                   kReversedValue );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Big, Endianness::Little >::Convert(
                       kReversedValue ) ),
                   kForwardValue );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Little, Endianness::Big >::Convert(
                       kForwardValue ) ),
                   kReversedValue );
        EXPECT_EQ( ( EndiannessConverter< Endianness::Little, Endianness::Big >::Convert(
                       kReversedValue ) ),
                   kForwardValue );
    }

    // TODO: Make this test depend on a build target platform
    {
        SCOPED_TRACE(
            "The converting from native endianness another must depend on a build target "
            "platform" );
        // For the Little-endian native endinness
        EXPECT_EQ( EndiannessConverter< Endianness::Little >::Convert( kForwardValue ),
                   kForwardValue );
        EXPECT_EQ( EndiannessConverter< Endianness::Big >::Convert( kForwardValue ),
                   kReversedValue );
    }
}

TYPED_TEST( TypeBinaryRepresentationTest, BitSize )
{
    using TType = typename TestFixture::TType;
    constexpr size_t kTypeBitSize = sizeof( TType ) * kBitsPerByte;

    static_assert( BitSize( TType{ } ) == kTypeBitSize );

    EXPECT_EQ( BitSize( TType{ } ), kTypeBitSize );
}

TEST( TypeBinaryRepresentationTestStatic, BufferSize )
{
    static_assert( BufferSize( 0 ) == 0 );
    static_assert( BufferSize( 1 ) == 1 );
    static_assert( BufferSize( 7 ) == 1 );
    static_assert( BufferSize( 8 ) == 1 );
    static_assert( BufferSize( 9 ) == 2 );
    static_assert( BufferSize( 16 ) == 2 );
    static_assert( BufferSize( 17 ) == 3 );
    static_assert( BufferSize( 24 ) == 3 );
    static_assert( BufferSize( ( std::numeric_limits< size_t >::max( ) - kBitsPerByte ) + 1 )
                   == std::numeric_limits< size_t >::max( ) / kBitsPerByte );

    EXPECT_EQ( BufferSize( 0 ), 0 );
    EXPECT_EQ( BufferSize( 1 ), 1 );
    EXPECT_EQ( BufferSize( 7 ), 1 );
    EXPECT_EQ( BufferSize( 8 ), 1 );
    EXPECT_EQ( BufferSize( 9 ), 2 );
    EXPECT_EQ( BufferSize( 16 ), 2 );
    EXPECT_EQ( BufferSize( 17 ), 3 );
    EXPECT_EQ( BufferSize( 24 ), 3 );
    EXPECT_EQ( BufferSize( ( std::numeric_limits< size_t >::max( ) - kBitsPerByte ) + 1 ),
               std::numeric_limits< size_t >::max( ) / kBitsPerByte );
}
