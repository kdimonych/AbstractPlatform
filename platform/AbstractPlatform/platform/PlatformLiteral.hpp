#pragma once

#include <cstdint>

inline static constexpr std::uint16_t operator"" _u16(unsigned long long value)
{
  return static_cast<std::uint16_t>(value);
}

inline static constexpr std::int16_t operator"" _i16(unsigned long long value)
{
  return static_cast<std::int16_t>(value);
}

inline static constexpr std::uint8_t operator"" _u8(unsigned long long value)
{
  return static_cast<std::uint8_t>(value);
}

inline static constexpr std::int8_t operator"" _i8(unsigned long long value)
{
  return static_cast<std::int8_t>(value);
}
