#include <gtest/gtest.h>

#include <AbstractPlatform/common/Memory.hpp>

#include <utility>
#include <cstdint>
#include <limits>

using namespace AbstractPlatform;
namespace
{
}

template < typename T >
struct ScalarTypeCopyTest : public testing::Test
{
    using TDestinationBufferType = typename T::first_type;
    using TSourceScalarType = typename T::second_type;
};

using TScalarTypes = testing::Types< std::pair< std::uint8_t, std::uint8_t >,
                                     std::pair< std::uint8_t, std::uint16_t >,
                                     std::pair< std::uint8_t, std::uint32_t >,
                                     std::pair< std::uint8_t, float >,
                                     std::pair< std::uint16_t, std::uint16_t >,
                                     std::pair< std::uint16_t, std::uint32_t >,
                                     std::pair< std::uint16_t, float >,
                                     std::pair< std::uint32_t, std::uint32_t >,
                                     std::pair< std::uint32_t, float > >;
TYPED_TEST_SUITE( ScalarTypeCopyTest, TScalarTypes );

TYPED_TEST( ScalarTypeCopyTest, EqualSizedCopy )
{
    using TSourceScalarType = typename TestFixture::TSourceScalarType;
    using TDestinationBufferType = typename TestFixture::TDestinationBufferType;

    constexpr auto kDestinationBufferCount
        = sizeof( TSourceScalarType ) / sizeof( TDestinationBufferType );

    TDestinationBufferType destination[ kDestinationBufferCount ];
    std::memset( destination, 0, sizeof( destination ) );

    TSourceScalarType scalar;
    for ( size_t i = 0; i < kDestinationBufferCount; ++i )
    {
        reinterpret_cast< TDestinationBufferType* >( &scalar )[ i ]
            = static_cast< TDestinationBufferType >( i );
    }

    auto nextDestination = ScalarTypeCopy( destination, scalar );
    EXPECT_EQ( nextDestination, destination );

    for ( size_t i = 0; i < kDestinationBufferCount; ++i )
    {
        EXPECT_EQ( reinterpret_cast< TDestinationBufferType* >( &scalar )[ i ], destination[ i ] );
    }
}