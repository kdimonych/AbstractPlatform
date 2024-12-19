#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>
#include <AbstractPlatform/common/ArrayHelper.hpp>
#include <AbstractPlatform/output/display/AbstractDisplay.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>
#include <AbstractPlatform/common/TypeBinaryRepresentation.hpp>
// #include <AbstractPlatform/tensor/StaticTensorIterator.hpp>

#include <cstdint>

namespace AbstractPlatform
{
template < typename taPixelValue, size_t taWidth, size_t taHeight >
class TStaticCanvasView : public TAbstractReadOnlyCanvas< taPixelValue >
{
    using TPixel = taPixelValue;
    static constexpr auto kPixelWidth = static_cast< int >( taWidth );
    static constexpr auto kPixelHeight = static_cast< int >( taHeight );

    virtual ~TStaticCanvasView( ) = default;

    TStaticCanvasView( const std::uint8_t* aStaticCanvasBuffer,
                       [[maybe_unused]] size_t aBufferSize )
        : iStaticCanvasBuffer{ aStaticCanvasBuffer }
    {
        assert( kBytesPerWidth * kPixelHeight <= aBufferSize );
    }

    template < typename taArrayElement, size_t taBuffSize >
    TStaticCanvasView( taArrayElement const ( &aStaticCanvasBuffer )[ taBuffSize ] )
        : iStaticCanvasBuffer{ static_cast< const std::uint8_t* >( aStaticCanvasBuffer ) }
    {
        static_assert( kBytesPerWidth * kPixelHeight <= ArraySizeBytes( aStaticCanvasBuffer ),
                       "The buffer size is not enough to fit the TStaticCanvasView" );
    }

    template < typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight >
    TStaticCanvasView(
        taArrayElement const ( &aStaticCanvasBuffer )[ taBuffSizeWidth ][ taBuffSizeHeight ] )
        : iStaticCanvasBuffer{ static_cast< const std::uint8_t* >( aStaticCanvasBuffer ) }
    {
        static_assert( kBytesPerWidth * kPixelHeight <= ArraySizeBytes( aStaticCanvasBuffer ),
                       "The buffer size is not enough to fit the TStaticCanvasView" );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    virtual int
    PixelWidth( ) const NOEXCEPT override
    {
        return kPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    virtual int
    PixelHeight( ) const NOEXCEPT override
    {
        return kPixelHeight;
    }

    /**
     * @brief Set the current coordinate of the modification start from, e.g. the pixel to modify.
     *
     * @param aX x coordinate of the pixel.
     * @param aY y coordinate of the pixel.
     *
     * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
     * the interface implementation is not expected to check both aX and aY values.
     */
    virtual void
    SetPosition( int aX, int aY ) NOEXCEPT override
    {
    }

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return TPosition Current coordinate
     */
    TPosition
    GetPosition( ) const NOEXCEPT override
    {
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @return TPixel The value of the pixel.
     */
    virtual TPixel
    GetPixel( ) const NOEXCEPT override
    {
    }

private:
    static constexpr size_t kBytesPerWidth
        = ( kPixelWidth * TPixel::Bits( ) + kBitsPerByte - 1 ) / kBitsPerByte;

    const std::uint8_t* iStaticCanvasBuffer;
    size_t iBitIndex;
};

template < size_t taWidth, size_t taHeight >
class TStaticCanvasView< TBitPixel, taWidth, taHeight >
    : public TAbstractReadOnlyCanvas< TBitPixel >
{
public:
    using TPixel = TBitPixel;
    static constexpr auto kPixelWidth = static_cast< int >( taWidth );
    static constexpr auto kPixelHeight = static_cast< int >( taHeight );

    static_assert( ( taWidth % kBitsPerByte ) == 0, "taWidth has to be multiple of 8" );

    virtual ~TStaticCanvasView( ) = default;

    TStaticCanvasView( const std::uint8_t* aStaticCanvasBuffer,
                       [[maybe_unused]] size_t aBufferSize )
        : iStaticCanvasBuffer{ aStaticCanvasBuffer }
    {
        assert( kBytesPerWidth * kPixelHeight <= aBufferSize );
    }

    template < typename taArrayElement, size_t taBuffSize >
    TStaticCanvasView( taArrayElement const ( &aStaticCanvasBuffer )[ taBuffSize ] )
        : iStaticCanvasBuffer{ static_cast< const std::uint8_t* >( aStaticCanvasBuffer ) }
    {
        static_assert( kBytesPerWidth * kPixelHeight <= ArraySizeBytes( aStaticCanvasBuffer ),
                       "The buffer size is not enough to fit the TStaticCanvasView" );
    }

    template < typename taArrayElement, size_t taBuffSizeWidth, size_t taBuffSizeHeight >
    TStaticCanvasView(
        taArrayElement const ( &aStaticCanvasBuffer )[ taBuffSizeWidth ][ taBuffSizeHeight ] )
        : iStaticCanvasBuffer{ static_cast< const std::uint8_t* >( aStaticCanvasBuffer ) }
    {
        static_assert( kBytesPerWidth * kPixelHeight <= ArraySizeBytes( aStaticCanvasBuffer ),
                       "The buffer size is not enough to fit the TStaticCanvasView" );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    virtual int
    PixelWidth( ) const NOEXCEPT override
    {
        return kPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    virtual int
    PixelHeight( ) const NOEXCEPT override
    {
        return kPixelHeight;
    }

    /**
     * @brief Set the current coordinate of the modification start from, e.g. the pixel to modify.
     *
     * @param aX x coordinate of the pixel.
     * @param aY y coordinate of the pixel.
     *
     * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
     * the interface implementation is not expected to check both aX and aY values.
     */
    virtual void
    SetPosition( int aX, int aY ) NOEXCEPT override
    {
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

        iBufferIndex = aX / kBitsPerByte + kBytesPerWidth * aY;
        iBitIndex = aX % kBitsPerByte;
    }

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return TPosition Current coordinate
     */
    TPosition
    GetPosition( ) const NOEXCEPT override
    {
        const auto y = ( iBufferIndex % kBytesPerWidth );
        return { y * kBitsPerByte + iBitIndex, y };
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @return TPixel The value of the pixel.
     */
    virtual TPixel
    GetPixel( ) const NOEXCEPT override
    {
        return CheckBit( iStaticCanvasBuffer[ iBufferIndex ], iBitIndex );
    }

private:
    static constexpr size_t kBytesPerWidth
        = ( kPixelWidth * TPixel::Bits( ) + kBitsPerByte - 1 ) / kBitsPerByte;

    const std::uint8_t* iStaticCanvasBuffer;
    size_t iBitIndex;
    size_t iBufferIndex;
};
}  // namespace AbstractPlatform