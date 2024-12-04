#pragma once

namespace AbstractPlatform
{

template < typename taValue >
static constexpr inline taValue
SetBit( taValue aValue, size_t aBitIndex )
{
    return static_cast< taValue >( aValue | ( taValue{ 1 } << aBitIndex ) );
}

template < typename taValue >
static constexpr inline taValue
ClearBit( taValue aValue, size_t aBitIndex )
{
    return static_cast< taValue >( aValue & ~( taValue{ 1 } << aBitIndex ) );
}

template < typename taValue >
static constexpr inline taValue
ToggleBit( taValue aValue, size_t aBitIndex )
{
    return static_cast< taValue >( aValue ^ ( taValue{ 1 } << aBitIndex ) );
}

template < typename taValue >
static constexpr inline bool
CheckBit( taValue aValue, size_t aBitIndex )
{
    return static_cast< bool >( ( aValue >> aBitIndex ) & taValue{ 1 } );
}
}  // namespace AbstractPlatform