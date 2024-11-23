#include <cstdint>

constexpr inline std::uint16_t operator"" _u16( unsigned long long value )
{
    return static_cast< std::uint16_t >( value );
}

constexpr inline std::int16_t operator"" _i16( unsigned long long value )
{
    return static_cast< std::int16_t >( value );
}

constexpr inline std::uint8_t operator"" _u8( unsigned long long value )
{
    return static_cast< std::uint8_t >( value );
}

constexpr inline std::int8_t operator"" _i8( unsigned long long value )
{
    return static_cast< std::int8_t >( value );
}