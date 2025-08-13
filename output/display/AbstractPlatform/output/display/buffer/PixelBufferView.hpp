#pragma once
#include <AbstractPlatform/common/Buffer.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/output/display/buffer/PixelBufferImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <new>
#include <variant>

namespace AbstractPlatform {
template <typename taRawBuffer, TBufferOrientation taOrientation = TBufferOrientation::Horizontal>
struct TPixelBufferView;

template <typename taRawBuffer, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TPixelBufferView<taRawBuffer, taOrientation>>
{
  using TBuffer                                    = taRawBuffer;
  using TBufferTraits                              = TBufferTraits<TBuffer>;
  using TPixel                                     = typename TBufferTraits::TValueType;
  using TBufferRef                                 = TBuffer&;
  using TConstBufferRef                            = const TBuffer&;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <typename taRawBuffer, TBufferOrientation taOrientation>
struct TPixelBufferView : public TPixelBufferImpl<TPixelBufferView<taRawBuffer, taOrientation>>
{
  using TThis                                      = TPixelBufferView<taRawBuffer, taOrientation>;
  using TPixelBufferImpl                           = TPixelBufferImpl<TThis>;
  using TTraits                                    = TPixelBufferTraits<TThis>;
  using TBufferTraits                              = typename TTraits::TBufferTraits;
  using TPixel                                     = typename TTraits::TPixel;
  using TBufferRef                                 = typename TTraits::TBufferRef;
  using TConstBufferRef                            = typename TTraits::TConstBufferRef;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  TPixelBufferView(size_t aWidth, size_t aHeight, TBufferRef buffer)
    : TPixelBufferImpl()
    , iPixelBuffer{buffer}
    , iWidth{aWidth}
    , iHeight{aHeight}
  {
    assert(buffer != nullptr);
    assert(aWidth > 0 && aHeight > 0);
  }

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return iWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return iHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return Width() * Height();
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified absolute index.
   */
  inline constexpr TPixel GetPixel(size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    return TBufferTraits::GetValue(iPixelBuffer, aIndex);
  }

  /**
   * @brief Set the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified index.
   */
  inline constexpr void SetPixel(size_t aIndex, TPixel aPixel) NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    TBufferTraits::SetValue(iPixelBuffer, aIndex, aPixel);
  }

  inline constexpr void Clear() NOEXCEPT
  {
    TBufferTraits::SetAll(iPixelBuffer, TPixel{});
  }

  inline constexpr void SetAll(TPixel aPixel) NOEXCEPT
  {
    TBufferTraits::SetAll(iPixelBuffer, aPixel);
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferRef GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr TBufferRef GetInnerBuffer() NOEXCEPT
  {
    return iPixelBuffer;
  }

  TBufferRef   iPixelBuffer;
  const size_t iWidth  = 0u;
  const size_t iHeight = 0u;
};

template <typename taRawBuffer, TBufferOrientation taOrientation>
struct TPixelBufferTraits<TPixelBufferView<const taRawBuffer, taOrientation>>
{
  using TBuffer                                    = taRawBuffer;
  using TBufferTraits                              = TBufferTraits<TBuffer>;
  using TPixel                                     = typename TBufferTraits::TValueType;
  using TConstBufferRef                            = const TBuffer&;
  static constexpr TBufferOrientation kOrientation = taOrientation;
};

template <typename taRawBuffer, TBufferOrientation taOrientation>
struct TPixelBufferView<const taRawBuffer, taOrientation>
  : public PixelBufferConstImpl<TPixelBufferView<const taRawBuffer, taOrientation>>
{
  using TThis            = TPixelBufferView<const taRawBuffer, taOrientation>;
  using TTraits          = TPixelBufferTraits<TThis>;
  using TPixelBufferImpl = TPixelBufferImpl<TThis>;
  using TBufferTraits    = typename TTraits::TBufferTraits;
  using TPixel           = typename TTraits::TPixel;
  using TPixelConstRef   = typename TTraits::TPixelConstRef;
  using TConstBufferRef  = typename TTraits::TConstBufferRef;
  static constexpr TBufferOrientation kOrientation = TTraits::kOrientation;

  using TPixelBufferImpl::GetPixel;
  using TPixelBufferImpl::SetPixel;
  using TPixelBufferImpl::TPixelBufferImpl;

  TPixelBufferView(size_t aWidth, size_t aHeight, TConstBufferRef buffer)
    : iPixelBuffer{buffer}
    , iWidth{aWidth}
    , iHeight{aHeight}
  {
    assert(aWidth > 0 && aHeight > 0);
  }

  /**
   * @brief Returns the width of the pixel buffer.
   *
   * @return size_t The width of the pixel buffer.
   */
  inline constexpr const size_t Width() const NOEXCEPT
  {
    return iWidth;
  }

  /**
   * @brief Returns the height of the pixel buffer.
   *
   * @return size_t The height of the pixel buffer.
   */
  inline constexpr const size_t Height() const NOEXCEPT
  {
    return iHeight;
  }

  /**
   * @brief Returns the size of the pixel buffer in pixels.
   *
   * @return size_t The size of the pixel buffer in pixels.
   */
  inline constexpr const size_t Size() const NOEXCEPT
  {
    return Width() * Height();
  }

  /**
   * @brief Get the Pixel object
   *
   * @param aIndex The absolute index of the pixel in the inner array.
   * @return TPixel& The pixel at the specified absolute index.
   * @note The method asserts that the coordinates are within the bounds of the pixel buffer.
   *       If the coordinates are out of bounds, it will trigger an assertion failure.
   * @note The method is noexcept, meaning it does not throw exceptions.
   *       It is expected to be used in performance-critical code where exceptions are not desired.
   * @note The method uses assertions to ensure that the coordinates are valid.
   *       This is useful during development to catch errors early.
   */
  inline constexpr TPixel GetPixel(size_t aIndex) const NOEXCEPT
  {
    assert(aIndex < Width() * Height());
    return TBufferTraits::GetValue(iPixelBuffer, aIndex);
  }

  /**
   * @brief Get the Buffer object
   *
   * @return const TPixel*
   */
  inline constexpr const TConstBufferRef GetInnerBuffer() const NOEXCEPT
  {
    return iPixelBuffer;
  }

  TConstBufferRef iPixelBuffer;
  const size_t    iWidth  = 0u;
  const size_t    iHeight = 0u;
};

template <TBufferOrientation taOrientation = TBufferOrientation::Horizontal, typename taBuffer>
TPixelBufferView<taBuffer, taOrientation>
CreatePixelBufferView(size_t aWidth, size_t aHeight, taBuffer& aBuffer)
{
  return TPixelBufferView<taBuffer, taOrientation>(aWidth, aHeight, aBuffer);
}

template <TBufferOrientation taOrientation = TBufferOrientation::Horizontal, typename taBuffer>
TPixelBufferView<const taBuffer, taOrientation>
CreatePixelBufferView(size_t aWidth, size_t aHeight, const taBuffer& aBuffer)
{
  return TPixelBufferView<const taBuffer, taOrientation>(aWidth, aHeight, aBuffer);
}

// TODO: implement support of compressed buffers

} // namespace AbstractPlatform
