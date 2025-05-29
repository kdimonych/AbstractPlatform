#pragma once

#include <AbstractPlatform/common/BitOperations.hpp>

#include <algorithm>
#include <type_traits>

namespace AbstractPlatform {
/**
 * @brief Sets a specific bit in the value.
 * This function sets the bit at the specified index to 1.
 *
 * @tparam taValue The type of the value to set the bit in.
 *                 It should be an integral type.
 * @param aValue The value in which to set the bit.
 * @param aBitIndex The index of the bit to set (0-based).
 * @return constexpr taValue The value with the specified bit set.
 * @note The aBitIndex should be less than the bit size of taValue.
 *       Passing an index that is out of bounds may lead to undefined behavior.
 */
template <typename taValue>
inline static constexpr taValue SetBit(taValue aValue, size_t aBitIndex)
{
  static_assert(std::is_integral<taValue>::value);

  using TTargetType = typename SizeCompatible<taValue>::TType;
  return static_cast<taValue>(static_cast<TTargetType>(aValue) | (TTargetType{1} << aBitIndex));
}

template <typename taValue>
inline static constexpr taValue ClearBit(taValue aValue, size_t aBitIndex)
{
  static_assert(std::is_integral<taValue>::value);

  using TTargetType = typename SizeCompatible<taValue>::TType;
  return static_cast<taValue>(static_cast<TTargetType>(aValue) & ~(TTargetType{1} << aBitIndex));
}

/**
 * @brief Toggles a specific bit in the value.
 * This function flips the bit at the specified index from 0 to 1 or from 1 to 0.
 *
 * @tparam taValue The type of the value to toggle the bit in.
 *                 It should be an integral type.
 * @param aValue The value in which to toggle the bit.
 * @param aBitIndex The index of the bit to toggle (0-based).
 * @return constexpr taValue The value with the specified bit toggled.
 * @note The aBitIndex should be less than the bit size of taValue.
 *       Passing an index that is out of bounds may lead to undefined behavior.
 */
template <typename taValue>
inline static constexpr taValue ToggleBit(taValue aValue, size_t aBitIndex)
{
  static_assert(std::is_integral<taValue>::value);

  using TTargetType = typename SizeCompatible<taValue>::TType;
  return static_cast<taValue>(static_cast<TTargetType>(aValue) ^ (TTargetType{1} << aBitIndex));
}

/**
 * @brief Checks if a specific bit is set in the value.
 * This function checks if the bit at the specified index is set to 1.
 *
 * @tparam taValue The type of the value to check.
 *                 It should be an integral type.
 * @param aValue The value to check.
 * @param aBitIndex The index of the bit to check (0-based).
 * @return constexpr bool True if the bit is set, false otherwise.
 * @note The aBitIndex should be less than the bit size of taValue.
 *       Passing an index that is out of bounds may lead to undefined behavior.
 */
template <typename taValue>
inline static constexpr bool CheckBit(taValue aValue, size_t aBitIndex)
{
  static_assert(std::is_integral<taValue>::value);

  using TTargetType = typename SizeCompatible<taValue>::TType;
  return static_cast<bool>((static_cast<TTargetType>(aValue) >> aBitIndex) & TTargetType{1});
}

/**
 * @brief Returns a value with all bits set.
 * This function returns a value where all bits are set to 1.
 *
 * @tparam taValue The type of the value for which all bits are set.
 *                 It should be an integral type.
 * @return constexpr auto A value with all bits set.
 */
template <typename taValue>
inline static constexpr auto AllBitsSet()
{
  static_assert(std::is_integral<taValue>::value);

  using TProxyType = typename SizeCompatible<taValue>::TType;
  return static_cast<taValue>(std::numeric_limits<TProxyType>::max());
}

/**
 * @brief Returns a value with all bits cleared.
 * This function returns a value where all bits are set to 0.
 *
 * @tparam taValue The type of the value for which all bits are cleared.
 *                 It should be an integral type.
 * @return constexpr auto A value with all bits cleared.
 */
template <typename taValue>
inline static constexpr auto AllBitsCleared()
{
  static_assert(std::is_integral<taValue>::value);

  using TProxyType = typename SizeCompatible<taValue>::TType;
  return static_cast<taValue>(std::numeric_limits<TProxyType>::max());
}

/**
 * @brief Returns a mask for the specified bit range.
 * The mask is created by setting all bits from aStartBitIndex to aEndBitIndex.
 * @note The aStartBitIndex is inclusive, while aEndBitIndex is exclusive.
 * @note The aStartBitIndex should be less than or equal to aEndBitIndex,
 *
 *
 * @tparam taValue The type of the value for which the mask is created.
 *                 It should be an integral type.
 * @param aFirstBitIndex The first bit index (inclusive).
 * @param aLastBitIndex The last bit index (inclusive).
 * @return constexpr auto The mask for the specified bit range.
 * @note The aStartBitIndex should be less than or equal to aLastBitIndex, and both should be within
 *       the bit size of taValue.
 *       Passing invalid indices may lead to undefined behavior.
 */
template <typename taValue>
inline static constexpr auto BitRangeToMask(size_t aFirstBitIndex, size_t aLastBitIndex) NOEXCEPT
{
  static_assert(std::is_integral<taValue>::value, "taValue should be an integral type");

  using TProxyType = typename SizeCompatible<taValue>::TType;

  return static_cast<taValue>(AllBitsSet<TProxyType>() << aFirstBitIndex
                                ^ (AllBitsSet<TProxyType>() << aLastBitIndex)
                              | (TProxyType{1} << aLastBitIndex));
}

} // namespace AbstractPlatform
