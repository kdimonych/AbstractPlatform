#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <cstdint>
#include <cassert>

namespace AbstractPlatform
{

struct TBitPixel
{
    constexpr TBitPixel( ) = default;
    constexpr TBitPixel( bool aPixelValue )
        : iPixelValue{ aPixelValue }
    {
    }

    constexpr operator bool( ) const
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
    int iX = 0;
    int iY = 0;
};

enum class TPlottingOrigin
{
    TopLeftCorner,
    BottomLeftCorner
};

class TAbstractCanvasNavigation
{
public:
    virtual ~TAbstractCanvasNavigation( ) = default;

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
     * @brief Set the current coordinate of the modification start from, e.g. the pixel to modify.
     *
     * @param aX x coordinate of the pixel.
     * @param aY y coordinate of the pixel.
     *
     * Note: the calling side controls the ranges of both aX and aY values. To preserve performance,
     * the interface implementation is not expected to check both aX and aY values.
     */
    virtual void SetPosition( int aX, int aY ) NOEXCEPT = 0;

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return TPosition Current coordinate
     */
    virtual TPosition GetPosition( ) const NOEXCEPT = 0;
};

template < typename taPixelValue >
class TAbstractReadOnlyCanvas : virtual public TAbstractCanvasNavigation
{
public:
    using TPixel = taPixelValue;

    virtual ~TAbstractReadOnlyCanvas( ) = default;

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @return TPixel The value of the pixel.
     */
    virtual TPixel GetPixel( ) const NOEXCEPT = 0;
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
     * @param TPixel A pixel value to set.
     *
     */
    virtual void SetPixel( TPixel aPixelValue ) NOEXCEPT = 0;

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
    MergeCanvas( TAbstractReadOnlyCanvas& aSourceCanvas,
                 int aFromX,
                 int aFromY,
                 int aToX,
                 int aToY ) NOEXCEPT
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

        const TPosition startPosition = this->GetPosition( );
        const auto pixelWidth = std::min( this->PixelHeight( ), sourceWidth );
        const auto pixelHeight = std::min( this->PixelHeight( ), sourceHeight );

        int targetY = startPosition.iY;
        int sourceY = aFromY;

        for ( ; targetY < pixelHeight; ++targetY, ++sourceY )
        {
            int targetX = startPosition.iX;
            int sourceX = aFromX;
            for ( ; targetX < pixelWidth; ++targetX, ++sourceX )
            {
                this->SetPosition( targetX, targetY );
                aSourceCanvas.SetPosition( sourceX, sourceY );
                SetPixel( aSourceCanvas.GetPixel( ) );
            }
        }
    }
};

}  // namespace AbstractPlatform
