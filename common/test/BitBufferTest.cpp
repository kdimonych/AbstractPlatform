#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/BitBuffer.hpp>
#include <AbstractPlatform/common/BitOperations.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
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

/********************* TBufferLayout *********************/

template <typename taBlockType, Endian taBlockEndian>
auto TBitBufferCreationTest()
{
  using TBlockType              = taBlockType;
  constexpr size_t kBlockSize   = sizeof(TBlockType);
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  constexpr Endian kBlockEndian = taBlockEndian;
  constexpr size_t kBitSize     = kBlockBits * 5;
  constexpr size_t kByteSize    = kBitSize / kBitsPerByte;
  constexpr size_t kBufferSize  = kBitSize / kBlockBits;
  using TBitBuffer              = TBitBuffer<kBitSize, TBlockType, kBlockEndian>;

  TBitBuffer buffer = {TBlockType{0}, TBlockType{0}, TBlockType{0}, TBlockType{0}, TBlockType{0}};

  static_assert(TBitBuffer::kBlockSize == kBlockSize, "Expected sizeof(TBlockType)");
  static_assert(TBitBuffer::kBlockBits == kBlockBits, "Expected sizeof(TBlockType) * 8");
  if constexpr (kBlockEndian == Endian::Little)
  {
    static_assert(TBitBuffer::kBlockEndian == kBlockEndian, "Expected Endian::Little");
  }
  else
  {
    static_assert(TBitBuffer::kBlockEndian == kBlockEndian, "Expected Endian::Big");
  }

  static_assert(TBitBuffer::kBitSize == kBitSize, "Expected taBitSize");
  static_assert(TBitBuffer::Size() == kBitSize, "Expected kBitSize");
  static_assert(ArrayLength(TBitBuffer{}) == kBitSize, "Expected kBitSize");
  static_assert(BitSize(TBitBuffer{}) == kBitSize, "Expected kBitSize");

  static_assert(TBitBuffer::kByteSize == kByteSize, "Expected kByteSize");
  static_assert(ArraySizeBytes(TBitBuffer{}) == kByteSize, "Expected kByteSize");

  static_assert(TBitBuffer::kBufferSize == kBufferSize, "Expected kBufferSize");
  static_assert(sizeof(TBitBuffer::iBuffer) == kBufferSize * kBlockSize,
                "Expected kBufferSize * kBlockSize");

  EXPECT_TRUE(buffer.None()) << "All bits have to be not set";
  EXPECT_FALSE(buffer.All()) << "All bits have to be not set";
  EXPECT_FALSE(buffer.Any()) << "All bits have to be not set";
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_Creation)
{
  using TBlockType = typename TestFixture::TType;
  TBitBufferCreationTest<TBlockType, Endian::Little>();
  TBitBufferCreationTest<TBlockType, Endian::Big>();
}

/**
 * @brief The bit pattern function.
 *
 * @tparam taBlockBits number of bits in a block
 * @param aIndex the index of bit
 * @return true or false as a function of aIndex and taBlockBits
 */
template <size_t taBlockBits>
inline static constexpr bool TestPattern(size_t aIndex)
{
  return ((aIndex / (taBlockBits / 2)) % 2) == 0;
}

template <typename taBlockType, Endian taBlockEndian>
auto CreatePeriodicPattern()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;
  constexpr size_t kBlockSize   = sizeof(TBlockType);
  using BitIndexMapper          = TEndianBitIndexMapper<kBlockSize, kBlockEndian>;
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;

  TBlockType       testBuff[5];
  constexpr size_t kBitSize = kBlockBits * ArrayLength(testBuff);

  for (size_t i = 0; i < kBitSize;)
  {
    TBlockType val = TBlockType{0};

    const auto block = i / kBlockBits;
    for (size_t j = 0; j < kBlockBits; ++j, i++)
    {
      const auto bitValue    = TestPattern<kBlockBits>(i);
      const auto mappedIndex = BitIndexMapper::MapIndex(j);
      val                    = bitValue ? SetBit(val, mappedIndex) : ClearBit(val, mappedIndex);
    }
    testBuff[block] = val;
  }

  std::cout << "Test buffer contents"
            << (kBlockEndian == Endian::Little ? " (little-endian): " : "    (big-endian): ");
  std::uint8_t* p = reinterpret_cast<std::uint8_t*>(testBuff);
  for (size_t i = 0; i < ArraySizeBytes(testBuff); ++i)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(p[i]) << " ";
  }
  std::cout << std::endl;

  constexpr size_t kByteSize = kBitSize / kBitsPerByte;
  using TBitBuffer           = TBitBuffer<kBitSize, TBlockType, kBlockEndian>;
  return TBitBuffer{testBuff[0], testBuff[1], testBuff[2], testBuff[3], testBuff[4]};
}

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_ConstGet()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;

  const auto buffer = CreatePeriodicPattern<TBlockType, kBlockEndian>();

  static_assert(std::is_same_v<decltype(buffer[0]), bool>);
  static_assert(std::is_same_v<decltype(buffer.Test(0)), bool>);

  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    const bool expected = TestPattern<buffer.kBlockBits>(i);

    EXPECT_EQ(buffer[i], expected) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), expected) << "Failed for global bit index: " << i;
  }
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_ConstGet)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_ConstGet<TBlockType, Endian::Little>();
  TBitBuffer_ConstGet<TBlockType, Endian::Big>();
}

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_Get()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;

  auto buffer = CreatePeriodicPattern<TBlockType, kBlockEndian>();

  static_assert(std::is_same_v<decltype(buffer[0]), TBitRef<TBlockType>>);
  static_assert(std::is_same_v<decltype(buffer.Test(0)), bool>);

  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    const bool expected = TestPattern<buffer.kBlockBits>(i);

    EXPECT_EQ(buffer[i], expected) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), expected) << "Failed for global bit index: " << i;
  }
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_Get)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_Get<TBlockType, Endian::Little>();
  TBitBuffer_Get<TBlockType, Endian::Big>();
}

// TODO: Implement rest of the tests
