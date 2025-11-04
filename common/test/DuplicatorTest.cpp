#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/common/Duplicator.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

template <typename T>
struct DuplicatorPODTypesTest : public testing::Test
{
  using TType = T;
};

struct PodStruct
{
  int   a;
  float b;
  char  c;
};

inline static constexpr bool operator==(const PodStruct& aA, const PodStruct& aB)
{
  return aA.a == aB.a && aA.b == aB.b && aA.c == aB.c;
}

inline static constexpr bool operator!=(const PodStruct& aA, const PodStruct& aB)
{
  return !operator==(aA, aB);
}

using TDuplicatorGroupTestTypes =
  testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, PodStruct>;

TYPED_TEST_SUITE(DuplicatorPODTypesTest, TDuplicatorGroupTestTypes);

/*================= MemoryDuplicator Mock =================*/

struct CpuMemCopyMock
{
  MOCK_METHOD(void, CopyMemory, (const void* src, void* dst, size_t size), (const));
};

template <>
struct AbstractPlatform::MemoryDuplicator<CpuMemCopyMock>
{
  CpuMemCopyMock* mock;

  MemoryDuplicator(CpuMemCopyMock* m)
    : mock(m)
  {
  }

  template <typename taObject>
  inline void CopyMemory(const taObject* aSrcObject,
                         taObject*       aDstObject,
                         size_t          aSize = sizeof(taObject)) const NOEXCEPT
  {
    if (mock)
    {
      mock->CopyMemory(aSrcObject, aDstObject, aSize);
    }
    else
    {
      MemoryDuplicator<CpuMemCopy>().CopyMemory(aSrcObject, aDstObject, aSize);
    }
  }
};

/*================= CreateHelper =================*/
template <typename taT>
struct CreateHelper
{
  inline static constexpr taT Create(int initValue)
  {
    return static_cast<taT>(initValue);
  }
};

template <>
struct CreateHelper<PodStruct>
{
  inline static constexpr PodStruct Create(int initValue)
  {
    return {static_cast<int>(initValue),
            static_cast<float>(initValue),
            static_cast<char>(initValue)};
  }
};

template <typename taT, size_t taSize>
struct CreateHelper<std::array<taT, taSize>>
{
  inline static constexpr std::array<taT, taSize> Create(int initValue)
  {
    std::array<taT, taSize> arr{};
    for (size_t i = 0; i < taSize; ++i)
    {
      arr[i] = CreateHelper<taT>::Create(initValue);
    }
    return arr;
  }
};

template <typename taType>
struct BigObject
{
  taType data[kBulkCopyThreshold / sizeof(taType) + 1];
};

template <typename taType>
inline static constexpr bool operator==(const BigObject<taType>& aA, const BigObject<taType>& aB)
{
  return std::equal(std::begin(aA.data), std::end(aA.data), std::begin(aB.data));
}

template <typename taType>
inline static constexpr bool operator!=(const BigObject<taType>& aA, const BigObject<taType>& aB)
{
  return !operator==(aA, aB);
}

template <typename taType>
struct CreateHelper<BigObject<taType>>
{
  inline static BigObject<taType> Create(int initValue)
  {
    BigObject<taType> obj;
    for (auto& element : obj.data)
    {
      element = CreateHelper<taType>::Create(initValue);
    }
    return obj;
  }
};

/*================= Tests =================*/

TYPED_TEST(DuplicatorPODTypesTest, SmallObject)
{
  using TType = typename TestFixture::TType;
  static_assert(std::is_pod<TType>::value, "TType must be POD for this test");

  TType sourceObject      = CreateHelper<TType>::Create(42);
  TType destinationObject = CreateHelper<TType>::Create(0);

  EXPECT_NE(sourceObject, destinationObject);

  CpuMemCopyMock                   mock;
  MemoryDuplicator<CpuMemCopyMock> memoryDuplicator(&mock);

  EXPECT_CALL(mock, CopyMemory(::testing::_, ::testing::_, sizeof(TType))).Times(0);

  Clone(memoryDuplicator, sourceObject, destinationObject);

  EXPECT_EQ(sourceObject, destinationObject);
}

TYPED_TEST(DuplicatorPODTypesTest, BigObject)
{
  using TType          = typename TestFixture::TType;
  using TTestValueType = BigObject<TType>;
  static_assert(std::is_pod<TTestValueType>::value, "BigObject<TType> must be POD for this test");

  auto sourceObject      = CreateHelper<TTestValueType>::Create(42);
  auto destinationObject = CreateHelper<TTestValueType>::Create(0);

  EXPECT_NE(sourceObject, destinationObject);

  CpuMemCopyMock                   mock;
  MemoryDuplicator<CpuMemCopyMock> memoryDuplicator(&mock);

  EXPECT_CALL(mock, CopyMemory(::testing::_, ::testing::_, sizeof(TTestValueType)))
    .Times(1)
    .WillRepeatedly(::testing::Invoke(
      [](const void* src, void* dst, size_t size) { std::memcpy(dst, src, size); }));

  Clone(memoryDuplicator, sourceObject, destinationObject);

  EXPECT_EQ(sourceObject, destinationObject);
}

TYPED_TEST(DuplicatorPODTypesTest, Array)
{
  using TType          = typename TestFixture::TType;
  using TTestValueType = std::array<TType, kBulkCopyThreshold / sizeof(TType) + 1>;

  auto sourceObject      = CreateHelper<TTestValueType>::Create(42);
  auto destinationObject = CreateHelper<TTestValueType>::Create(0);

  EXPECT_NE(sourceObject, destinationObject);

  CpuMemCopyMock                   mock;
  MemoryDuplicator<CpuMemCopyMock> memoryDuplicator(&mock);

  EXPECT_CALL(mock, CopyMemory(::testing::_, ::testing::_, sizeof(TTestValueType)))
    .Times(1)
    .WillRepeatedly(::testing::Invoke(
      [](const void* src, void* dst, size_t size) { std::memcpy(dst, src, size); }));

  Clone(memoryDuplicator, sourceObject, destinationObject);

  EXPECT_EQ(sourceObject, destinationObject);
}

struct NonPodSmallObject
{
  NonPodSmallObject()
    : value(0)
  {
  }

  NonPodSmallObject(int v)
    : value(v)
  {
  }

  bool operator==(const NonPodSmallObject& other) const
  {
    return value == other.value;
  }

  bool operator!=(const NonPodSmallObject& other) const
  {
    return !(*this == other);
  }

  int value;
};

struct NonPodBigObject
{
  NonPodBigObject()
  {
    for (size_t i = 0; i < ArrayLength(data); ++i)
    {
      data[i] = 0;
    }
  }

  NonPodBigObject(int v)
  {
    for (size_t i = 0; i < ArrayLength(data); ++i)
    {
      data[i] = v;
    }
  }

  bool operator==(const NonPodBigObject& other) const
  {
    return std::equal(std::begin(data), std::end(data), std::begin(other.data));
  }

  bool operator!=(const NonPodBigObject& other) const
  {
    return !operator==(other);
  }

  int data[kBulkCopyThreshold / sizeof(int) + 1];
};

TEST(DuplicatorNonPODTypesTest, SmallObject)
{
  using TType = NonPodSmallObject;
  static_assert(!std::is_pod<TType>::value, "TType must be non-POD for this test");

  TType sourceObject      = CreateHelper<TType>::Create(42);
  TType destinationObject = CreateHelper<TType>::Create(0);

  EXPECT_NE(sourceObject, destinationObject);

  CpuMemCopyMock                   mock;
  MemoryDuplicator<CpuMemCopyMock> memoryDuplicator(&mock);

  EXPECT_CALL(mock, CopyMemory(::testing::_, ::testing::_, sizeof(TType))).Times(0);

  Clone(memoryDuplicator, sourceObject, destinationObject);

  EXPECT_EQ(sourceObject, destinationObject);
}

TEST(DuplicatorNonPODTypesTest, BigObject)
{
  using TType = NonPodBigObject;
  static_assert(!std::is_pod<TType>::value, "TType must be non-POD for this test");

  auto sourceObject      = CreateHelper<TType>::Create(42);
  auto destinationObject = CreateHelper<TType>::Create(0);

  EXPECT_NE(sourceObject, destinationObject);

  CpuMemCopyMock                   mock;
  MemoryDuplicator<CpuMemCopyMock> memoryDuplicator(&mock);

  // The memory duplicator is not expected to be called for non-POD types
  EXPECT_CALL(mock, CopyMemory(::testing::_, ::testing::_, sizeof(TType))).Times(0);

  Clone(memoryDuplicator, sourceObject, destinationObject);

  EXPECT_EQ(sourceObject, destinationObject);
}
