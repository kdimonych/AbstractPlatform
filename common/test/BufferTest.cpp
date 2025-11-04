#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/common/Buffer.hpp>

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
struct BufferGroupTest : public testing::Test
{
  using TType = T;
};

using TBBufferGroupTestTypes =
  testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

TYPED_TEST_SUITE(BufferGroupTest, TBBufferGroupTestTypes);

template <typename taValueTypeType, size_t taIndex>
inline static constexpr bool BufferTest()
{
  return true;
}

template <typename taTValueType, size_t taAlignment>
auto CheckBufferAlignment(const TRBuffer<taTValueType, taAlignment>& buffer)
{
  return AbstractPlatform::IsAligned(buffer.GetBuffer(), buffer.kAlignment);
};

template <typename taTValueType, size_t taAlignment>
auto CheckSize(const TRBuffer<taTValueType, taAlignment>& buffer, size_t expectedSize)
{
  return buffer.Size() == expectedSize;
};

template <typename taTValueType, size_t taAlignment>
auto CheckRBuffer(const TRBuffer<taTValueType, taAlignment>& buffer, size_t expectedSize)
{
  static_assert(std::is_same_v<decltype(buffer.GetBuffer()), const taTValueType*>,
                "GetBuffer should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.cbegin()), const taTValueType*>,
                "cbegin should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.cend()), const taTValueType*>,
                "cend should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.begin()), const taTValueType*>,
                "begin should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.end()), const taTValueType*>,
                "end should return a const pointer");

  CheckBufferAlignment(buffer);
  CheckSize(buffer, expectedSize);

  EXPECT_EQ(buffer.GetBuffer(), buffer.begin());
  EXPECT_EQ(buffer.GetBuffer() + buffer.Size(), buffer.end());
  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), expectedSize);
  EXPECT_EQ(buffer.begin(), buffer.cbegin());
  EXPECT_EQ(buffer.end(), buffer.cend());
};

template <typename taTValueType, size_t taAlignment>
auto CheckRWBuffer(TRWBuffer<taTValueType, taAlignment>& buffer, size_t expectedSize)
{
  static_assert(std::is_same_v<decltype(buffer.GetBuffer()), taTValueType*>,
                "GetBuffer should return a non-const pointer");
  static_assert(std::is_same_v<decltype(buffer.cbegin()), const taTValueType*>,
                "cbegin should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.cend()), const taTValueType*>,
                "cend should return a const pointer");
  static_assert(std::is_same_v<decltype(buffer.begin()), taTValueType*>,
                "begin should return a non-const pointer");
  static_assert(std::is_same_v<decltype(buffer.end()), taTValueType*>,
                "end should return a non-const pointer");

  CheckBufferAlignment(buffer);
  CheckSize(buffer, expectedSize);

  EXPECT_EQ(buffer.GetBuffer(), buffer.begin());
  EXPECT_EQ(buffer.GetBuffer() + buffer.Size(), buffer.end());
  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), expectedSize);
  EXPECT_EQ(buffer.begin(), buffer.cbegin());
  EXPECT_EQ(buffer.end(), buffer.cend());
};

template <typename taTValueType, size_t taAlignment>
auto CheckRBufferContentDefault(TRBuffer<taTValueType, taAlignment>& buffer,
                                size_t                               expectedSize,
                                const taTValueType&                  expectedDefaultValue)
{
  ASSERT_EQ(buffer.Size(), expectedSize);
  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), expectedSize);
  size_t count = 0;
  for (const auto& item : buffer)
  {
    EXPECT_EQ(item, expectedDefaultValue);
    ++count;
  }
  EXPECT_EQ(count, expectedSize);
};

template <typename taTValueType, size_t taAlignment, typename taExpectation>
auto CheckRBufferContent(TRBuffer<taTValueType, taAlignment>& buffer,
                         const taExpectation                  expectedContent,
                         size_t                               expectedSize)
{
  auto it = buffer.begin();
  ASSERT_EQ(buffer.Size(), expectedSize);
  EXPECT_EQ(std::distance(it, buffer.end()), expectedSize);
  for (const auto& item : expectedContent)
  {
    auto expectedValue = item;
    EXPECT_EQ(*it, expectedValue);
    ++it;
  }
  EXPECT_EQ(it, buffer.end());
};

template <typename taTValueType, size_t taAlignment, typename taExpectation>
auto CheckRWBufferContent(TRWBuffer<taTValueType, taAlignment>& buffer,
                          const taExpectation                   newContent,
                          size_t                                expectedSize)
{
  auto it = buffer.begin();
  ASSERT_EQ(buffer.Size(), expectedSize);
  EXPECT_EQ(std::distance(it, buffer.end()), expectedSize);
  for (const auto& newItem : newContent)
  {
    *it++ = newItem; // Modify the buffer content
  }
  ASSERT_EQ(it, buffer.end());

  it = buffer.begin();
  for (const auto& newItem : newContent)
  {
    EXPECT_EQ(*it++, newItem);
  }
};

/* ============== TStackBuffer ============= */
TYPED_TEST(BufferGroupTest, TStackBuffer_defaultConstructor)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t    kSize = 10;
  TStackBuffer<TType, kSize> buffer;

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContentDefault(buffer, kSize, TType{});
}

TYPED_TEST(BufferGroupTest, TStackBuffer_fromInitializationListConstructor)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t kSize = 4;
  TType                   val{};
  auto                    expectedArray = std::array<TType, kSize>{val, ++val, ++val, ++val};

  TStackBuffer<TType, kSize> buffer{expectedArray[0],
                                    expectedArray[1],
                                    expectedArray[2],
                                    expectedArray[3]};

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContent(buffer, expectedArray, kSize);
}

TYPED_TEST(BufferGroupTest, TStackBuffer_fromInitFunctionConstructor)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t kSize = 4;
  TType                   val{};

  auto expectedArray = std::array<TType, kSize>{val, ++val, ++val, ++val};

  auto   it           = expectedArray.begin();
  auto   endIt        = expectedArray.end();
  size_t count        = 0;
  auto   initFunction = [&count, &it, endIt]() -> TType {
    EXPECT_NE(it, endIt) << "Initialization function called more times than expected";
    if (it == endIt)
    {
      return TType{}; // Return default value if iterator exceeds expected size
    }
    ++count;
    return *it++;
  };

  TStackBuffer<TType, kSize> buffer{initFunction};
  EXPECT_EQ(count, kSize);

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContent(buffer, expectedArray, kSize);
}

TYPED_TEST(BufferGroupTest, TStackBuffer_modification)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t kSize = 4;
  TType                   val{};
  auto                    initialArray = std::array<TType, kSize>{val, ++val, ++val, ++val};
  auto                    expectedArray =
    std::array<TType, kSize>{static_cast<TType>(initialArray[0] + initialArray[0]),
                             static_cast<TType>(initialArray[1] + initialArray[1]),
                             static_cast<TType>(initialArray[2] + initialArray[2]),
                             static_cast<TType>(initialArray[3] + initialArray[3])};

  TStackBuffer<TType, kSize> buffer{initialArray[0],
                                    initialArray[1],
                                    initialArray[2],
                                    initialArray[3]};

  CheckRBufferContent(buffer, initialArray, kSize);
  CheckRWBufferContent(buffer, expectedArray, kSize);
}

TYPED_TEST(BufferGroupTest, TStackBuffer_traits)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = TStackBuffer<TType, kSize>;
  using TBufferTraits           = TBufferTraits<TBuffer>;

  TBuffer        buffer           = {TType{}, TType{}, TType{}, TType{}};
  const TBuffer& const_buffer_ref = buffer;

  using TExpectedIterator      = typename TBuffer::TIterator;
  using TExpectedConstIterator = typename TBuffer::TConstIterator;

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), const_buffer_ref.cbegin());
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), const_buffer_ref.begin());
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), const_buffer_ref.cend());
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), const_buffer_ref.end());

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), buffer.cbegin());
  EXPECT_EQ(TBufferTraits::begin(buffer), buffer.begin());
  EXPECT_EQ(TBufferTraits::cend(buffer), buffer.cend());
  EXPECT_EQ(TBufferTraits::end(buffer), buffer.end());
}

/* ============== TBufferView ============= */
TYPED_TEST(BufferGroupTest, TBufferView_constructor)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t   kSize = 4;
  std::array<TType, kSize>  srcBuffer{TType{}, TType{}, TType{}, TType{}};
  TBufferView<TType, kSize> buffer{srcBuffer.data(), kSize};

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContentDefault(buffer, kSize, TType{});
}

TYPED_TEST(BufferGroupTest, TBufferView_copy_of_const)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t         kSize = 4;
  const std::array<TType, kSize>  srcBuffer{TType{1}, TType{2}, TType{3}, TType{4}};
  TBufferView<const TType, kSize> buffer{srcBuffer.data(), kSize};

  auto copyBuffer = buffer; // Copy constructor

  CheckRBuffer(copyBuffer, kSize);

  CheckRBufferContent(copyBuffer, srcBuffer, kSize);
}

TYPED_TEST(BufferGroupTest, TBufferView_move_const)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t         kSize = 4;
  const std::array<TType, kSize>  srcBuffer{TType{1}, TType{2}, TType{3}, TType{4}};
  TBufferView<const TType, kSize> buffer{srcBuffer.data(), kSize};

  auto movedBuffer = std::move(buffer); // Move constructor

  CheckRBuffer(movedBuffer, kSize);

  CheckRBufferContent(movedBuffer, srcBuffer, kSize);
}

TYPED_TEST(BufferGroupTest, TBufferView_move)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t   kSize = 4;
  std::array<TType, kSize>  srcBuffer{TType{1}, TType{2}, TType{3}, TType{4}};
  TBufferView<TType, kSize> buffer{srcBuffer.data(), kSize};

  auto movedBuffer = std::move(buffer); // Move constructor

  CheckRBuffer(movedBuffer, kSize);

  CheckRBufferContent(movedBuffer, srcBuffer, kSize);
}

TYPED_TEST(BufferGroupTest, TBufferView_modification)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t        kSize = 4;
  const std::array<TType, kSize> originBuffer{TType{1}, TType{2}, TType{3}, TType{4}};
  std::array<TType, kSize>       srcBuffer{originBuffer[0],
                                     originBuffer[1],
                                     originBuffer[2],
                                     originBuffer[3]};
  TBufferView<TType, kSize>      buffer{srcBuffer.data(), kSize};

  CheckRBufferContent(buffer, originBuffer, kSize);

  // Modify the buffer content
  for (auto& item : buffer)
  {
    item = static_cast<TType>(item + item);
  }

  CheckRWBufferContent(buffer, srcBuffer, kSize);

  // Prove that data changed
  for (size_t i = 0; i < kSize; ++i)
  {
    EXPECT_EQ(buffer[i], static_cast<TType>(originBuffer[i] + originBuffer[i]));
  }
}

TYPED_TEST(BufferGroupTest, TBufferView_traits)
{
  using TType                   = typename TestFixture::TType;
  static constexpr size_t kSize = 4;
  using TBuffer                 = TBufferView<TType, kSize>;
  using TBufferTraits           = TBufferTraits<TBuffer>;

  std::array<TType, kSize> srcBuffer{TType{}, TType{}, TType{}, TType{}};

  TBuffer        buffer           = {srcBuffer.data(), kSize};
  const TBuffer& const_buffer_ref = buffer;

  using TExpectedIterator      = typename TBuffer::TIterator;
  using TExpectedConstIterator = typename TBuffer::TConstIterator;

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), const_buffer_ref.cbegin());
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), const_buffer_ref.begin());
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), const_buffer_ref.cend());
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), const_buffer_ref.end());

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), buffer.cbegin());
  EXPECT_EQ(TBufferTraits::begin(buffer), buffer.begin());
  EXPECT_EQ(TBufferTraits::cend(buffer), buffer.cend());
  EXPECT_EQ(TBufferTraits::end(buffer), buffer.end());
}

/* ============== THeapBuffer ============= */
TYPED_TEST(BufferGroupTest, THeapBuffer_constructor)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t kSize = 10;
  THeapBuffer<TType>      buffer(kSize);

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContentDefault(buffer, kSize, TType{});
}

TYPED_TEST(BufferGroupTest, THeapBuffer_constructor_aligned)
{
  using TType = typename TestFixture::TType;

  static constexpr size_t                              kSize = 10;
  THeapBuffer<TType, AbstractPlatform::kWordAlignment> buffer(kSize);

  EXPECT_EQ(buffer.kAlignment, AbstractPlatform::kWordAlignment);
  EXPECT_TRUE(AbstractPlatform::IsAligned(buffer.GetBuffer(), AbstractPlatform::kWordAlignment));

  CheckRBuffer(buffer, kSize);
  CheckRWBuffer(buffer, kSize);

  CheckRBufferContentDefault(buffer, kSize, TType{});
}

TYPED_TEST(BufferGroupTest, THeapBuffer_traits)
{
  using TType         = typename TestFixture::TType;
  using TBuffer       = THeapBuffer<TType, AbstractPlatform::kWordAlignment>;
  using TBufferTraits = TBufferTraits<TBuffer>;

  static constexpr size_t kSize = 4;

  TBuffer        buffer(kSize);
  const TBuffer& const_buffer_ref = buffer;

  using TExpectedIterator      = typename TBuffer::TIterator;
  using TExpectedConstIterator = typename TBuffer::TConstIterator;

  static_assert(std::is_same<typename TBufferTraits::TIterator, TExpectedIterator>::value,
                "TIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TConstIterator, TExpectedConstIterator>::value,
                "TConstIterator should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TValueType, TType>::value,
                "TValueType should match expected type");
  static_assert(std::is_same<typename TBufferTraits::TBuffer, TBuffer>::value,
                "TBuffer should match expected type");

  EXPECT_EQ(TBufferTraits::Size(const_buffer_ref), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(const_buffer_ref), const_buffer_ref.cbegin());
  EXPECT_EQ(TBufferTraits::begin(const_buffer_ref), const_buffer_ref.begin());
  EXPECT_EQ(TBufferTraits::cend(const_buffer_ref), const_buffer_ref.cend());
  EXPECT_EQ(TBufferTraits::end(const_buffer_ref), const_buffer_ref.end());

  EXPECT_EQ(TBufferTraits::Size(buffer), kSize);
  EXPECT_EQ(TBufferTraits::cbegin(buffer), buffer.cbegin());
  EXPECT_EQ(TBufferTraits::begin(buffer), buffer.begin());
  EXPECT_EQ(TBufferTraits::cend(buffer), buffer.cend());
  EXPECT_EQ(TBufferTraits::end(buffer), buffer.end());
}
