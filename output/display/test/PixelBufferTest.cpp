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

  inline static constexpr auto
  SizeTest(const TPixelBuffer& buffer, size_t expectedWidth, size_t expectedHeight)
  {
    EXPECT_EQ(buffer.Size(), expectedWidth * expectedHeight);
    EXPECT_EQ(buffer.Width(), expectedWidth);
    EXPECT_EQ(buffer.Height(), expectedHeight);

    TPosition startPos{0, 0};
    EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), buffer.Size());
    EXPECT_EQ(buffer.StartFrom(startPos), buffer.begin());
    EXPECT_EQ(buffer.StartFrom(startPos.iX, startPos.iY), buffer.begin());
    EXPECT_EQ(buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1), std::prev(buffer.end()));
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos), buffer.end()), buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos.iX, startPos.iY), buffer.end()),
              buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos),
                            buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1))
                + 1,
              buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos.iX, startPos.iY),
                            buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1))
                + 1,
              buffer.Size());
  }

  inline static constexpr auto
  SizeTest(TPixelBuffer& buffer, size_t expectedWidth, size_t expectedHeight)
  {
    EXPECT_EQ(buffer.Size(), expectedWidth * expectedHeight);
    EXPECT_EQ(buffer.Width(), expectedWidth);
    EXPECT_EQ(buffer.Height(), expectedHeight);

    TPosition startPos{0, 0};
    EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), buffer.Size());
    EXPECT_EQ(buffer.StartFrom(startPos), buffer.begin());
    EXPECT_EQ(buffer.StartFrom(startPos.iX, startPos.iY), buffer.begin());
    EXPECT_EQ(buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1), std::prev(buffer.end()));
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos), buffer.end()), buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos.iX, startPos.iY), buffer.end()),
              buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos),
                            buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1))
                + 1,
              buffer.Size());
    EXPECT_EQ(std::distance(buffer.StartFrom(startPos.iX, startPos.iY),
                            buffer.StartFrom(buffer.Width() - 1, buffer.Height() - 1))
                + 1,
              buffer.Size());
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

        buffer.GetPixel(pos) = value;
        EXPECT_EQ(buffer.GetPixel(pos), buffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(buffer.GetPixel(pos), constBuffer.GetPixel(pos));
        EXPECT_EQ(buffer.GetPixel(pos.iX, pos.iY), constBuffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(*buffer.StartFrom(pos), constBuffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(*constBuffer.StartFrom(pos.iX, pos.iY), constBuffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(*constBuffer.StartFrom(pos), constBuffer.GetPixel(pos.iX, pos.iY));

        EXPECT_EQ(buffer(pos), constBuffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(buffer(pos.iX, pos.iY), constBuffer.GetPixel(pos.iX, pos.iY));

        EXPECT_EQ(buffer.GetIndex(pos.iX, pos.iY), buffer.GetIndex(pos));
        EXPECT_EQ(constBuffer.GetIndex(pos.iX, pos.iY), constBuffer.GetIndex(pos));
        EXPECT_EQ(constBuffer.GetIndex(pos), buffer.GetIndex(pos));

        EXPECT_EQ(buffer[buffer.GetIndex(pos.iX, pos.iY)], buffer.GetPixel(pos.iX, pos.iY));
        EXPECT_EQ(buffer[buffer.GetIndex(pos)], buffer.GetPixel(pos));

        EXPECT_EQ(buffer.GetPosition(buffer.GetIndex(pos)), pos);
        EXPECT_EQ(constBuffer.GetPosition(constBuffer.GetIndex(pos)), pos);
        EXPECT_EQ(constBuffer.GetPosition(buffer.GetIndex(pos)), pos);

        EXPECT_EQ(buffer.GetData()[buffer.GetIndex(pos)], buffer.GetPixel(pos));
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
struct PixelBufferViewTest : testing::Test
{
  using TPixel = taTypeParam;

  inline static constexpr TPixel pixel(int aValue)
  {
    return PixelBuildHelper<TPixel>::make(aValue);
  }

  // Support constexpr evaluation
};

TYPED_TEST_SUITE(StaticPixelTest, TPixelTypeList);
TYPED_TEST_SUITE(PixelBufferViewTest, TPixelTypeList);

/*================== Tests ====================*/
TYPED_TEST(StaticPixelTest, DefaultCreatedValueIsZero)
{
  using TPixel = TypeParam;

  auto pixel = [this](auto aValue) { return this->pixel(aValue); };

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  using PixelBuffer = StaticPixelBuffer<kWidth, kHeight, TPixel>;

  PixelBuffer buffer{pixel(0), pixel(0), pixel(0), pixel(0)};

  CommonTest<PixelBuffer>::SizeTest(buffer, kWidth, kHeight);
  CommonTest<PixelBuffer>::SizeTest(static_cast<const PixelBuffer&>(buffer), kWidth, kHeight);

  for (const auto& pixelValue : buffer)
  {
    EXPECT_EQ(pixelValue, pixel(0));
  }
}

TYPED_TEST(PixelBufferViewTest, DefaultCreatedValueIsZero)
{
  using TPixel      = TypeParam;
  auto pixel        = [this](auto aValue) { return this->pixel(aValue); };
  using PixelBuffer = PixelBufferView<TPixel>;

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  std::array<TPixel, kWidth * kHeight> rawBuffer{pixel(0), pixel(0), pixel(0), pixel(0)};
  PixelBuffer                          buffer{kWidth, kHeight, rawBuffer.data()};

  CommonTest<PixelBuffer>::SizeTest(buffer, kWidth, kHeight);
  CommonTest<PixelBuffer>::SizeTest(static_cast<const PixelBuffer&>(buffer), kWidth, kHeight);

  for (const auto& pixelValue : buffer)
  {
    EXPECT_EQ(pixelValue, pixel(0));
  }
}

template <typename taTPixel, TBufferOrientation taOrientation>
auto StaticPixelBufferOrientationTest()
{
  using TPixel      = taTPixel;
  using PixelBuffer = StaticPixelBuffer<2, 2, TPixel>;
  auto pixel        = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  using PixelBuffer = StaticPixelBuffer<kWidth, kHeight, TPixel>;

  PixelBuffer buffer{pixel(0), pixel(0), pixel(0), pixel(0)};

  CommonTest<PixelBuffer>::FillTest(buffer);
};

TYPED_TEST(StaticPixelTest, Fill)
{
  using TPixel = TypeParam;

  StaticPixelBufferOrientationTest<TPixel, TBufferOrientation::Horizontal>();
  StaticPixelBufferOrientationTest<TPixel, TBufferOrientation::Vertical>();
}

template <typename taTPixel, TBufferOrientation taOrientation>
auto PixelBufferViewOrientationTest()
{
  using TPixel      = taTPixel;
  using PixelBuffer = PixelBufferView<TPixel, taOrientation>;
  auto pixel        = [](auto aValue) { return PixelBuildHelper<TPixel>::make(aValue); };

  static constexpr size_t kWidth  = 2;
  static constexpr size_t kHeight = 2;

  std::array<TPixel, kWidth * kHeight> rawBuffer{pixel(0), pixel(0), pixel(0), pixel(0)};
  PixelBuffer                          buffer{kWidth, kHeight, rawBuffer.data()};

  CommonTest<PixelBuffer>::FillTest(buffer);
};

TYPED_TEST(PixelBufferViewTest, Fill)
{
  using TPixel = TypeParam;

  PixelBufferViewOrientationTest<TPixel, TBufferOrientation::Horizontal>();
  PixelBufferViewOrientationTest<TPixel, TBufferOrientation::Vertical>();
}

// TODO: implement rest of the tests
