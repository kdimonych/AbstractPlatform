#pragma once

#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace AbstractPlatform {

namespace Impl {
inline static constexpr size_t
GetClosestPowerOfTwoDividerImpl(const size_t aValue, const size_t aStartPower, const size_t aRest);
} // namespace Impl

template <typename taValue>
inline static constexpr bool
IsPowerOfTwo(taValue value, typename std::enable_if<std::is_integral<taValue>::value>::type* = 0)
{
  return value != taValue{0} && (value & (value - 1)) == taValue{0};
}

/**
 * @brief Calculate the closest power of two that produces value less than aValue
 *
 * @param aValue The value to find the closest power of two for
 * @param aStartPower The starting power of two to check from
 * @return constexpr size_t The closest power of two
 */
inline static constexpr size_t
GetClosestPowerOfTwo(const size_t aValue,
                     const size_t aStartPower = sizeof(size_t) * kBitsPerByte - 1)
{
  assert(aStartPower < sizeof(size_t) * kBitsPerByte);
  return aValue == size_t{0}
           ? std::numeric_limits<size_t>::max()
           : ((size_t{1} << aStartPower) <= aValue ? aStartPower
                                                   : GetClosestPowerOfTwo(aValue, aStartPower - 1));
}

/**
 * @brief Calculates the power of two value which produces the largest divider of aValue
 *
 * @param aValue The value to find the closest power of two divider for
 * @param aStartPower The starting power of two to check from
 * @return constexpr size_t The closest power of two divider
 */
inline static constexpr size_t
GetClosestPowerOfTwoDivider(const size_t aValue,
                            const size_t aStartPower = sizeof(size_t) * kBitsPerByte - 1)
{
  return aValue == 0 ? std::numeric_limits<size_t>::max()
                     : Impl::GetClosestPowerOfTwoDividerImpl(aValue,
                                                             aStartPower,
                                                             aValue % (size_t{1} << aStartPower));
}

/************************************* Implementation Details *************************************/

namespace Impl {
inline static constexpr size_t
GetClosestPowerOfTwoDividerImpl(const size_t aValue, const size_t aStartPower, const size_t aRest)
{
  return (aRest == 0) ? aStartPower
                      : GetClosestPowerOfTwoDividerImpl(aValue,
                                                        aStartPower - 1,
                                                        aValue % (size_t{1} << (aStartPower - 1)));
}

} //   namespace Impl

} // namespace AbstractPlatform
