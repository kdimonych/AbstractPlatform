#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/AbstractDisplay.hpp>

#include <cstdint>
#include <cmath>
#include <cassert>

namespace AbstractPlatform
{

template < typename taCanvas >
class CDrawer
{
public:
    using TCanvas = taCanvas;
    using TAbstractCanvas = typename TAbstractCanvas< TCanvas >;
    using TPixel = TCanvas::TPixel;

    TPixel iPixelValue;

    CDrawer( TAbstractCanvas& aCanvas )
        : iPixelValue{ }
        , iCanvas{ aCanvas }
        , iPosition{ }
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
    Fill( )
    {
        iCanvas.FillWith( iPixelValue );
    }

    template < typename taPosition >
    void
    SetPosition( taPosition&& aPosition )
    {
        assert( aPosition.iX >= 0 );
        assert( aPosition.iY >= 0 );
        assert( aPosition.iX < iCanvas.PixelWidth( ) );
        assert( aPosition.iY < iCanvas.PixelHeight( ) );

        iPosition = std::forward< taPosition >( aPosition );
    }

    TPosition
    GetPosition( ) const
    {
        return iPosition;
    }

    template < typename taPosition >
    void
    SetPixelValue( taPosition&& aPosition )
    {
        assert( aPosition.iX >= 0 );
        assert( aPosition.iY >= 0 );
        assert( aPosition.iX < iCanvas.PixelWidth( ) );
        assert( aPosition.iY < iCanvas.PixelHeight( ) );

        iPosition = std::forward< taPosition >( aPosition );
    }

    TPosition
    GetPosition( ) const
    {
        return iPosition;
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
    DrawLineTo( const TPosition& aToPosition )
    {
        assert( aToPosition.iX >= 0 );
        assert( aToPosition.iY >= 0 );
        assert( aToPosition.iX < iCanvas.PixelWidth( ) );
        assert( aToPosition.iY < iCanvas.PixelHeight( ) );

        if ( aToPosition.iX < iPosition.iX )
        {
            std::swap( aToPosition.iX, iPosition.iX );
        }
        if ( aToPosition.iY < iPosition.iY )
        {
            std::swap( aToPosition.iX, iPosition.iX );
        }

        int dx = std::abs( aToPosition.iX - iPosition.iX );
        int sx = iPosition.iX < aToPosition.iX ? 1 : -1;
        int dy = -std::abs( aToPosition.iY - iPosition.iY );
        int sy = iPosition.iY < aToPosition.iY ? 1 : -1;
        int err = dx + dy;
        int e2;

        while ( true )
        {
            iCanvas.SetPixel( iPosition, iPixelValue );
            if ( iPosition.iX == aToPosition.iX && iPosition.iY == aToPosition.iY )
            {
                break;
            }
            e2 = 2 * err;

            if ( e2 >= dy )
            {
                err += dy;
                iPosition.iX += sx;
            }
            if ( e2 <= dx )
            {
                err += dx;
                iPosition.iY += sy;
            }
        }
    }

private:
    TAbstractCanvas& iCanvas;
    TPosition iPosition;
};

template < typename taPixelValue >
static constexpr CDrawer< taPixelValue >
CreateDrawer( TAbstractCanvas< taPixelValue >& aCanvas )
{
    return CDrawer< taPixelValue >( aCanvas );
}

}  // namespace AbstractPlatform