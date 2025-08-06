#include <AbstractPlatform/common/BufferTraits.hpp>

#include <gtest/gtest.h>

#include <iterator>
#include <type_traits>

using namespace AbstractPlatform;

namespace {
}

template <typename T>
struct BufferTraitsGroupTest : public testing::Test
{
  using TType = T;
};

using TBBufferTraitsGroupTestTypes =
  testing::Types<bool, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

TYPED_TEST_SUITE(BufferTraitsGroupTest, TBBufferTraitsGroupTestTypes);

TYPED_TEST(BufferTraitsGroupTest, raw_array)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = TType[kSize];
  using TBufferTraits           = TBufferTraits<TBuffer>;

  static constexpr TBuffer constexpr_buffer = {TType{}, TType{}, TType{}, TType{}};
  TBuffer                  buffer           = {TType{}, TType{}, TType{}, TType{}};
  const TBuffer&           const_buffer_ref = buffer;

  using TExpectedIterator      = TType*;
  using TExpectedConstIterator = decltype(std::cbegin(constexpr_buffer));

  static_assert(TBufferTraits::Size(constexpr_buffer) == kSize, "Size should match kSize");

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(constexpr_buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(constexpr_buffer), std::cbegin(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::begin(constexpr_buffer), std::begin(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::cend(constexpr_buffer), std::cend(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::end(constexpr_buffer), std::cend(constexpr_buffer));

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), std::cbegin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), std::begin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), std::cend(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), std::cend(const_buffer_ref));

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), std::cbegin(buffer));
  EXPECT_EQ(TBufferTraits::begin(buffer), std::begin(buffer));
  EXPECT_EQ(TBufferTraits::cend(buffer), std::cend(buffer));
  EXPECT_EQ(TBufferTraits::end(buffer), std::cend(buffer));
}

TYPED_TEST(BufferTraitsGroupTest, raw_two_dimensional_array)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = TType[kSize][kSize];
  using TBufferTraits           = TBufferTraits<TBuffer>;

  static constexpr TBuffer constexpr_buffer = {{TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}}};
  TBuffer                  buffer           = {{TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}},
                                               {TType{}, TType{}, TType{}, TType{}}};
  const TBuffer&           const_buffer_ref = buffer;

  using TExpectedIterator      = TType*;
  using TExpectedConstIterator = const TType*;

  static_assert(TBufferTraits::Size(constexpr_buffer) == kSize * kSize, "Size should match kSize");

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(constexpr_buffer), kSize * kSize);
  EXPECT_EQ(TBufferTraits::cbegin(constexpr_buffer), std::cbegin(*std::cbegin(constexpr_buffer)));
  EXPECT_EQ(TBufferTraits::begin(constexpr_buffer), std::begin(*std::begin(constexpr_buffer)));
  EXPECT_EQ(TBufferTraits::cend(constexpr_buffer),
            std::cbegin(*std::cbegin(constexpr_buffer)) + (kSize * kSize));
  EXPECT_EQ(TBufferTraits::end(constexpr_buffer),
            std::begin(*std::begin(constexpr_buffer)) + (kSize * kSize));

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize * kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), std::cbegin(*std::cbegin(const_buffer_ref)));
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), std::begin(*std::begin(const_buffer_ref)));
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref),
            std::cbegin(*std::cbegin(const_buffer_ref)) + (kSize * kSize));
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref),
            std::begin(*std::begin(const_buffer_ref)) + (kSize * kSize));

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize * kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), std::cbegin(*std::cbegin(buffer)));
  EXPECT_EQ(TBufferTraits::begin(buffer), std::begin(*std::begin(buffer)));
  EXPECT_EQ(TBufferTraits::cend(buffer), std::cbegin(*std::cbegin(buffer)) + (kSize * kSize));
  EXPECT_EQ(TBufferTraits::end(buffer), std::begin(*std::begin(buffer)) + (kSize * kSize));
}

TYPED_TEST(BufferTraitsGroupTest, array)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = std::array<TType, kSize>;
  using TBufferTraits           = TBufferTraits<TBuffer>;

  static constexpr TBuffer constexpr_buffer = {{TType{}, TType{}, TType{}, TType{}}};
  TBuffer                  buffer           = {{TType{}, TType{}, TType{}, TType{}}};
  const TBuffer&           const_buffer_ref = buffer;

  using TExpectedIterator      = typename TBuffer::iterator;
  using TExpectedConstIterator = typename TBuffer::const_iterator;

  static_assert(TBufferTraits::Size(constexpr_buffer) == kSize, "Size should match kSize");

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(constexpr_buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(constexpr_buffer), std::cbegin(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::begin(constexpr_buffer), std::begin(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::cend(constexpr_buffer), std::cend(constexpr_buffer));
  EXPECT_EQ(TBufferTraits::end(constexpr_buffer), std::end(constexpr_buffer));

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), std::cbegin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), std::begin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), std::cend(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), std::end(const_buffer_ref));

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), std::cbegin(buffer));
  EXPECT_EQ(TBufferTraits::begin(buffer), std::begin(buffer));
  EXPECT_EQ(TBufferTraits::cend(buffer), std::cend(buffer));
  EXPECT_EQ(TBufferTraits::end(buffer), std::end(buffer));
}

TYPED_TEST(BufferTraitsGroupTest, vector)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = std::vector<TType>;
  using TBufferTraits           = TBufferTraits<TBuffer>;

  TBuffer        buffer           = {{TType{}, TType{}, TType{}, TType{}}};
  const TBuffer& const_buffer_ref = buffer;

  using TExpectedIterator      = typename TBuffer::iterator;
  using TExpectedConstIterator = typename TBuffer::const_iterator;

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), std::cbegin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), std::begin(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), std::cend(const_buffer_ref));
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), std::end(const_buffer_ref));

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), std::cbegin(buffer));
  EXPECT_EQ(TBufferTraits::begin(buffer), std::begin(buffer));
  EXPECT_EQ(TBufferTraits::cend(buffer), std::cend(buffer));
  EXPECT_EQ(TBufferTraits::end(buffer), std::end(buffer));
}
