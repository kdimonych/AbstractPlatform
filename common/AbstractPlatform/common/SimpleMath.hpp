#pragma once

#include <cstddef>
#include <type_traits>

namespace AbstractPlatform {

template <typename taValue>
inline static constexpr bool
IsPowerOfTwo(taValue value, typename std::enable_if<std::is_integral<taValue>::value>::type* = 0)
{
  return value != taValue{0} && (value & (value - 1)) == taValue{0};
}

} // namespace AbstractPlatform
