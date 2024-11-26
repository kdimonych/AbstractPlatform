#pragma once

namespace AbstractPlatform
{
template < typename taArrayElement, size_t taArrayElemets >
constexpr inline size_t
ArrayLength( const taArrayElement ( & )[ taArrayElemets ] )
{
    return taArrayElemets;
}
template < typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM >
constexpr inline size_t
ArrayLength( const taArrayElement ( & )[ taArrayElemetsN ][ taArrayElemetsM ] )
{
    return taArrayElemetsN * taArrayElemetsM;
}

}  // namespace AbstractPlatform