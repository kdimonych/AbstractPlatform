#pragma once
#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>

#include <cstdint>
#include <cmath>

namespace AbstractPlatform
{

struct BitPixel
{
    constexpr BitPixel( ) = default;
    constexpr BitPixel( bool aPixelValue )
        : iPixelValue{ aPixelValue }
    {
    }

    constexpr operator bool( ) const
    {
        return iPixelValue;
    }

    bool iPixelValue = false;
};

struct RGBPixel
{
    constexpr RGBPixel( ) = default;
    constexpr RGBPixel( std::uint8_t aRed, std::uint8_t aGreen, std::uint8_t aBlue )
        : iRed{ aRed }
        , iGreen{ aGreen }
        , iBlue{ aBlue }
    {
    }

    std::uint8_t iRed = 0;
    std::uint8_t iGreen = 0;
    std::uint8_t iBlue = 0;
};

enum class PlottingOrigin
{
    TopLeftCorner,
    BottomLeftCorner
};

template < typename taPixelValue,
           PlottingOrigin taPlottingOrigin = PlottingOrigin::BottomLeftCorner >
class TAbstractCanvas
{
public:
    using TPixel = taPixelValue;
    static constexpr auto KPlottingOrigin = taPlottingOrigin;

    struct Position
    {
        int iX = 0;
        int iY = 0;
    };

    virtual ~TAbstractCanvas( ) = default;

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    virtual int PixelWidth( ) const = 0;

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int
     */
    virtual int PixelHeight( ) const = 0;

    /**
     * @brief Set the current coordinate of the modification start from, e.g. the pixel to modify.
     *
     * @param aX x coordinate of the pixel.
     * @param aY y coordinate of the pixel.
     *
     * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
     * the interface implementation is not expected to check both aX and aY values.
     */
    virtual void SetPosition( int aX, int aY ) = 0;

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return Position Current coordinate
     */
    virtual Position GetPosition( ) const = 0;

    /**
     * @brief Sets a pixel value located at the current coordinates.
     *
     * @param TPixel A pixel value to set.
     *
     */
    virtual void SetPixel( TPixel aPixelValue ) = 0;

    /**
     * @brief Inverts a pixel value located at the current coordinates.
     */
    virtual void InvertPixel( ){ };

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @return TPixel The value of the pixel.
     */
    virtual TPixel GetPixel( ) const = 0;

    /**
     * @brief Fills entire canvas with provided pixel value.
     *
     * @param TPixel A pixel value to fill the canvas with.
     */
    virtual void
    FillWith( TPixel aPixelValue )
    {
        for ( int x = 0; x < PixelWidth( ); ++x )
        {
            for ( int y = 0; y < PixelHeight( ); ++y )
            {
                SetPosition( x, y );
                SetPixel( aPixelValue );
            }
        }
    }

    /**
     * @brief Clears the canvas
     */
    virtual void
    Clear( )
    {
        FillWith( TPixel{ } );
    }

    virtual void
    AddCanvas( TAbstractCanvas& aSourceCanvas, int aFromX, int aFromY, int aToX, int aToY )
    {
        assert( aFromX >= 0 );
        assert( aToX >= 0 );
        assert( aFromY >= 0 );
        assert( aToY >= 0 );
        assert( aFromX < aSourceCanvas.PixelWidth( ) );
        assert( aToX < aSourceCanvas.PixelWidth( ) );
        assert( aFromY < aSourceCanvas.PixelHeight( ) );
        assert( aToY < aSourceCanvas.PixelHeight( ) );

        const auto sourceWidth = std::abs( aToX - aFromX ) + 1;
        const auto sourceHeight = std::abs( aToY - aFromY ) + 1;

        const Position startPosition = GetPosition( );
        const auto pixelWidth = std::min( PixelHeight( ), sourceWidth );
        const auto pixelHeight = std::min( PixelHeight( ), sourceHeight );

        int targetY = startPosition.iY;
        int sourceY = aFromY;

        for ( ; targetY < pixelHeight; ++targetY, ++sourceY )
        {
            int targetX = startPosition.iX;
            int sourceX = aFromX;
            for ( ; targetX < pixelWidth; ++targetX, ++sourceX )
            {
                SetPosition( targetX, targetY );
                aSourceCanvas.SetPosition( sourceX, sourceY );
                SetPixel( aSourceCanvas.GetPixel( ) );
            }
        }
    }
};

template < typename taPixelValue,
           PlottingOrigin taPlottingOrigin = PlottingOrigin::BottomLeftCorner >
class CDrawer
{
public:
    using TAbstractCanvas = class TAbstractCanvas< taPixelValue, taPlottingOrigin >;
    using TPixel = taPixelValue;
    static constexpr auto KPlottingOrigin = taPlottingOrigin;

    CDrawer( TAbstractCanvas& aCanvas )
        : iCanvas{ aCanvas }
    {
    }

    /**
     * @brief Draws a line from point (aFromX, aFromY) to (aToX, aToY) with a pixel value
     *        aPixelValue
     *
     * @param aFromX An x coordinate of the line origin.
     * @param aFromY An y coordinate of the line origin.
     * @param aToX An x coordinate of the line destination.
     * @param aToY An y coordinate of the line destination.
     * @param aPixelValue A pixel value.
     */
    void
    DrawLine( int aFromX, int aFromY, int aToX, int aToY, TPixel aPixelValue = TPixel{ true } )
    {
        assert( aFromX >= 0 );
        assert( aToX >= 0 );
        assert( aFromY >= 0 );
        assert( aToY >= 0 );
        assert( aFromX < iCanvas.PixelWidth( ) );
        assert( aToX < iCanvas.PixelWidth( ) );
        assert( aFromY < iCanvas.PixelHeight( ) );
        assert( aToY < iCanvas.PixelHeight( ) );

        if ( aToX < aFromX )
        {
            std::swap( aToX, aFromX );
        }
        if ( aToY < aFromY )
        {
            std::swap( aToX, aFromX );
        }

        int dx = std::abs( aToX - aFromX );
        int sx = aFromX < aToX ? 1 : -1;
        int dy = -std::abs( aToY - aFromY );
        int sy = aFromY < aToY ? 1 : -1;
        int err = dx + dy;
        int e2;

        while ( true )
        {
            iCanvas.SetPosition( aFromX, aFromY );
            iCanvas.SetPixel( aPixelValue );
            if ( aFromX == aToX && aFromY == aToY )
            {
                break;
            }
            e2 = 2 * err;

            if ( e2 >= dy )
            {
                err += dy;
                aFromX += sx;
            }
            if ( e2 <= dx )
            {
                err += dx;
                aFromY += sy;
            }
        }
    }

private:
    TAbstractCanvas& iCanvas;
};

}  // namespace AbstractPlatform
