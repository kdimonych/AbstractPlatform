#pragma once

#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>
#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>
#include <AbstractPlatform/output/display/AbstractCanvas.hpp>
// #include <AbstractPlatform/tensor/StaticTensorIterator.hpp>

#include <cstdint>
#include <tuple>

namespace AbstractPlatform {
template <typename taPixelValue, size_t taWidth, size_t taHeight>
class TStaticCanvasView : public TAbstractCanvas
{
  using TPixel                       = taPixelValue;
  static constexpr auto kPixelWidth  = static_cast<int>(taWidth);
  static constexpr auto kPixelHeight = static_cast<int>(taHeight);

  TStaticCanvasView(const std::uint8_t* aStaticCanvasBuffer, [[maybe_unused]] size_t aBufferSize)
    : iStaticCanvasBuffer{aStaticCanvasBuffer}
  {
    assert(aBufferSize >= kBufferSize);
  }

  template <typename taArrayElement, size_t taBuffSize>
  TStaticCanvasView(const taArrayElement (&aStaticCanvasBuffer)[taBuffSize])
    : iStaticCanvasBuffer{static_cast<const std::uint8_t*>(aStaticCanvasBuffer)}
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
  }

  template <typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight>
  TStaticCanvasView(const taArrayElement (&aStaticCanvasBuffer)[taBuffSizeWidth][taBuffSizeHeight])
    : iStaticCanvasBuffer{static_cast<const std::uint8_t*>(aStaticCanvasBuffer)}
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
  }

  ~TStaticCanvasView() override = default;

  /**
   * @brief Resets the buffer with new buffer pointer.
   *
   * @param aStaticCanvasBuffer A new buffer pointer.
   */
  void ResetBuffer(const std::uint8_t* aStaticCanvasBuffer) NOEXCEPT
  {
    assert(aStaticCanvasBuffer != nullptr);
    iStaticCanvasBuffer = aStaticCanvasBuffer;
  }

  template <typename taArrayElement, size_t taBuffSize>
  void ResetBuffer(const taArrayElement (&aStaticCanvasBuffer)[taBuffSize])
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
    iStaticCanvasBuffer = static_cast<const std::uint8_t*>(aStaticCanvasBuffer);
  }

  template <typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight>
  void ResetBuffer(const taArrayElement (&aStaticCanvasBuffer)[taBuffSizeWidth][taBuffSizeHeight])
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
    iStaticCanvasBuffer = static_cast<const std::uint8_t*>(aStaticCanvasBuffer);
  }

  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  int PixelWidth() const NOEXCEPT override
  {
    return PixelWidthImpl();
  }

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  int PixelHeight() const NOEXCEPT override
  {
    return PixelHeightImpl();
  }

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @return TPixel The value of the pixel.
   */
  void GetPixel(const TPosition& aPosition, TAbstractCanvas::TPixel& pixel) const NOEXCEPT override
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < kPixelWidth);
    assert(aPosition.iY < kPixelHeight);

    pixel = GetPixelImpl(aPosition);
  }

protected:
  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  inline static constexpr int PixelWidthImpl() NOEXCEPT

  {
    return kPixelWidth;
  }

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  inline static constexpr int PixelHeightImpl() NOEXCEPT
  {
    return kPixelHeight;
  }

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @return TPixel The value of the pixel.
   * Note: the calling side controls the ranges of both aX and aY values. To preserve
   * performance, the interface implementation is not expected to check both aX and aY values.
   */
  TPixel GetPixelImpl(const TPosition& aPosition) const NOEXCEPT
  {
    const auto bufferIndex = BufferIndexFromPosition(aPosition);
    return TPixel{iStaticCanvasBuffer[bufferIndex],
                  iStaticCanvasBuffer[bufferIndex + 1],
                  iStaticCanvasBuffer[bufferIndex + 2]};
  }

  /**
   * @brief Gets the implementation defined RAW buffer pointer for read-only operations.
   *
   * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
   * is not accessible for implementation or not ready for use.
   */
  inline const std::uint8_t* GetRawBufferImpl() const NOEXCEPT
  {
    return iStaticCanvasBuffer;
  }

  /**
   * @brief Gets the implementation defined RAW buffer size in bytes.
   *
   * @return The implementation-defined RAW buffer size in bytes. Returns 0ul in case the buffer
   * is not accessible for implementation or not ready for use.
   */
  inline static constexpr size_t GetRawBufferSizeImpl() NOEXCEPT
  {
    return kBufferSize;
  }

private:
  /**
   * @brief Converts the current coordinate to a buffer index.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   *
   * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
   * the interface implementation is not expected to check both aX and aY values.
   */
  inline static size_t BufferIndexFromPosition(const TPosition& aPosition) NOEXCEPT
  {
    assert(aPosition.iX < kPixelWidth);
    assert(aPosition.iY < kPixelHeight);
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);

    const auto x = static_cast<size_t>(aPosition.iX);
    const auto y = static_cast<size_t>(aPosition.iY);

    return x * kBytesPerPixel + kBytesPerWidth * y;
  }

  inline static TPosition PositionFromBufferIndex(size_t aBufferIndex) NOEXCEPT
  {
    assert(aBufferIndex < kBytesPerWidth * kPixelHeight);
    return {static_cast<TPosition::TIndex>((aBufferIndex % kBytesPerWidth) / kBytesPerPixel),
            static_cast<TPosition::TIndex>(aBufferIndex / kBytesPerWidth)};
  }

  static constexpr size_t kBytesPerPixel = (TPixel::Bits() + kBitsPerByte - 1) / kBitsPerByte;
  static constexpr size_t kBytesPerWidth = kPixelWidth * kBytesPerPixel;
  static constexpr size_t kBufferSize    = kBytesPerWidth * kPixelHeight;

  const std::uint8_t* iStaticCanvasBuffer;
};

template <size_t taWidth, size_t taHeight>
class TStaticCanvasView<TBitPixel, taWidth, taHeight> : public TAbstractCanvas
{
public:
  using TPixel                       = TBitPixel;
  static constexpr auto kPixelWidth  = static_cast<int>(taWidth);
  static constexpr auto kPixelHeight = static_cast<int>(taHeight);

  static_assert((taWidth % kBitsPerByte) == 0, "taWidth has to be multiple of 8");

  TStaticCanvasView(const std::uint8_t* aStaticCanvasBuffer, [[maybe_unused]] size_t aBufferSize)
    : iStaticCanvasBuffer{aStaticCanvasBuffer}
  {
    assert(kBytesPerWidth * kPixelHeight <= aBufferSize);
  }

  template <typename taArrayElement, size_t taBuffSize>
  TStaticCanvasView(const taArrayElement (&aStaticCanvasBuffer)[taBuffSize])
    : iStaticCanvasBuffer{static_cast<const std::uint8_t*>(aStaticCanvasBuffer)}
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
  }

  template <typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight>
  TStaticCanvasView(const taArrayElement (&aStaticCanvasBuffer)[taBuffSizeWidth][taBuffSizeHeight])
    : iStaticCanvasBuffer{static_cast<const std::uint8_t*>(aStaticCanvasBuffer)}
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
  }

  ~TStaticCanvasView() override = default;

  /**
   * @brief Resets the buffer with new buffer pointer.
   *
   * @param aStaticCanvasBuffer A new buffer pointer.
   */
  void ResetBuffer(const std::uint8_t* aStaticCanvasBuffer) NOEXCEPT
  {
    assert(aStaticCanvasBuffer != nullptr);
    iStaticCanvasBuffer = aStaticCanvasBuffer;
  }

  template <typename taArrayElement, size_t taBuffSize>
  void ResetBuffer(const taArrayElement (&aStaticCanvasBuffer)[taBuffSize])
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
    iStaticCanvasBuffer = static_cast<const std::uint8_t*>(aStaticCanvasBuffer);
  }

  template <typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight>
  void ResetBuffer(const taArrayElement (&aStaticCanvasBuffer)[taBuffSizeWidth][taBuffSizeHeight])
  {
    static_assert(ArraySizeBytes(aStaticCanvasBuffer) >= kBufferSize,
                  "The buffer size is not enough to fit the TStaticCanvasView");
    iStaticCanvasBuffer = static_cast<const std::uint8_t*>(aStaticCanvasBuffer);
  }

  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  int PixelWidth() const NOEXCEPT override
  {
    return PixelWidthImpl();
  }

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  int PixelHeight() const NOEXCEPT override
  {
    return PixelHeightImpl();
  }

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @param aPosition The {x, y} coordinates of the pixel.
   * @return TPixel The value of the pixel.
   */
  void GetPixel(const TPosition& aPosition, TAbstractCanvas::TPixel& pixel) const NOEXCEPT override
  {
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);
    assert(aPosition.iX < kPixelWidth);
    assert(aPosition.iY < kPixelHeight);

    pixel = GetPixelImpl(aPosition);
  }

protected:
  /**
   * @brief Returns pixel width of the canvas.
   *
   * @return int A pixel width of the canvas.
   */
  inline static constexpr int PixelWidthImpl() NOEXCEPT
  {
    return kPixelWidth;
  }

  /**
   * @brief Returns pixel height of the canvas.
   *
   * @return int A pixel height of the canvas.
   */
  inline static constexpr int PixelHeightImpl() NOEXCEPT
  {
    return kPixelHeight;
  }

  /**
   * @brief Gets a pixel value located at the current coordinates.
   *
   * @return TPixel The value of the pixel.
   */
  TPixel GetPixelImpl(const TPosition& aPosition) const NOEXCEPT
  {
    auto [bufferIndex, bitIndex] = ConvertPositionToBufferIndexes(aPosition);
    return CheckBit(iStaticCanvasBuffer[bufferIndex], bitIndex);
  }

  /**
   * @brief Gets the implementation defined RAW buffer pointer for read-only operations.
   *
   * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
   * is not accessible for implementation or not ready for use.
   */
  inline const std::uint8_t* GetRawBufferImpl() const NOEXCEPT
  {
    return iStaticCanvasBuffer;
  }

  /**
   * @brief Gets the implementation defined RAW buffer size in bytes.
   *
   * @return The implementation-defined RAW buffer size in bytes. Returns 0ul in case the buffer
   * is not accessible for implementation or not ready for use.
   */
  inline static constexpr size_t GetRawBufferSizeImpl() NOEXCEPT
  {
    return kBufferSize;
  }

private:
  inline static auto ConvertPositionToBufferIndexes(const TPosition& aPosition) NOEXCEPT
  {
    assert(aPosition.iX < kPixelWidth);
    assert(aPosition.iY < kPixelHeight);
    assert(aPosition.iX >= 0);
    assert(aPosition.iY >= 0);

    //    HMI representation (32 x 3 pixels canvas)
    //     x: 0      -------    pixels  ------>     32
    //  bits: 7  ...  0 7  ...  0 7  ...  0 7  ...  0
    // bytes:     3         2         1         0
    //    y 0|1111 1111|1111 1111|0001 1111|1110 1111|
    //    | 1|1111 1111|1111 1111|1110 0000|0000 0000|
    //    V 2|1111 1111|1111 1111|0001 1111|1110 1111|
    // HW friendly representation  (32 x 3 pixels canvas)
    //     x: 0      -------    pixels  ------>     32
    //  bits: 0  ...  7 0  ...  7 0  ...  7 0  ...  7
    // bytes:     0         1         2         3
    //    y 0|1111 0111|1111 1000|1111 1111|1111 1111|
    //    | 1|0000 0000|0000 0111|1111 1111|1111 1111|
    //    V 2|1111 0111|1111 1000|1111 1111|1111 1111|

    const auto x = static_cast<size_t>(aPosition.iX);
    const auto y = static_cast<size_t>(aPosition.iY);

    const size_t bufferIndex = x / kBitsPerByte + kBytesPerWidth * y;
    const size_t bitIndex    = x % kBitsPerByte;

    return std::make_tuple(bufferIndex, bitIndex);
  }

  inline static TPosition PositionFromBufferIndex(size_t aBufferIndex, size_t aBitIndex) NOEXCEPT
  {
    assert(aBufferIndex < kBufferSize);
    assert(aBitIndex < kBitsPerByte);

    return {
      static_cast<TPosition::TIndex>((aBufferIndex % kBytesPerWidth) * kBitsPerByte + aBitIndex),
      static_cast<TPosition::TIndex>(aBufferIndex / kBytesPerWidth)};
  }

  static constexpr size_t kBytesPerWidth =
    (kPixelWidth * TPixel::Bits() + kBitsPerByte - 1) / kBitsPerByte;
  static constexpr size_t kBufferSize = kBytesPerWidth * kPixelHeight;

  const std::uint8_t* iStaticCanvasBuffer;
};
} // namespace AbstractPlatform
