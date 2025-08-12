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
struct IntegralValuesGroupTest : public testing::Test
{
  using TType = T;
};

using TIntegralValuesGroupTestTypes = testing::Types<std::uint8_t,
                                                     std::uint16_t,
                                                     std::uint32_t,
                                                     std::uint64_t,
                                                     std::int8_t,
                                                     std::int16_t,
                                                     std::int32_t,
                                                     std::int64_t>;

TYPED_TEST_SUITE(IntegralValuesGroupTest, TIntegralValuesGroupTestTypes);

TYPED_TEST(IntegralValuesGroupTest, IsPowerOfTwo)
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
