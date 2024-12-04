#pragma once
#include <AbstractPlatform/common/Platform.hpp>
#include <cstring>

namespace AbstractPlatform
{
template < typename DestinationBufferType, typename taScalarType >
DestinationBufferType*
ScalarTypeCopy( DestinationBufferType* aDestinationBuffer, taScalarType aSourceValue )
{
    static_assert( sizeof( taScalarType ) >= sizeof( DestinationBufferType ),
                   "Source scalar type size less than destination element type size" );
    static_assert(
        sizeof( taScalarType ) % sizeof( DestinationBufferType ) == 0u,
        "Word alignment mismatch between destination element type and source scalar type" );
    return static_cast< DestinationBufferType* >( std::memcpy(
        aDestinationBuffer, static_cast< void* >( &aSourceValue ), sizeof( taScalarType ) ) );
}
}  // namespace AbstractPlatform