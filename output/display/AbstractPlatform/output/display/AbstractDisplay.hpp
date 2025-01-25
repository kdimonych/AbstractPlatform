#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <cstdint>
#include <cassert>
#include <memory>

namespace AbstractPlatform
{

struct TBitPixel
{
    constexpr TBitPixel( ) = default;
    constexpr TBitPixel( bool aPixelValue )
        : iPixelValue{ aPixelValue }
    {
    }

    constexpr
    operator bool( ) const
    {
        return iPixelValue;
    }

    bool iPixelValue = false;

    /**
     * @brief Returns the bit count used to represent the pixel value.
     *
     * @return constexpr size_t The bit count used to represent the pixel value.
     */
    static constexpr size_t
    Bits( )
    {
        return 1;
    }
};

struct TRGBPixel
{
    constexpr TRGBPixel( ) = default;
    constexpr TRGBPixel( std::uint8_t aRed, std::uint8_t aGreen, std::uint8_t aBlue )
        : iRed{ aRed }
        , iGreen{ aGreen }
        , iBlue{ aBlue }
    {
    }

    std::uint8_t iRed = 0;
    std::uint8_t iGreen = 0;
    std::uint8_t iBlue = 0;

    /**
     * @brief Returns the bit count used to represent the pixel value.
     *
     * @return constexpr size_t The bit count used to represent the pixel value.
     */
    static constexpr size_t
    Bits( )
    {
        return 24;
    }
};

struct TPosition
{
    using TIndex = int;
    TIndex iX = 0;
    TIndex iY = 0;
};

enum class TPlottingOrigin
{
    TopLeftCorner,
    BottomLeftCorner
};

class TCanvasIteratorBase
{
protected:
    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    int PixelWidthFwd( ) const NOEXCEPT;

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    int PixelHeightFwd( ) const NOEXCEPT;

    /**
     * @brief Set the current coordinate of the modification start from, e.g. the pixel to modify.
     *
     * @param aX x coordinate of the pixel.
     * @param aY y coordinate of the pixel.
     *
     * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
     * the interface implementation is not expected to check both aX and aY values.
     */
    void SetPositionFwd( int aX, int aY ) NOEXCEPT;

public:
    TCanvasIteratorBase( );
    ~TCanvasIteratorBase( );
};

template < typename taPixelValue >
class TAbstractReadOnlyCanvas
{
public:
    using TPixel = taPixelValue;

    virtual ~TAbstractReadOnlyCanvas( ) = default;

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    virtual int PixelWidth( ) const NOEXCEPT = 0;

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    virtual int PixelHeight( ) const NOEXCEPT = 0;

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @param aPosition The {x, y} coordinates of the pixel.
     * @return TPixel The value of the pixel.
     */
    virtual TPixel GetPixel( const TPosition& aPosition ) const NOEXCEPT = 0;

    /**
     * @brief Gets the implementation defined RAW buffer pointer for read-only operations.
     *
     * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    virtual const std::uint8_t* GetRawBuffer( ) const NOEXCEPT = 0;

    /**
     * @brief Gets the implementation defined RAW buffer size in bytes.
     *
     * @return The implementation-defined RAW buffer size in bytes. Returns 0ul in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    virtual size_t GetRawBufferSize( ) const NOEXCEPT = 0;
};

template < typename taPixelValue >
class TAbstractCanvas : virtual public TAbstractReadOnlyCanvas< taPixelValue >
{
public:
    using TAbstractReadOnlyCanvas = class TAbstractReadOnlyCanvas< taPixelValue >;
    using TPixel = typename TAbstractReadOnlyCanvas::TPixel;

    virtual ~TAbstractCanvas( ) = default;

    /**
     * @brief Sets a pixel value located at the current coordinates.
     *
     * @param aPosition The {x, y} coordinates of the pixel.
     * @param TPixel A pixel value to set.
     *
     */
    virtual void SetPixel( const TPosition& aPosition, TPixel aPixelValue ) NOEXCEPT = 0;

    /**
     * @brief Fills entire canvas with provided pixel value.
     *
     * @param TPixel A pixel value to fill the canvas with.
     */
    virtual void
    FillWith( TPixel aPixelValue ) NOEXCEPT
    {
        for ( int x = 0; x < this->PixelWidth( ); ++x )
        {
            for ( int y = 0; y < this->PixelHeight( ); ++y )
            {
                this->SetPosition( x, y );
                SetPixel( aPixelValue );
            }
        }
    }

    /**
     * @brief Clears the canvas
     */
    virtual void
    Clear( ) NOEXCEPT
    {
        FillWith( TPixel{ } );
    }

    virtual void
    MergeCanvas( const TPosition& aStartPosition,
                 TAbstractReadOnlyCanvas& aSourceCanvas,
                 const TPosition& aSourceFrom,
                 const TPosition& aSourceTo ) NOEXCEPT
    {
        assert( aStartPosition.iX >= 0 );
        assert( aStartPosition.iY >= 0 );
        assert( aStartPosition.iX < this->PixelWidth( ) );
        assert( aStartPosition.iY < this->PixelHeight( ) );

        assert( aSourceFrom.iX >= 0 );
        assert( aSourceFrom.iY >= 0 );
        assert( aSourceFrom.iX < aSourceCanvas.PixelWidth( ) );
        assert( aSourceTo.iY < aSourceCanvas.PixelHeight( ) );

        assert( aSourceTo.iX >= 0 );
        assert( aSourceTo.iY >= 0 );
        assert( aSourceTo.iX < aSourceCanvas.PixelWidth( ) );
        assert( aSourceFrom.iX < aSourceCanvas.PixelHeight( ) );

        const auto sourceWidth = std::abs( aSourceTo.iX - aSourceFrom.iX ) + 1;
        const auto sourceHeight = std::abs( aSourceTo.iY - aSourceFrom.iY ) + 1;

        const auto pixelWidth = std::min( this->PixelHeight( ), sourceWidth );
        const auto pixelHeight = std::min( this->PixelHeight( ), sourceHeight );

        int targetY = aStartPosition.iY;
        int sourceY = aSourceFrom.iY;

        for ( ; targetY < pixelHeight; ++targetY, ++sourceY )
        {
            int targetX = aStartPosition.iX;
            int sourceX = aSourceFrom.iX;
            for ( ; targetX < pixelWidth; ++targetX, ++sourceX )
            {
                this->SetPosition( targetX, targetY );
                aSourceCanvas.SetPosition( sourceX, sourceY );
                SetPixel( aSourceCanvas.GetPixel( ) );
            }
        }
    }

    /**
     * @brief Gets the implementation defined RAW buffer pointer for read/write operations.
     *
     * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    virtual std::uint8_t* GetRawBuffer( ) NOEXCEPT = 0;
};

}  // namespace AbstractPlatform
