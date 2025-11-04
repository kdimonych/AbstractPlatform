#include <AbstractPlatform/common/BitOperations.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

namespace {
static_assert(kBitsPerByte == 8);

template <typename taDataType, typename taIndexType, taIndexType... taIndexes>
inline static constexpr std::pair<taDataType, taDataType>
ByteSwapTestPairImpl(std::integer_sequence<taIndexType, taIndexes...>)
{
  using TProxyType = typename SizeCompatible<taDataType>::TType;

  constexpr auto kForwardValue = (SetByte(taDataType{}, taIndexes, taIndexes) | ...);
  constexpr auto kReversedValue =
    (SetByte(taDataType{}, (sizeof(taDataType) - 1) - taIndexes, taIndexes) | ...);

  static_assert(sizeof(taDataType) <= 1 || kForwardValue != kReversedValue,
                "kForwardValue and kReversedValue must be different");
  static_assert(sizeof(taDataType) > 1 || kForwardValue == kReversedValue,
                "kForwardValue and kReversedValue must be equal for 1-byte types");

  return {kForwardValue, kReversedValue};
}

template <typename taDataType>
inline static constexpr std::pair<taDataType, taDataType> ByteSwapTestPair()
{
  return ByteSwapTestPairImpl<taDataType>(std::make_index_sequence<sizeof(taDataType)>{});
}
} // namespace

template <typename T>
struct BitOperationsTest : public testing::Test
{
  using TType = T;
};

using TBitOperationsTestTypes = testing::Types<std::uint8_t,
                                               std::uint16_t,
                                               std::uint32_t,
                                               std::uint64_t,
                                               std::int8_t,
                                               std::int16_t,
                                               std::int32_t,
                                               std::int64_t,
                                               std::size_t>;
TYPED_TEST_SUITE(BitOperationsTest, TBitOperationsTestTypes);

template <typename taDataType, typename taIndexType, taIndexType... taIndexes>
inline static constexpr bool
ByteMaskStaticTestImpl(std::integer_sequence<taIndexType, taIndexes...>)
{
  using TProxyType = typename SizeCompatible<taDataType>::TType;
  // TODO: Test that ByteMask and ByteInverseMask return correct values for all bytes
  return true;
}

template <typename taDataType>
inline static constexpr bool ByteMaskStaticTest()
{
  return ByteMaskStaticTestImpl<taDataType>(std::make_index_sequence<sizeof(taDataType)>{}), true;
}

TYPED_TEST(BitOperationsTest, ByteMask)
{
  using TType      = typename TestFixture::TType;
  using TProxyType = typename SizeCompatible<TType>::TType;

  static_assert(ByteMaskStaticTest<TType>());

  for (size_t byteIndex = 0; byteIndex < sizeof(TType); ++byteIndex)
  {
    SCOPED_TRACE("Testing ByteMask for byte index: " + std::to_string(byteIndex));
    const TType expected = static_cast<TType>(TProxyType{0xff} << (byteIndex * kBitsPerByte));
    const TType expected_inverse = ~expected;

    EXPECT_EQ(ByteMask<TType>(byteIndex), expected);
    EXPECT_EQ(ByteInverseMask<TType>(byteIndex), expected_inverse);
  }
}

template <typename taDataType, typename taIndexType, taIndexType... taIndexes>
inline static constexpr bool
InvertBitsStaticTestImpl(std::integer_sequence<taIndexType, taIndexes...>)
{
  using TProxyType = typename SizeCompatible<taDataType>::TType;
  // TODO: Test that InvertBits return correct values for all bytes
  return true;
}

template <typename taDataType>
inline static constexpr bool InvertBitsStaticTest()
{
  return InvertBitsStaticTestImpl<taDataType>(std::make_index_sequence<sizeof(taDataType)>{}), true;
}

TYPED_TEST(BitOperationsTest, InvertBits)
{
  using TType      = typename TestFixture::TType;
  using TProxyType = typename SizeCompatible<TType>::TType;

  static_assert(InvertBitsStaticTest<TType>());

  const auto       value            = TType{0};
  const TProxyType expected         = static_cast<TProxyType>(value);
  const TType      expected_inverse = static_cast<TType>(~expected);

  EXPECT_EQ(InvertBits<TType>(value), expected_inverse);
}

/******************************** SetByte test  ******************************************/
TYPED_TEST(BitOperationsTest, SetByte)
{
  using TType      = typename TestFixture::TType;
  using TProxyType = typename SizeCompatible<TType>::TType;

  for (size_t byteIndex = 0; byteIndex < sizeof(TType); ++byteIndex)
  {
    {
      const auto  original_value = TType{};
      const auto  byte_value     = 0xff;
      const TType expected =
        static_cast<TType>(TProxyType{byte_value} << (byteIndex * kBitsPerByte));

      EXPECT_EQ(SetByte(original_value, byteIndex, byte_value), expected)
        << "SetByte does not return expected value for original_value: " << std::hex
        << original_value << " byte index: " << std::dec << byteIndex << std::hex
        << " and byte_value:" << byte_value;
    }
    {
      const auto  original_value = std::numeric_limits<TType>::max();
      const auto  byte_value     = 0x11;
      const TType expected       = static_cast<TType>(
        original_value & ByteInverseMask<TProxyType>(byteIndex)
        | (TProxyType{byte_value} << (byteIndex * kBitsPerByte) & ByteMask<TProxyType>(byteIndex)));

      EXPECT_EQ(SetByte(original_value, byteIndex, byte_value), expected)
        << "SetByte does not return expected value for original_value: " << std::hex
        << original_value << " byte index: " << std::dec << byteIndex << std::hex
        << " and byte_value:" << byte_value;
    }
  }
}

template <typename taDataType>
inline static constexpr bool ByteSwap_StaticBitOperationsTest()
{
  using TType                   = taDataType;
  constexpr auto kForwardValue  = ByteSwapTestPair<TType>().first;
  constexpr auto kReversedValue = ByteSwapTestPair<TType>().second;

  static_assert(ByteSwap(kForwardValue) == kReversedValue);
  static_assert(ByteSwap(kReversedValue) == kForwardValue);

  return true;
}

TYPED_TEST(BitOperationsTest, ByteSwap)
{
  using TType = typename TestFixture::TType;

  static_assert(ByteSwap_StaticBitOperationsTest<TType>());

  const auto [kForwardValue, kReversedValue] = ByteSwapTestPair<TType>();
  if (sizeof(TType) > 1)
  {
    ASSERT_NE(kForwardValue, kReversedValue)
      << "kForwardValue and kReversedValue must be different";
  }
  else
  {
    ASSERT_EQ(kForwardValue, kReversedValue)
      << "kForwardValue and kReversedValue must be equal for 1-byte types";
  }

  EXPECT_EQ(ByteSwap(kForwardValue), kReversedValue);
  EXPECT_EQ(ByteSwap(kReversedValue), kForwardValue);
}

TYPED_TEST(BitOperationsTest, EndianConverter)
{
  using TType = typename TestFixture::TType;

  const auto [kForwardValue, kReversedValue] = ByteSwapTestPair<TType>();

  {
    SCOPED_TRACE("The converting between the same endianness must have no effect");
    EXPECT_EQ(EndianConverter<Endian::Native>::Convert(kForwardValue), kForwardValue);
    EXPECT_EQ(EndianConverter<Endian::Native>::Convert(kReversedValue), kReversedValue);
    EXPECT_EQ((EndianConverter<Endian::Native, Endian::Native>::Convert(kForwardValue)),
              kForwardValue);
    EXPECT_EQ((EndianConverter<Endian::Native, Endian::Native>::Convert(kReversedValue)),
              kReversedValue);

    EXPECT_EQ((EndianConverter<Endian::Big, Endian::Big>::Convert(kForwardValue)), kForwardValue);
    EXPECT_EQ((EndianConverter<Endian::Big, Endian::Big>::Convert(kReversedValue)), kReversedValue);
  }

  {
    SCOPED_TRACE("The converting between the different endianness must reverse byte order");
    EXPECT_EQ((EndianConverter<Endian::Big, Endian::Little>::Convert(kForwardValue)),
              kReversedValue);
    EXPECT_EQ((EndianConverter<Endian::Big, Endian::Little>::Convert(kReversedValue)),
              kForwardValue);
    EXPECT_EQ((EndianConverter<Endian::Little, Endian::Big>::Convert(kForwardValue)),
              kReversedValue);
    EXPECT_EQ((EndianConverter<Endian::Little, Endian::Big>::Convert(kReversedValue)),
              kForwardValue);
  }

  // TODO: Make this test depend on a build target platform
  {
    SCOPED_TRACE("The converting from native endianness another must depend on a build target "
                 "platform");
    // For the Little-endian native endinness
    EXPECT_EQ(EndianConverter<Endian::Little>::Convert(kForwardValue), kForwardValue);
    EXPECT_EQ(EndianConverter<Endian::Big>::Convert(kForwardValue), kReversedValue);
  }
}

TYPED_TEST(BitOperationsTest, BitSize)
{
  using TType                   = typename TestFixture::TType;
  constexpr size_t kTypeBitSize = sizeof(TType) * kBitsPerByte;

  static_assert(BitSize(TType{}) == kTypeBitSize);

  EXPECT_EQ(BitSize(TType{}), kTypeBitSize);
}

TEST(BitOperationsTestStatic, BufferSize)
{
  static_assert(BufferSize(0) == 0);
  static_assert(BufferSize(1) == 1);
  static_assert(BufferSize(7) == 1);
  static_assert(BufferSize(8) == 1);
  static_assert(BufferSize(9) == 2);
  static_assert(BufferSize(16) == 2);
  static_assert(BufferSize(17) == 3);
  static_assert(BufferSize(24) == 3);
  static_assert(BufferSize((std::numeric_limits<size_t>::max() - kBitsPerByte) + 1)
                == std::numeric_limits<size_t>::max() / kBitsPerByte);

  EXPECT_EQ(BufferSize(0), 0);
  EXPECT_EQ(BufferSize(1), 1);
  EXPECT_EQ(BufferSize(7), 1);
  EXPECT_EQ(BufferSize(8), 1);
  EXPECT_EQ(BufferSize(9), 2);
  EXPECT_EQ(BufferSize(16), 2);
  EXPECT_EQ(BufferSize(17), 3);
  EXPECT_EQ(BufferSize(24), 3);
  EXPECT_EQ(BufferSize((std::numeric_limits<size_t>::max() - kBitsPerByte) + 1),
            std::numeric_limits<size_t>::max() / kBitsPerByte);
}

/***************************** TEndianBitIndexMapper test ************************************/

template <typename taIndexType, typename taBlockType, Endian taBlockEndian>
auto TEndianBitIndexMapperTest()
{
  using TBlockType = taBlockType;
  using TIndexType = taIndexType;

  constexpr TBlockType kBlockSize = static_cast<TBlockType>(sizeof(TBlockType));
  constexpr TBlockType kBlockBits = kBlockSize * kBitsPerByte;
  using TEndianBitIndexMapper     = TEndianBitIndexMapper<kBlockSize, taBlockEndian>;

  if constexpr (taBlockEndian == Endian::Native)
  {
    // Test behavior for native endianness
    for (TBlockType inBlockIndex = 0; inBlockIndex < kBlockBits; ++inBlockIndex)
    {
      ASSERT_LT(inBlockIndex, kBlockBits) << "Global bit index out of bounds: " << inBlockIndex;

      const auto expectedInBlockIndex = inBlockIndex;

      const auto mappedIndex = TEndianBitIndexMapper::MapIndex(inBlockIndex);
      EXPECT_EQ(mappedIndex, expectedInBlockIndex)
        << "Failed for in block bit index: " << inBlockIndex;

      // Check if the mapping is reversible
      // Map the index back to the original in-block index
      // This is a simple test to ensure that the mapping can be reversed correctly.
      const auto reversibleMappedIndex = TEndianBitIndexMapper::MapIndex(mappedIndex);
      EXPECT_EQ(reversibleMappedIndex, inBlockIndex)
        << "Failed for in block bit index: " << inBlockIndex;
    }
  }
  else
  {
    // Test behavior for non-native endianness
    for (size_t inBlockIndex = 0; inBlockIndex < kBlockBits; ++inBlockIndex)
    {
      ASSERT_LT(inBlockIndex, kBlockBits) << "Global bit index out of bounds: " << inBlockIndex;

      const auto byteIndex   = inBlockIndex / kBitsPerByte;
      const auto inByteIndex = inBlockIndex % kBitsPerByte;

      ASSERT_LT(byteIndex, kBlockSize) << "Global byte index out of bounds: " << byteIndex;

      const auto expectedInBlockIndex = (kBlockSize - 1 - byteIndex) * kBitsPerByte + inByteIndex;

      const auto mappedIndex = TEndianBitIndexMapper::MapIndex(inBlockIndex);
      EXPECT_EQ(mappedIndex, expectedInBlockIndex)
        << "Failed for in block bit index: " << inBlockIndex;

      // Check if the mapping is reversible
      // Map the index back to the original in-block index
      // This is a simple test to ensure that the mapping can be reversed correctly.
      const auto reversibleMappedIndex = TEndianBitIndexMapper::MapIndex(mappedIndex);
      EXPECT_EQ(reversibleMappedIndex, inBlockIndex)
        << "Failed for in block bit index: " << inBlockIndex;
    }
  }
}

TYPED_TEST(BitOperationsTest, TBufferLayout_TEndianBitIndexMapper)
{
  using TBlockType = typename TestFixture::TType;

  TEndianBitIndexMapperTest<std::int64_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::int64_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::uint64_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::uint64_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::int32_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::int32_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::uint32_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::uint32_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::int16_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::int16_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::uint16_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::uint16_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::int8_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::int8_t, TBlockType, Endian::Little>();

  TEndianBitIndexMapperTest<std::uint8_t, TBlockType, Endian::Big>();
  TEndianBitIndexMapperTest<std::uint8_t, TBlockType, Endian::Little>();
}

/******************************* Bit reverse test ************************************/
TEST(BitOperationsTestStatic, BitwiseReverse8)
{
  std::uint8_t value    = 0b00000000;
  std::uint8_t reversed = BitwiseReverse8(value);
  EXPECT_EQ(reversed, 0b00000000);

  value    = 0b11111111;
  reversed = BitwiseReverse8(value);
  EXPECT_EQ(reversed, 0b11111111);

  value    = 0b10101010;
  reversed = BitwiseReverse8(value);
  EXPECT_EQ(reversed, 0b01010101);

  value    = 0b11001100;
  reversed = BitwiseReverse8(value);
  EXPECT_EQ(reversed, 0b00110011);

  value    = 0b11110000;
  reversed = BitwiseReverse8(value);
  EXPECT_EQ(reversed, 0b00001111);

  value                 = 0b00000001;
  std::uint8_t expected = 0b10000000;
  for (; value != 0b10000000; value <<= 1, expected >>= 1)
  {
    reversed = BitwiseReverse8(value);
    EXPECT_EQ(reversed, expected) << "Failed for value: " << std::hex << (int)value;
  }
}

TEST(BitOperationsTestStatic, BitwiseReverse16)
{
  std::uint16_t value    = 0b0000000000000000;
  std::uint16_t reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b0000000000000000);

  value    = 0b1111111111111111;
  reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b1111111111111111);

  value    = 0b1010101010101010;
  reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b0101010101010101);

  value    = 0b1100110011001100;
  reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b0011001100110011);

  value    = 0b1111000011110000;
  reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b0000111100001111);

  value    = 0b1111111100000000;
  reversed = BitwiseReverse16(value);
  EXPECT_EQ(reversed, 0b0000000011111111);

  value                  = 0b0000000000000001;
  std::uint16_t expected = 0b1000000000000000;
  for (; value != 0b1000000000000000; value <<= 1, expected >>= 1)
  {
    reversed = BitwiseReverse16(value);
    EXPECT_EQ(reversed, expected) << "Failed for value: " << std::hex << (int)value;
  }
}

TEST(BitOperationsTestStatic, BitwiseReverse32)
{
  std::uint32_t value    = 0b00000000000000000000000000000000;
  std::uint32_t reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b00000000000000000000000000000000);

  value    = 0b11111111111111111111111111111111;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b11111111111111111111111111111111);

  value    = 0b10101010101010101010101010101010;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b01010101010101010101010101010101);

  value    = 0b11001100110011001100110011001100;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b00110011001100110011001100110011);

  value    = 0b11110000111100001111000011110000;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b00001111000011110000111100001111);

  value    = 0b11111111000000001111111100000000;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b00000000111111110000000011111111);

  value    = 0b11111111111111110000000000000000;
  reversed = BitwiseReverse32(value);
  EXPECT_EQ(reversed, 0b00000000000000001111111111111111);

  value                  = 0b00000000000000000000000000000001;
  std::uint32_t expected = 0b10000000000000000000000000000000;
  for (; value != 0b10000000000000000000000000000000; value <<= 1, expected >>= 1)
  {
    reversed = BitwiseReverse32(value);
    EXPECT_EQ(reversed, expected) << "Failed for value: " << std::hex << (int)value;
  }
}

TEST(BitOperationsTestStatic, BitwiseReverse64)
{
  std::uint64_t value    = 0b0000000000000000000000000000000000000000000000000000000000000000;
  std::uint64_t reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0000000000000000000000000000000000000000000000000000000000000000);

  value    = 0b1111111111111111111111111111111111111111111111111111111111111111;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b1111111111111111111111111111111111111111111111111111111111111111);

  value    = 0b1010101010101010101010101010101010101010101010101010101010101010;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0101010101010101010101010101010101010101010101010101010101010101);

  value    = 0b1100110011001100110011001100110011001100110011001100110011001100;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0011001100110011001100110011001100110011001100110011001100110011);

  value    = 0b1111000011110000111100001111000011110000111100001111000011110000;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0000111100001111000011110000111100001111000011110000111100001111);

  value    = 0b1111111100000000111111110000000011111111000000001111111100000000;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0000000011111111000000001111111100000000111111110000000011111111);

  value    = 0b1111111111111111000000000000000011111111111111110000000000000000;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0000000000000000111111111111111100000000000000001111111111111111);

  value    = 0b1111111111111111111111111111111100000000000000000000000000000000;
  reversed = BitwiseReverse64(value);
  EXPECT_EQ(reversed, 0b0000000000000000000000000000000011111111111111111111111111111111);

  value                  = 0b0000000000000000000000000000000000000000000000000000000000000001;
  std::uint64_t expected = 0b1000000000000000000000000000000000000000000000000000000000000000;
  for (; value != 0b1000000000000000000000000000000000000000000000000000000000000000;
       value <<= 1, expected >>= 1)
  {
    reversed = BitwiseReverse64(value);
    EXPECT_EQ(reversed, expected) << "Failed for value: " << std::hex << (int)value;
  }
}
