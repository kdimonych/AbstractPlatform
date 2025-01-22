#pragma once
#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>

#include <cstddef>

namespace AbstractPlatform
{
template < typename taArrayElement, size_t taArrayElemetsCount >
constexpr inline size_t
ArrayLength( const taArrayElement ( & )[ taArrayElemetsCount ] )
{
    return taArrayElemetsCount;
}
template < typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM >
constexpr inline size_t
ArrayLength( const taArrayElement ( & )[ taArrayElemetsN ][ taArrayElemetsM ] )
{
    return taArrayElemetsN * taArrayElemetsM;
}

template < typename taArrayElement, size_t taArrayElemetsCount >
constexpr inline size_t
ArraySizeBytes( const taArrayElement ( & )[ taArrayElemetsCount ] )
{
    return taArrayElemetsCount * sizeof( taArrayElement );
}
template < typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM >
constexpr inline size_t
ArraySizeBytes( const taArrayElement ( & )[ taArrayElemetsN ][ taArrayElemetsM ] )
{
    return taArrayElemetsN * taArrayElemetsM * sizeof( taArrayElement );
}

template < typename taArrayElement, size_t taArrayElemetsCount >
constexpr inline size_t
BitSize( const taArrayElement ( & )[ taArrayElemetsCount ] )
{
    return sizeof( taArrayElement ) * kBitsPerByte * taArrayElemetsCount;
}

template < typename taArrayElement, size_t taArrayElemetsN, size_t taArrayElemetsM >
constexpr inline size_t
BitSize( const taArrayElement ( & )[ taArrayElemetsN ][ taArrayElemetsM ] )
{
    return taArrayElemetsN * taArrayElemetsM * sizeof( taArrayElement ) * kBitsPerByte;
}

}  // namespace AbstractPlatform