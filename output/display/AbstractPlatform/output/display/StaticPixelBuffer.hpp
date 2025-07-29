#pragma once
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/output/display/impl/PixelBufferImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <new>
#include <variant>

namespace AbstractPlatform {
/**
 * @brief The PixelBufer structure is used to store pixel data.
 * It contains the width and height of the pixel buffer and a unique pointer to the pixel data.
 * The buffer structure:
 * @code
 *  First element of the buffer.
 *      |
 *      V
 * | x:0, y:0          | x:1, y:0         | ... | x:iWidth-1, y:0         |
 * | x:1, y:0          | x:1, y:0         | ... | x:iWidth-1, y:1         |
 * | x:1, y:0          | x:1, y:0         | ... | x:iWidth-1, y:2         |
 *                                    ...
 * | x:01, y:iHeight-1 | x:1, y:iHeight-1 | ... | x:iWidth-1, y:iHeight-1 |
 *                                                           ^
 *                                                           |
 *                                              Last element of the buffer.
 * @endcode
 */
// Config optimized for stack allocation
template <size_t taWidth, size_t taHeight, typename taPixel>
struct StaticPixelBufferConfig
{
  using TPixel         = taPixel;
  using TIndex         = TPosition::TIndex;
  using TBuffer        = std::array<TPixel, taWidth * taHeight>;
  using TIterator      = TPixel*;
  using TConstIterator = const TPixel*;

  static constexpr size_t kWidth  = taWidth;
  static constexpr size_t kHeight = taHeight;

  inline static constexpr TBuffer make()
  {
    return TBuffer{};
  }

  inline static TBuffer make(TPixel aDefaultPixel)
  {
    auto buffer = make();
    std::fill(buffer.get(), buffer.get() + (kWidth * kHeight), aDefaultPixel);
    return buffer;
  }

  template <typename... taPixels>
  inline static constexpr TBuffer make(taPixels&&... aPixels)
  {
    static_assert(sizeof...(taPixels) == kWidth * kHeight, "Invalid number of pixels");
    return TBuffer{{std::forward<taPixels>(aPixels)...}};
  }
};

template <size_t taWidth,
          size_t taHeight,
          typename taPixel,
          template <size_t, size_t, typename> typename taConfig = StaticPixelBufferConfig>
struct StaticPixelBuffer;

template <size_t taWidth,
          size_t taHeight,
          typename taPixel,
          template <size_t, size_t, typename> typename taConfig>
struct PixelBufferTraits<StaticPixelBuffer<taWidth, taHeight, taPixel, taConfig>>
{
  using TConfig        = taConfig<taWidth, taHeight, taPixel>;
  using TPixel         = typename TConfig::TPixel;
  using TIndex         = TPosition::TIndex;
  using TBuffer        = typename TConfig::TBuffer;
  using TIterator      = typename TConfig::TIterator;
  using TConstIterator = typename TConfig::TConstIterator;

  static constexpr size_t kWidth  = taWidth;
  static constexpr size_t kHeight = taHeight;
};

template <size_t taWidth,
          size_t taHeight,
          typename taPixel,
          template <size_t, size_t, typename> typename taConfig>
struct StaticPixelBuffer
  : public PixelBufferImpl<StaticPixelBuffer<taWidth, taHeight, taPixel, taConfig>>
{
  using TThis   = StaticPixelBuffer<taWidth, taHeight, taPixel, taConfig>;
  using TConfig = taConfig<taWidth, taHeight, taPixel>;
  using TTraits = PixelBufferTraits<TThis>;
  using TPixel  = typename TTraits::TPixel;
  using TIndex  = typename TTraits::TIndex;
  using TBuffer = typename TTraits::TBuffer;

  static constexpr size_t kWidth  = TTraits::kWidth;
  static constexpr size_t kHeight = TTraits::kHeight;

  static_assert(kWidth > 0 && kHeight > 0);

  /**
   * @brief Construct a new Static Pixel Buffer object
   * NOTE: This will create a pixel buffer with default-initialized pixels.
   *       If you want to initialize the pixels, use the constructor with parameters.
   */
  inline constexpr StaticPixelBuffer()
    : PixelBufferImpl<TThis>()
    , iPixelBuffer{TConfig::make()}
  {
  }

  inline constexpr StaticPixelBuffer(TPixel aDefaultPixel)
    : PixelBufferImpl<TThis>()
    , iPixelBuffer{TConfig::make(aDefaultPixel)}
  {
  }

  template <typename... taPixels>
  inline constexpr StaticPixelBuffer(taPixels&&... aPixels)
    : PixelBufferImpl<StaticPixelBuffer<taWidth, taHeight, taPixel>>()
    , iPixelBuffer{TConfig::make(std::forward<taPixels>(aPixels)...)}
  {
    static_assert(sizeof...(taPixels) == kWidth * kHeight, "Invalid number of pixels");
  }

  StaticPixelBuffer(StaticPixelBuffer&&)            = default;
  StaticPixelBuffer& operator=(StaticPixelBuffer&&) = default;

  // Copy deliberately prohibited to enforce more efficient usage patterns.
  // This is to ensure that the pixel buffer is not copied, but rather moved or initialized
  StaticPixelBuffer(const StaticPixelBuffer&)            = delete;
  StaticPixelBuffer& operator=(const StaticPixelBuffer&) = delete;

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return kWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return kHeight;
  }

  /**
   * @brief Returns the pointer to the pixel buffer.
   *
   * @return TPixel* The pointer to the pixel buffer.
   */
  inline constexpr TPixel* GetBuffer() NOEXCEPT
  {
    return iPixelBuffer.data();
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TPixel* GetBuffer() const NOEXCEPT
  {
    return iPixelBuffer.data();
  }

  std::array<TPixel, kWidth * kHeight> iPixelBuffer;
};

} // namespace AbstractPlatform
