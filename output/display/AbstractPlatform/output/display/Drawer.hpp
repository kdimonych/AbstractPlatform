#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/AbstractDisplay.hpp>

#include <cstdint>
#include <cmath>
#include <cassert>

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
     * @brief Draws a line from point aFromPosition to aToPosition with a pixel value
     *        aPixelValue
     *
     * @param aFromPosition The {x, y} coordinates of the line origin.
     * @param aToPosition The {x, y} coordinates of the line destination.
     * @param aPixelValue A pixel value.
     */
    void
    DrawLine( TPosition aFromPosition,
              const TPosition& aToPosition,
              TPixel aPixelValue = TPixel{ true } )
    {
        assert( aFromPosition.iX >= 0 );
        assert( aToPosition.iX >= 0 );
        assert( aFromPosition.iY >= 0 );
        assert( aToPosition.iY >= 0 );
        assert( aFromPosition.iX < iCanvas.PixelWidth( ) );
        assert( aToPosition.iX < iCanvas.PixelWidth( ) );
        assert( aFromPosition.iY < iCanvas.PixelHeight( ) );
        assert( aToPosition.iY < iCanvas.PixelHeight( ) );

        if ( aToPosition.iX < aFromPosition.iX )
        {
            std::swap( aToPosition.iX, aFromPosition.iX );
        }
        if ( aToPosition.iY < aFromPosition.iY )
        {
            std::swap( aToPosition.iX, aFromPosition.iX );
        }

        int dx = std::abs( aToPosition.iX - aFromPosition.iX );
        int sx = aFromPosition.iX < aToPosition.iX ? 1 : -1;
        int dy = -std::abs( aToPosition.iY - aFromPosition.iY );
        int sy = aFromPosition.iY < aToPosition.iY ? 1 : -1;
        int err = dx + dy;
        int e2;

        while ( true )
        {
            iCanvas.SetPixel( { aFromPosition.iX, aFromPosition.iY }, aPixelValue );
            if ( aFromPosition.iX == aToPosition.iX && aFromPosition.iY == aToPosition.iY )
            {
                break;
            }
            e2 = 2 * err;

            if ( e2 >= dy )
            {
                err += dy;
                aFromPosition.iX += sx;
            }
            if ( e2 <= dx )
            {
                err += dx;
                aFromPosition.iY += sy;
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