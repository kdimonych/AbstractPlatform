#include <AbstractPlatform/output/display/buffer/PixelBufferView.hpp>
#include <AbstractPlatform/output/display/buffer/StaticPixelBuffer.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

using namespace AbstractPlatform;

namespace {
template <typename taPixel>
struct PixelBuildHelper
{
  using TPixel = taPixel;

  inline static constexpr TPixel make(int aValue)
  {
    return static_cast<TPixel>(aValue);
  }
};

template <>
struct PixelBuildHelper<TBitPixel>
{
  using TPixel = TBitPixel;

  inline static constexpr TPixel make(int aValue)
  {
    return TPixel{static_cast<bool>(aValue)};
  }
};

template <>
struct PixelBuildHelper<TRGBPixel>
{
  using TPixel = TRGBPixel;

  inline static constexpr TPixel make(int aValue)
  {
    return TPixel{static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue)};
  }
};

template <>
struct PixelBuildHelper<TRGBAPixel>
{
  using TPixel = TRGBAPixel;

  inline static constexpr TPixel make(int aValue)
  {
    return TPixel{static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue)};
  }
};

template <>
struct PixelBuildHelper<TRGB565Pixel>
{
  using TPixel = TRGB565Pixel;

  inline static constexpr TPixel make(int aValue)
  {
    return TPixel{static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue),
                  static_cast<TPixel::TValue>(aValue)};
  }
};

template <typename taPixel>
inline static constexpr bool operator==(const taPixel& lhs, const int& rhs)
{
  return lhs == PixelBuildHelper<taPixel>::make(rhs);
}

template <typename taPixel>
inline static constexpr bool operator==(const int& lhs, const taPixel& rhs)
{
  return PixelBuildHelper<taPixel>::make(lhs) == rhs;
}

template <typename taPixel>
inline static constexpr bool operator!=(const int& lhs, const taPixel& rhs)
{
  return !operator==(lhs, rhs);
}

template <typename taPixel>
inline static constexpr bool operator!=(const taPixel& lhs, const int& rhs)
{
  return !operator==(lhs, rhs);
}

}; // namespace

using TPixelTypeList =
  testing::Types<TRGBPixel, TRGBAPixel, TRGB565Pixel, std::uint8_t, std::uint16_t, std::uint32_t>;

template <class taPixelBuffer>
struct CommonTest
{
  using TPixelBuffer = taPixelBuffer;

  template <typename taFnPixelBuffer>
  inline static constexpr auto
  SizeTest(taFnPixelBuffer& buffer, size_t expectedWidth, size_t expectedHeight)
  {
    EXPECT_EQ(buffer.Size(), expectedWidth * expectedHeight);
    EXPECT_EQ(buffer.Width(), expectedWidth);
    EXPECT_EQ(buffer.Height(), expectedHeight);
  }

  inline static constexpr auto FillTest(TPixelBuffer& buffer)
  {
    using TPixel                    = typename TPixelBufferTraits<TPixelBuffer>::TPixel;
    const TPixelBuffer& constBuffer = buffer;
    auto                pixel = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

    ASSERT_GT(buffer.Size(), 0);
    ASSERT_GT(buffer.Width(), 0);
    ASSERT_GT(buffer.Height(), 0);

    constexpr auto kShift = 100;

    // Fill with a specific pixels
    for (int w = 0; w < buffer.Width(); ++w)
    {
      for (int h = 0; h < buffer.Height(); ++h)
      {
        TPosition pos{w, h};
        auto      value = pixel(static_cast<int>(w + h * buffer.Width()) + kShift);

        buffer.SetPixel(pos, value);
        EXPECT_EQ(buffer.GetPixel(pos), buffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(buffer.GetPixel(pos), constBuffer.GetPixel(pos));
        EXPECT_EQ(buffer.GetPixel(pos.iX, pos.iY), constBuffer.GetPixel(pos.iX, pos.iY));

        EXPECT_EQ(buffer(pos), constBuffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(buffer(pos.iX, pos.iY), constBuffer.GetPixel(pos.iX, pos.iY));

        EXPECT_EQ(buffer.GetIndex(pos.iX, pos.iY), buffer.GetIndex(pos));
        EXPECT_EQ(constBuffer.GetIndex(pos.iX, pos.iY), constBuffer.GetIndex(pos));
        EXPECT_EQ(constBuffer.GetIndex(pos), buffer.GetIndex(pos));

        EXPECT_EQ(buffer.GetPosition(buffer.GetIndex(pos)), pos);
        EXPECT_EQ(constBuffer.GetPosition(constBuffer.GetIndex(pos)), pos);
        EXPECT_EQ(constBuffer.GetPosition(buffer.GetIndex(pos)), pos);
      }
    }
  }
};

template <class taTypeParam>
struct StaticPixelTest : testing::Test
{
  using TPixel = taTypeParam;

  inline static constexpr TPixel pixel(int aValue)
  {
    return PixelBuildHelper<TPixel>::make(aValue);
  }

  // Support constexpr evaluation
};

template <class taTypeParam>
struct TPixelBufferViewTest : testing::Test
{
  using TPixel = taTypeParam;

  inline static constexpr TPixel pixel(int aValue)
  {
    return PixelBuildHelper<TPixel>::make(aValue);
  }

  // Support constexpr evaluation
};

TYPED_TEST_SUITE(StaticPixelTest, TPixelTypeList);
TYPED_TEST_SUITE(TPixelBufferViewTest, TPixelTypeList);

/*================== Tests ====================*/
template <typename taTPixel, TBufferOrientation taOrientation>
void TStaticPixelBufferDefaultCreatedValueIsZeroTest()
{
  using TPixel = taTPixel;
  auto pixel   = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr TBufferOrientation kOrientation = taOrientation;
  static constexpr size_t             kWidth       = 2;
  static constexpr size_t             kHeight      = 2;

  using PixelBuffer = TStaticPixelBuffer<kWidth, kHeight, TPixel, kOrientation>;

  PixelBuffer buffer{pixel(0), pixel(0), pixel(0), pixel(0)};

  CommonTest<PixelBuffer>::SizeTest(buffer, kWidth, kHeight);
  CommonTest<PixelBuffer>::SizeTest(static_cast<const PixelBuffer&>(buffer), kWidth, kHeight);

  for (const auto& pixelValue : buffer.GetInnerBuffer())
  {
    EXPECT_EQ(pixelValue, pixel(0));
  }
}

TYPED_TEST(StaticPixelTest, DefaultCreatedValueIsZero)
{
  using TPixel = TypeParam;
  TStaticPixelBufferDefaultCreatedValueIsZeroTest<TPixel, TBufferOrientation::Horizontal>();
  TStaticPixelBufferDefaultCreatedValueIsZeroTest<TPixel, TBufferOrientation::Vertical>();
}

template <typename taTPixel, TBufferOrientation taOrientation>
void TPixelBufferViewDefaultCreatedValueIsZeroTest()
{
  using TPixel      = taTPixel;
  using PixelBuffer = TPixelBufferView<TPixel, taOrientation>;
  auto pixel        = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  std::array<TPixel, kWidth * kHeight> rawBuffer{pixel(0), pixel(0), pixel(0), pixel(0)};
  PixelBuffer                          buffer{kWidth, kHeight, rawBuffer.data()};

  CommonTest<PixelBuffer>::SizeTest(buffer, kWidth, kHeight);
  CommonTest<PixelBuffer>::SizeTest(static_cast<const PixelBuffer&>(buffer), kWidth, kHeight);

  for (size_t idx = 0; idx < buffer.Size(); ++idx)
  {
    EXPECT_EQ(buffer.GetInnerBuffer()[idx], pixel(0));
  }
}

TYPED_TEST(TPixelBufferViewTest, DefaultCreatedValueIsZero)
{
  using TPixel = TypeParam;

  TPixelBufferViewDefaultCreatedValueIsZeroTest<TPixel, TBufferOrientation::Horizontal>();
  TPixelBufferViewDefaultCreatedValueIsZeroTest<TPixel, TBufferOrientation::Vertical>();
}

template <typename taTPixel, TBufferOrientation taOrientation>
auto TStaticPixelBufferOrientationTest()
{
  using TPixel = taTPixel;
  auto pixel   = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr TBufferOrientation kOrientation = taOrientation;
  static constexpr size_t             kWidth       = 2;
  static constexpr size_t             kHeight      = 2;
  using PixelBuffer = TStaticPixelBuffer<kWidth, kHeight, TPixel, kOrientation>;

  PixelBuffer buffer{pixel(0), pixel(0), pixel(0), pixel(0)};

  CommonTest<PixelBuffer>::FillTest(buffer);
};

TYPED_TEST(StaticPixelTest, Fill)
{
  using TPixel = TypeParam;

  TStaticPixelBufferOrientationTest<TPixel, TBufferOrientation::Horizontal>();
  TStaticPixelBufferOrientationTest<TPixel, TBufferOrientation::Vertical>();
}

template <typename taTPixel, TBufferOrientation taOrientation>
auto TPixelBufferViewOrientationTest()
{
  using TPixel = taTPixel;
  auto pixel   = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr TBufferOrientation kOrientation = taOrientation;
  static constexpr size_t             kWidth       = 2;
  static constexpr size_t             kHeight      = 2;

  using PixelBuffer = TPixelBufferView<TPixel, kOrientation>;

  std::array<TPixel, kWidth * kHeight> rawBuffer{pixel(0), pixel(0), pixel(0), pixel(0)};
  PixelBuffer                          buffer{kWidth, kHeight, rawBuffer.data()};

  CommonTest<PixelBuffer>::FillTest(buffer);
};

TYPED_TEST(TPixelBufferViewTest, Fill)
{
  using TPixel = TypeParam;

  TPixelBufferViewOrientationTest<TPixel, TBufferOrientation::Horizontal>();
  TPixelBufferViewOrientationTest<TPixel, TBufferOrientation::Vertical>();
}

// TODO: implement rest of the tests
