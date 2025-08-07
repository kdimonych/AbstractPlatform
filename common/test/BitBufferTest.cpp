#include <AbstractPlatform/common/BitBuffer.hpp>
#include <AbstractPlatform/common/BitOperations.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

namespace {
}

template <typename T>
struct BitBufferGroupTest : public testing::Test
{
  using TType = T;
};

using TBitBufferGroupTestTypes =
  testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

TYPED_TEST_SUITE(BitBufferGroupTest, TBitBufferGroupTestTypes);

/********************* TBufferLayout ********************/

TYPED_TEST(BitBufferGroupTest, TBufferLayout_LayoutConstants)
{
  using TBlockType = typename TestFixture::TType;

  constexpr size_t kBlockSize = sizeof(TBlockType);
  constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;
  using TBufferLayout         = TBufferLayout<TBlockType>;

  static_assert(TBufferLayout::kBlockSize == kBlockSize, "Expected sizeof(TBlockType)");
  static_assert(TBufferLayout::kBlockBits == kBlockBits, "Expected sizeof(TBlockType)");
}

TYPED_TEST(BitBufferGroupTest, TBufferLayout_BlockBitIndex)
{
  using TBlockType            = typename TestFixture::TType;
  constexpr size_t kBlockSize = sizeof(TBlockType);
  constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;
  using TBufferLayout         = TBufferLayout<TBlockType>;

  size_t expectedBitShift = 0;
  // Test behavior for native endianness
  for (size_t i = 0; i < kBlockBits * 5; ++i)
  {
    const auto shift = TBufferLayout::BlockBitIndex(i);
    EXPECT_EQ(shift, expectedBitShift) << "Failed for global bit index: " << i;
    (++expectedBitShift) %= kBlockBits;
  }
}

TYPED_TEST(BitBufferGroupTest, TBufferLayout_Inc)
{
  using TBlockType            = typename TestFixture::TType;
  constexpr size_t kBlockSize = sizeof(TBlockType);
  constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;
  using TBufferLayout         = TBufferLayout<TBlockType>;

  size_t expectedInBlockBitIndex = 0;
  size_t inBlockBitIndex         = 0;

  // Test behavior for native endianness
  for (size_t i = 0; i < kBlockBits * 5; ++i)
  {
    (++expectedInBlockBitIndex) %= kBlockBits;
    const size_t expectedBlockIndexAdd = (i + 1) % (kBlockBits) == 0 ? 1 : 0;
    const auto   blockIndexAdd         = TBufferLayout::Inc(inBlockBitIndex);
    EXPECT_EQ(blockIndexAdd, expectedBlockIndexAdd)
      << "Failed for global bit index: " << i << ", inBlockBitIndex: " << inBlockBitIndex;
    EXPECT_EQ(inBlockBitIndex, expectedInBlockBitIndex)
      << "Failed for global bit index: " << i << ", blockIndexAdd: " << blockIndexAdd;
  }
}

TYPED_TEST(BitBufferGroupTest, TBufferLayout_Dec)
{
  using TBlockType            = typename TestFixture::TType;
  constexpr size_t kBlockSize = sizeof(TBlockType);
  constexpr size_t kBlockBits = kBlockSize * kBitsPerByte;
  using TBufferLayout         = TBufferLayout<TBlockType>;

  size_t expectedInBlockBitIndex = 0;
  size_t inBlockBitIndex         = 0;

  // Test behavior for native endianness
  for (size_t i = 0; i < kBlockBits * 5; ++i)
  {
    (++expectedInBlockBitIndex) %= kBlockBits;
    const size_t expectedBlockIndexAdd = (i + 1) % (kBlockBits) == 0 ? 1 : 0;
    const auto   blockIndexAdd         = TBufferLayout::Inc(inBlockBitIndex);
    EXPECT_EQ(blockIndexAdd, expectedBlockIndexAdd)
      << "Failed for global bit index: " << i << ", inBlockBitIndex: " << inBlockBitIndex;
    EXPECT_EQ(inBlockBitIndex, expectedInBlockBitIndex)
      << "Failed for global bit index: " << i << ", blockIndexAdd: " << blockIndexAdd;
  }
}

/********************* TBufferLayout ********************/
