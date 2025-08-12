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
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  using BitIndexMapper          = TEndianBitIndexMapper<kBlockSize, kBlockEndian>;

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

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_Set()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;
  constexpr size_t kBlockSize   = sizeof(TBlockType);
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  using TBitBuffer              = TBitBuffer<kBlockBits * 5, TBlockType, kBlockEndian>;

  auto buffer = TBitBuffer{TBlockType{}, TBlockType{}, TBlockType{}, TBlockType{}, TBlockType{}};

  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    const bool bitValue = TestPattern<buffer.kBlockBits>(i);
    // Set value
    buffer[i] = bitValue;

    EXPECT_EQ(buffer[i], bitValue) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), bitValue) << "Failed for global bit index: " << i;
  }
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_Set)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_Set<TBlockType, Endian::Little>();
  TBitBuffer_Set<TBlockType, Endian::Big>();
}

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_BatchOperations()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;
  constexpr size_t kBlockSize   = sizeof(TBlockType);
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  using TBitBuffer              = TBitBuffer<kBlockBits * 5, TBlockType, kBlockEndian>;

  auto buffer = TBitBuffer{TBlockType{}, TBlockType{}, TBlockType{}, TBlockType{}, TBlockType{}};

  buffer.Set();
  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    EXPECT_EQ(buffer[i], 1) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), 1) << "Failed for global bit index: " << i;
  }

  buffer.Reset();
  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    EXPECT_EQ(buffer[i], 0) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), 0) << "Failed for global bit index: " << i;
  }

  buffer.Flip();
  for (size_t i = 0; i < buffer.Size(); ++i)
  {
    EXPECT_EQ(buffer[i], 1) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), 1) << "Failed for global bit index: " << i;
  }
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_BatchOperations)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_BatchOperations<TBlockType, Endian::Little>();
  TBitBuffer_BatchOperations<TBlockType, Endian::Big>();
}

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_BatchTestOperations()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;
  constexpr size_t kBlockSize   = sizeof(TBlockType);
  constexpr size_t kBlockBits   = kBlockSize * kBitsPerByte;
  using TBitBuffer              = TBitBuffer<kBlockBits * 2, TBlockType, kBlockEndian>;

  auto buffer = TBitBuffer{TBlockType{}, TBlockType{}};

  EXPECT_FALSE(buffer.All());
  EXPECT_FALSE(buffer.Any());
  EXPECT_TRUE(buffer.None());

  buffer.Set();
  EXPECT_TRUE(buffer.All());
  EXPECT_TRUE(buffer.Any());
  EXPECT_FALSE(buffer.None());

  buffer.Reset();
  buffer[buffer.Size() / 2] = true;
  EXPECT_FALSE(buffer.All());
  EXPECT_TRUE(buffer.Any());
  EXPECT_FALSE(buffer.None());
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_BatchTestOperations)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_BatchTestOperations<TBlockType, Endian::Little>();
  TBitBuffer_BatchTestOperations<TBlockType, Endian::Big>();
}

template <typename taBlockType, Endian taBlockEndian>
auto TBitBuffer_Iterator()
{
  using TBlockType              = taBlockType;
  constexpr Endian kBlockEndian = taBlockEndian;

  auto buffer      = CreatePeriodicPattern<TBlockType, kBlockEndian>();
  using TBitBuffer = decltype(buffer);

  const auto& constBufferRef = buffer;

  static_assert(std::is_same_v<decltype(buffer.begin()), decltype(buffer.end())>);
  static_assert(std::is_same_v<decltype(buffer.cbegin()), decltype(buffer.cend())>);
  static_assert(std::is_same_v<decltype(constBufferRef.begin()), decltype(constBufferRef.end())>);
  static_assert(std::is_same_v<decltype(constBufferRef.cbegin()), decltype(constBufferRef.cend())>);
  static_assert(std::is_same_v<decltype(constBufferRef.begin()), decltype(buffer.cbegin())>);
  static_assert(std::is_same_v<decltype(constBufferRef.cbegin()), decltype(buffer.cbegin())>);
  static_assert(std::is_same_v<decltype(constBufferRef.end()), decltype(buffer.cend())>);
  static_assert(std::is_same_v<decltype(constBufferRef.cend()), decltype(buffer.cend())>);

  static_assert(std::is_same_v<decltype(buffer.begin()), typename TBitBuffer::TIterator>);
  static_assert(std::is_same_v<decltype(buffer.cbegin()), typename TBitBuffer::TConstIterator>);
  static_assert(
    std::is_same_v<decltype(constBufferRef.begin()), typename TBitBuffer::TConstIterator>);
  static_assert(
    std::is_same_v<decltype(constBufferRef.cbegin()), typename TBitBuffer::TConstIterator>);

  static_assert(std::is_same_v<decltype(constBufferRef.begin().Get()), bool>);
  static_assert(std::is_same_v<decltype(constBufferRef.cbegin().Get()), bool>);
  static_assert(std::is_same_v<decltype(*constBufferRef.begin()), TBitRef<const TBlockType>>);
  static_assert(std::is_same_v<decltype(*constBufferRef.cbegin()), TBitRef<const TBlockType>>);

  auto it      = buffer.begin();
  auto constIt = buffer.cbegin();
  for (size_t i = 0; i < buffer.Size(); ++i, ++it, ++constIt)
  {
    EXPECT_EQ(buffer[i], *it) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), *it) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), it.Get()) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), constIt.Get()) << "Failed for global bit index: " << i;
    EXPECT_EQ(buffer.Test(i), *constIt) << "Failed for global bit index: " << i;
  }
}

TYPED_TEST(BitBufferGroupTest, TBitBuffer_Iterator)
{
  using TBlockType = typename TestFixture::TType;
  TBitBuffer_Iterator<TBlockType, Endian::Little>();
  TBitBuffer_Iterator<TBlockType, Endian::Big>();
}

TEST(BitBufferExampleTest, bit_order_non_native_endian)
{
  using TBlockType              = uint16_t;
  constexpr Endian kBlockEndian = Endian::Native == Endian::Little ? Endian::Big : Endian::Little;
  using TBitBuffer              = TBitBuffer<32, TBlockType, kBlockEndian>;
  const TBitBuffer bitBuffer    = {
    //            big <-- little
    TBlockType{0b0100111100001011},
    TBlockType{0b1111010011010001},
  };
  // Byte 0, bits 8-15 of the block 0
  EXPECT_EQ(bitBuffer[0], true);  // 1
  EXPECT_EQ(bitBuffer[1], true);  // 1
  EXPECT_EQ(bitBuffer[2], true);  // 1
  EXPECT_EQ(bitBuffer[3], true);  // 1
  EXPECT_EQ(bitBuffer[4], false); // 0
  EXPECT_EQ(bitBuffer[5], false); // 0
  EXPECT_EQ(bitBuffer[6], true);  // 1
  EXPECT_EQ(bitBuffer[7], false); // 0

  // Byte 1, bits 0-7 of the block 0
  EXPECT_EQ(bitBuffer[8], true);   // 1
  EXPECT_EQ(bitBuffer[9], true);   // 1
  EXPECT_EQ(bitBuffer[10], false); // 0
  EXPECT_EQ(bitBuffer[11], true);  // 1
  EXPECT_EQ(bitBuffer[12], false); // 0
  EXPECT_EQ(bitBuffer[13], false); // 0
  EXPECT_EQ(bitBuffer[14], false); // 0
  EXPECT_EQ(bitBuffer[15], false); // 0

  // Byte 2, bits 8-15 of the block 1
  EXPECT_EQ(bitBuffer[16], false); // 0
  EXPECT_EQ(bitBuffer[17], false); // 0
  EXPECT_EQ(bitBuffer[18], true);  // 1
  EXPECT_EQ(bitBuffer[19], false); // 0
  EXPECT_EQ(bitBuffer[20], true);  // 1
  EXPECT_EQ(bitBuffer[21], true);  // 1
  EXPECT_EQ(bitBuffer[22], true);  // 1
  EXPECT_EQ(bitBuffer[23], true);  // 1

  // Byte 3, bits 0-7 of the block 1
  EXPECT_EQ(bitBuffer[24], true);  // 1
  EXPECT_EQ(bitBuffer[25], false); // 0
  EXPECT_EQ(bitBuffer[26], false); // 0
  EXPECT_EQ(bitBuffer[27], false); // 0
  EXPECT_EQ(bitBuffer[28], true);  // 1
  EXPECT_EQ(bitBuffer[29], false); // 0
  EXPECT_EQ(bitBuffer[30], true);  // 1
  EXPECT_EQ(bitBuffer[31], true);  // 1
}

TEST(BitBufferExampleTest, bit_order_native_endian)
{
  using TBlockType              = uint16_t;
  constexpr Endian kBlockEndian = Endian::Native;
  using TBitBuffer              = TBitBuffer<32, TBlockType, kBlockEndian>;
  const TBitBuffer bitBuffer    = {
    //            big <-- little
    TBlockType{0b0100111100001011},
    TBlockType{0b1111010011010001},
  };
  // Byte 1, bits 0-7 of the block 0
  EXPECT_EQ(bitBuffer[0], true);  // 1
  EXPECT_EQ(bitBuffer[1], true);  // 1
  EXPECT_EQ(bitBuffer[2], false); // 0
  EXPECT_EQ(bitBuffer[3], true);  // 1
  EXPECT_EQ(bitBuffer[4], false); // 0
  EXPECT_EQ(bitBuffer[5], false); // 0
  EXPECT_EQ(bitBuffer[6], false); // 0
  EXPECT_EQ(bitBuffer[7], false); // 0

  // Byte 0, bits 8-15 of the block 0
  EXPECT_EQ(bitBuffer[8], true);   // 1
  EXPECT_EQ(bitBuffer[9], true);   // 1
  EXPECT_EQ(bitBuffer[10], true);  // 1
  EXPECT_EQ(bitBuffer[11], true);  // 1
  EXPECT_EQ(bitBuffer[12], false); // 0
  EXPECT_EQ(bitBuffer[13], false); // 0
  EXPECT_EQ(bitBuffer[14], true);  // 1
  EXPECT_EQ(bitBuffer[15], false); // 0

  // Byte 3, bits 0-7 of the block 1
  EXPECT_EQ(bitBuffer[16], true);  // 1
  EXPECT_EQ(bitBuffer[17], false); // 0
  EXPECT_EQ(bitBuffer[18], false); // 0
  EXPECT_EQ(bitBuffer[19], false); // 0
  EXPECT_EQ(bitBuffer[20], true);  // 1
  EXPECT_EQ(bitBuffer[21], false); // 0
  EXPECT_EQ(bitBuffer[22], true);  // 1
  EXPECT_EQ(bitBuffer[23], true);  // 1

  // Byte 2, bits 8-15 of the block 1
  EXPECT_EQ(bitBuffer[24], false); // 0
  EXPECT_EQ(bitBuffer[25], false); // 0
  EXPECT_EQ(bitBuffer[26], true);  // 1
  EXPECT_EQ(bitBuffer[27], false); // 0
  EXPECT_EQ(bitBuffer[28], true);  // 1
  EXPECT_EQ(bitBuffer[29], true);  // 1
  EXPECT_EQ(bitBuffer[30], true);  // 1
  EXPECT_EQ(bitBuffer[31], true);  // 1
}

// TODO: Implement rest of the tests
