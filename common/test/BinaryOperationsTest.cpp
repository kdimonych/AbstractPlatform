#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/BitOperations.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

namespace {
}

template <typename T>
struct BinaryOperationsGroupTest : public testing::Test
{
  using TType = T;
};

using TBinaryOperationsGroupTestTypes = testing::Types<std::uint8_t,
                                                       std::uint16_t,
                                                       std::uint32_t,
                                                       std::uint64_t,
                                                       std::int8_t,
                                                       std::int16_t,
                                                       std::int32_t,
                                                       std::int64_t,
                                                       std::size_t>;
TYPED_TEST_SUITE(BinaryOperationsGroupTest, TBinaryOperationsGroupTestTypes);

template <typename taT>
inline static constexpr taT SetBitTest(const size_t aBit)
{
  return taT{1} << aBit;
}

template <typename taDataType, size_t taIndex>
inline static constexpr bool StaticBitOperationsTest()
{
  constexpr taDataType kAllSetValue     = AllBitsSet<taDataType>();
  constexpr taDataType kZeroValue       = taDataType{};
  constexpr taDataType kExpectedBit     = SetBitTest<taDataType>(taIndex);
  constexpr taDataType kExpectedBitMask = kAllSetValue ^ kExpectedBit;

  static_assert(CheckBit(kExpectedBit, taIndex) == true);
  static_assert(CheckBit(kZeroValue, taIndex) == false);
  static_assert(CheckBit(kAllSetValue, taIndex) == true);

  static_assert(SetBit(kZeroValue, taIndex) == kExpectedBit);
  static_assert(SetBit(kAllSetValue, taIndex) == kAllSetValue);
  static_assert(SetBit(kExpectedBitMask, taIndex) == kAllSetValue);
  static_assert(SetBit(kExpectedBit, taIndex) == kExpectedBit);

  static_assert(ClearBit(kZeroValue, taIndex) == kZeroValue);
  static_assert(ClearBit(kAllSetValue, taIndex) == kExpectedBitMask);
  static_assert(ClearBit(kExpectedBitMask, taIndex) == kExpectedBitMask);
  static_assert(ClearBit(kExpectedBit, taIndex) == kZeroValue);

  static_assert(ToggleBit(kZeroValue, taIndex) == kExpectedBit);
  static_assert(ToggleBit(kAllSetValue, taIndex) == kExpectedBitMask);
  static_assert(ToggleBit(kExpectedBitMask, taIndex) == kAllSetValue);
  static_assert(ToggleBit(kExpectedBit, taIndex) == kZeroValue);

  return true;
}

template <typename taDataType, typename taIndexType, taIndexType... taIndexes>
inline static constexpr bool
StaticOperationsTestLoop(std::integer_sequence<taIndexType, taIndexes...>)
{
  return (StaticBitOperationsTest<taDataType, taIndexes>(), ...), true;
}

TYPED_TEST(BinaryOperationsGroupTest, MainOperation)
{
  using TType = typename TestFixture::TType;

  static_assert(StaticOperationsTestLoop<TType>(std::make_index_sequence<BitSize(TType{})>{}));

  auto checkForBit = [](size_t aIndex) {
    const TType kAllSetValue     = AllBitsSet<TType>();
    const TType kZeroValue       = TType{};
    const TType kExpectedBit     = SetBitTest<TType>(aIndex);
    const TType kExpectedBitMask = kAllSetValue ^ kExpectedBit;

    EXPECT_TRUE(CheckBit(kExpectedBit, aIndex));
    EXPECT_FALSE(CheckBit(kZeroValue, aIndex));
    EXPECT_TRUE(CheckBit(kAllSetValue, aIndex));

    EXPECT_EQ(SetBit(kZeroValue, aIndex), kExpectedBit);
    EXPECT_EQ(SetBit(kAllSetValue, aIndex), kAllSetValue);
    EXPECT_EQ(SetBit(kExpectedBitMask, aIndex), kAllSetValue);
    EXPECT_EQ(SetBit(kExpectedBit, aIndex), kExpectedBit);

    EXPECT_EQ(ClearBit(kZeroValue, aIndex), kZeroValue);
    EXPECT_EQ(ClearBit(kAllSetValue, aIndex), kExpectedBitMask);
    EXPECT_EQ(ClearBit(kExpectedBitMask, aIndex), kExpectedBitMask);
    EXPECT_EQ(ClearBit(kExpectedBit, aIndex), kZeroValue);

    EXPECT_EQ(ToggleBit(kZeroValue, aIndex), kExpectedBit);
    EXPECT_EQ(ToggleBit(kAllSetValue, aIndex), kExpectedBitMask);
    EXPECT_EQ(ToggleBit(kExpectedBitMask, aIndex), kAllSetValue);
    EXPECT_EQ(ToggleBit(kExpectedBit, aIndex), kZeroValue);
  };

  for (size_t bit = 0; bit < BitSize(TType{}); ++bit)
  {
    checkForBit(bit);
  }
}

template <typename taDataType, size_t taFirstIndex, size_t taLastIndex>
inline static constexpr bool StaticRangeToBitMaskTest()
{
  constexpr size_t kFirstIndex = std::min(taFirstIndex, taLastIndex);
  constexpr size_t kLastIndex  = std::max(taFirstIndex, taLastIndex);
  using TProxyType             = typename SizeCompatible<taDataType>::TType;

  constexpr auto kExpectedValue = static_cast<taDataType>(
    (AllBitsSet<TProxyType>() << kFirstIndex) ^ (AllBitsSet<TProxyType>() << kLastIndex)
    | (TProxyType{1} << kLastIndex));

  static_assert(BitRangeToMask<taDataType>(kFirstIndex, kLastIndex) == kExpectedValue,
                "BitRangeToMask does not return expected value");
  return true;
}

template <typename taDataType,
          size_t taFirstIndex,
          typename taLastIndexType,
          taLastIndexType... taLastIndexes>
inline static constexpr bool
StaticRangeToBitMaskTestLoopEnd(std::integer_sequence<taLastIndexType, taLastIndexes...>)
{
  return (StaticRangeToBitMaskTest<taDataType, taFirstIndex, taLastIndexes>(), ...), true;
}

template <typename taDataType, typename taFirstIndexType, taFirstIndexType... taFirstIndexes>
inline static constexpr bool
StaticRangeToBitMaskTestLoopStart(std::integer_sequence<taFirstIndexType, taFirstIndexes...>)
{
  return (StaticRangeToBitMaskTestLoopEnd<taDataType, taFirstIndexes>(
            std::make_index_sequence<BitSize(taDataType{})>{}),
          ...),
         true;
}

TYPED_TEST(BinaryOperationsGroupTest, RangeToBitMask)
{
  using TType      = typename TestFixture::TType;
  using TProxyType = typename SizeCompatible<TType>::TType;

  static_assert(
    StaticRangeToBitMaskTestLoopStart<TType>(std::make_index_sequence<BitSize(TType{})>{}));

  for (size_t endIndex = BitSize(TType{}); endIndex > 0; --endIndex)
  {
    for (size_t firstIndex = 0; firstIndex < endIndex; --firstIndex)
    {
      const size_t lastIndex     = endIndex - 1;
      const auto   expectedValue = static_cast<TType>((AllBitsSet<TProxyType>() << firstIndex)
                                                      ^ (AllBitsSet<TProxyType>() << lastIndex)
                                                    | (TProxyType{1} << lastIndex));

      EXPECT_EQ(BitRangeToMask<TType>(firstIndex, lastIndex), expectedValue)
        << "BitRangeToMask does not return expected value";
    }
  }
}

TEST(BinaryOperationsTest, RangeToBitMaskSimple)
{
  {
    using TType                  = std::uint8_t;
    constexpr TType  kExpected   = 0b11100000;
    constexpr size_t kFirstIndex = 5;
    constexpr size_t kLastIndex  = 8;

    static_assert(BitRangeToMask<TType>(kFirstIndex, kLastIndex) == kExpected,
                  "BitRangeToMask does not return expected value for std::uint8_t");
    EXPECT_EQ(BitRangeToMask<TType>(kFirstIndex, kLastIndex), kExpected)
      << "BitRangeToMask does not return expected value";
  }

  {
    using TType                  = std::uint16_t;
    constexpr TType  kExpected   = 0b00011111111100000;
    constexpr size_t kFirstIndex = 5;
    constexpr size_t kLastIndex  = 13;

    static_assert(BitRangeToMask<TType>(kFirstIndex, kLastIndex) == kExpected,
                  "BitRangeToMask does not return expected value for std::uint8_t");
    EXPECT_EQ(BitRangeToMask<TType>(kFirstIndex, kLastIndex), kExpected)
      << "BitRangeToMask does not return expected value";
  }
}
