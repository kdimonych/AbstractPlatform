#include <AbstractPlatform/common/BitOperations.hpp>
#include <AbstractPlatform/common/SimpleMath.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <type_traits>

using namespace AbstractPlatform;

namespace {
}

template <typename T>
struct SimpleMathGroupTest : public testing::Test
{
  using TType = T;
};

using TSimpleMathGroupTestTypes = testing::Types<std::uint8_t,
                                                 std::uint16_t,
                                                 std::uint32_t,
                                                 std::uint64_t,
                                                 std::int8_t,
                                                 std::int16_t,
                                                 std::int32_t,
                                                 std::int64_t>;

TYPED_TEST_SUITE(SimpleMathGroupTest, TSimpleMathGroupTestTypes);

TYPED_TEST(SimpleMathGroupTest, IsPowerOfTwo)
{
  using TType = typename TestFixture::TType;

  static_assert(!IsPowerOfTwo(TType{0}), "0 should not be a power of two");
  static_assert(IsPowerOfTwo(TType{1}), "1 should be a power of two");
  static_assert(IsPowerOfTwo(TType{2}), "2 should be a power of two");
  static_assert(!IsPowerOfTwo(TType{3}), "3 should not be a power of two");
  static_assert(IsPowerOfTwo(TType{4}), "4 should be a power of two");
  static_assert(!IsPowerOfTwo(TType{5}), "5 should not be a power of two");
  static_assert(IsPowerOfTwo(TType{8}), "8 should be a power of two");

  if constexpr (std::is_signed_v<TType>)
  {
    static_assert(!IsPowerOfTwo(TType{-1}), "-1 should not be a power of two");
    static_assert(!IsPowerOfTwo(TType{-2}), "-2 should not be a power of two");
    static_assert(!IsPowerOfTwo(TType{-3}), "-3 should not be a power of two");
    static_assert(!IsPowerOfTwo(TType{-4}), "-4 should not be a power of two");
    static_assert(!IsPowerOfTwo(TType{-5}), "-5 should not be a power of two");
    static_assert(!IsPowerOfTwo(TType{-8}), "-8 should not be a power of two");
  }

  constexpr size_t kBits     = sizeof(TType) * kBitsPerByte;
  constexpr TType  kMaxValue = static_cast<TType>(TType{1} << (kBits - 1));

  for (auto i = TType{1}; i < kMaxValue; i *= 2)
  {
    EXPECT_TRUE(IsPowerOfTwo(i)) << "Failed for value: " << static_cast<int>(i);
  }
}

TEST(SimpleMathTest, GetClosestPowerOfTwo)
{
  // Compile time test
  static_assert(GetClosestPowerOfTwo(0) == std::numeric_limits<size_t>::max(), "Failed for 0");
  static_assert(GetClosestPowerOfTwo(1) == 0, "Failed for 1");
  static_assert(GetClosestPowerOfTwo(2) == 1, "Failed for 2");
  static_assert(GetClosestPowerOfTwo(3) == 1, "Failed for 3");
  static_assert(GetClosestPowerOfTwo(4) == 2, "Failed for 4");
  static_assert(GetClosestPowerOfTwo(5) == 2, "Failed for 5");
  static_assert(GetClosestPowerOfTwo(6) == 2, "Failed for 6");
  static_assert(GetClosestPowerOfTwo(7) == 2, "Failed for 7");
  static_assert(GetClosestPowerOfTwo(8) == 3, "Failed for 8");
  // Runtime test
  EXPECT_EQ(GetClosestPowerOfTwo(0), std::numeric_limits<size_t>::max());
  EXPECT_EQ(GetClosestPowerOfTwo(1), 0);
  EXPECT_EQ(GetClosestPowerOfTwo(2), 1);
  EXPECT_EQ(GetClosestPowerOfTwo(3), 1);
  EXPECT_EQ(GetClosestPowerOfTwo(4), 2);
  EXPECT_EQ(GetClosestPowerOfTwo(5), 2);
  EXPECT_EQ(GetClosestPowerOfTwo(6), 2);
  EXPECT_EQ(GetClosestPowerOfTwo(7), 2);
  EXPECT_EQ(GetClosestPowerOfTwo(8), 3);
}

TEST(SimpleMathTest, GetClosestPowerOfTwoDivider)
{
  // Compile time test
  static_assert(GetClosestPowerOfTwoDivider(0) == std::numeric_limits<size_t>::max(),
                "Failed for 0");
  static_assert(GetClosestPowerOfTwoDivider(1) == 0, "Failed for 1");
  static_assert(GetClosestPowerOfTwoDivider(2) == 1, "Failed for 2");
  static_assert(GetClosestPowerOfTwoDivider(3) == 0, "Failed for 3");
  static_assert(GetClosestPowerOfTwoDivider(4) == 2, "Failed for 4");
  static_assert(GetClosestPowerOfTwoDivider(5) == 0, "Failed for 5");
  static_assert(GetClosestPowerOfTwoDivider(6) == 1, "Failed for 6");
  static_assert(GetClosestPowerOfTwoDivider(7) == 0, "Failed for 7");
  static_assert(GetClosestPowerOfTwoDivider(8) == 3, "Failed for 8");
  // Runtime test
  EXPECT_EQ(GetClosestPowerOfTwoDivider(0), std::numeric_limits<size_t>::max());
  EXPECT_EQ(GetClosestPowerOfTwoDivider(1), 0);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(2), 1);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(3), 0);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(4), 2);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(5), 0);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(6), 1);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(7), 0);
  EXPECT_EQ(GetClosestPowerOfTwoDivider(8), 3);
}
