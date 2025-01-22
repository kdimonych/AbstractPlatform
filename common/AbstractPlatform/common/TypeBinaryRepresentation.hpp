#pragma once

#include <AbstractPlatform/common/Platform.hpp>

#include <cstdint>
#include <algorithm>
#include <iterator>

namespace AbstractPlatform
{

static constexpr size_t kBitsPerByte = 8;

/**
 * @brief Determines scalar type endianess
 *
 *  The 32 bit value a = 0x0A0B0C0D will be arranged in the memory the following way:
 */
enum class Endianness
{
    Little,  // a[0] = 0D, a[1] = 0C, ... a[3] = 0A
    Big,     // a[0] = 0A, a[1] = 0B, ... a[3] = 0D
    Native = Little
};

template < typename taT >
static constexpr taT
ByteSwap( taT aValue ) NOEXCEPT
{
    using TProxyArray = std::uint8_t( & )[ sizeof( taT ) ];
    auto& byteArray = reinterpret_cast< TProxyArray& >( aValue );
    std::reverse( std::begin( byteArray ), std::end( byteArray ) );
    return aValue;
}

template < Endianness taToEndianness = Endianness::Native,
           Endianness taFromEndianness = Endianness::Native >
struct EndiannessConverter
{
    // The combinations: Endianness::Little -> Endianness::Little and Endianness::Big ->
    // Endianness::Big

    template < typename taValueType >
    static constexpr inline taValueType
    Convert( taValueType&& aSourceValue ) NOEXCEPT
    {
        return std::forward< taValueType >( aSourceValue );
    }
};

template <>
struct EndiannessConverter< Endianness::Big, Endianness::Little >
{
    // The combinations: Endianness::Little -> Endianness::Big and Endianness::Big ->
    // Endianness::Little;

    template < typename taValueType >
    static inline taValueType
    Convert( taValueType aSourceValue ) NOEXCEPT
    {
        return ByteSwap( aSourceValue );
    }
};

template <>
struct EndiannessConverter< Endianness::Little, Endianness::Big >
{
    template < typename taValueType >
    static inline taValueType
    Convert( taValueType aSourceValue ) NOEXCEPT
    {
        return ByteSwap( aSourceValue );
    }
};

template < typename taValue >
static constexpr inline auto
BitSize( taValue )
{
    return sizeof( taValue ) * kBitsPerByte;
}

template < size_t taSize >
struct SizeCompatibleImpl
{
    static_assert( taSize <= 8 );
    using TType = std::uint64_t;
};

template <>
struct SizeCompatibleImpl< 1 >
{
    using TType = std::uint8_t;
};

template <>
struct SizeCompatibleImpl< 2 >
{
    using TType = std::uint16_t;
};

template <>
struct SizeCompatibleImpl< 4 >
{
    using TType = std::uint32_t;
};

template < typename taValue >
struct SizeCompatible
{
    using TType = typename SizeCompatibleImpl< sizeof( taValue ) >::TType;
};

}  // namespace AbstractPlatform