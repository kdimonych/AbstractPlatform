#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>
#include <AbstractPlatform/output/display/AbstractDisplay.hpp>

#include <cstdint>
#include <cmath>

namespace AbstractPlatform
{

template < typename taPixelValue >
class CDrawer
{
public:
    using TAbstractCanvas = class TAbstractCanvas< taPixelValue >;
    using TPixel = taPixelValue;

    CDrawer( TAbstractCanvas& aCanvas )
        : iCanvas{ aCanvas }
    {
    }

    /**
     * @brief Clear the canvas via set all its pixel values to the default value.
     */
    inline void
    Clear( )
    {
        iCanvas.Clear( );
    }

    /**
     * @brief Fills entire canvas with provided pixel value.
     *
     * @param TPixel A pixel value to fill the canvas with.
     */
    void
    FillWith( TPixel aPixelValue )
    {
        iCanvas.FillWith( aPixelValue );
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

template < typename taPixelValue >
static constexpr CDrawer< taPixelValue >
CreateDrawer( TAbstractCanvas< taPixelValue >& aCanvas )
{
    return CDrawer< taPixelValue >( aCanvas );
}

}  // namespace AbstractPlatform