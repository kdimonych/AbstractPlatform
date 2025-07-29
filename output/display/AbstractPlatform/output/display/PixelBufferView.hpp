#pragma once
#include <AbstractPlatform/common/Buffer.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/Position.hpp>
#include <AbstractPlatform/output/display/impl/PixelBufferImpl.hpp>
#include <AbstractPlatform/platform/Platform.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <new>
#include <variant>

namespace AbstractPlatform {

enum class TPixelCompression
{
  None,
  Compressed,
};

// template <typename taPixel>
// struct PixelBufferView : public PixelBufferConstImpl<PixelBufferView<taPixel>>
// {
//   using TPixel  = taPixel;
//   using TBuffer = const TPixel*;

//   PixelBufferView(const TBuffer* buffer, size_t aWidth, size_t aHeight)
//     : iPixelBuffer{buffer}
//     , iWidth{aWidth}
//     , iHeight{aHeight}
//   {
//     assert(aWidth > 0 && aHeight > 0);
//   }

//   /**
//    * @brief Returns the width of the pixel buffer.
//    *
//    * @return size_t The width of the pixel buffer.
//    */
//   inline constexpr const size_t Width() const NOEXCEPT
//   {
//     return this->iWidth;
//   }

//   /**
//    * @brief Returns the height of the pixel buffer.
//    *
//    * @return size_t The height of the pixel buffer.
//    */
//   inline constexpr const size_t Height() const NOEXCEPT
//   {
//     return this->iHeight;
//   }

//   /**
//    * @brief Get the Buffer object
//    *
//    * @return const TPixel*
//    */
//   inline constexpr const TPixel* GetBuffer() const NOEXCEPT
//   {
//     return this->iPixelBuffer;
//   }

//   TPixel*      iPixelBuffer;
//   const size_t iWidth  = 0u;
//   const size_t iHeight = 0u;
// };

} // namespace AbstractPlatform
