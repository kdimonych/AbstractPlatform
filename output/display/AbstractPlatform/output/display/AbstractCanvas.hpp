#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <cstdint>
#include <cassert>
#include <memory>

namespace AbstractPlatform
{

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

template < typename taCanvasImpl >
class TAbstractReadOnlyCanvas
{
public:
    using TCanvasImpl = taCanvasImpl;
    using TPixel = typename TCanvasImpl::TPixel;

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    inline int
    PixelWidth( ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->PixelWidthImpl( );
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    inline int
    PixelHeight( ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->PixelHeightImpl( );
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @param aPosition The {x, y} coordinates of the pixel.
     * @return TPixel The value of the pixel.
     */
    inline TPixel
    GetPixel( const TPosition& aPosition ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->GetPixelImpl( aPosition );
    }

    /**
     * @brief Gets the implementation defined RAW buffer pointer for read-only operations.
     *
     * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    inline const std::uint8_t*
    GetRawBuffer( ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->GetRawBufferImpl( );
    }

    /**
     * @brief Gets the implementation defined RAW buffer size in bytes.
     *
     * @return The implementation-defined RAW buffer size in bytes. Returns 0ul in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    inline size_t
    GetRawBufferSize( ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->GetRawBufferSize( );
    }

protected:
    inline TPixel
    GetPixelImpl( const TPosition& aPosition ) const NOEXCEPT
    {
        return static_cast< const TCanvasImpl* >( this )->GetPixelImpl(
            static_cast< const TCanvasImpl* >( this )->GetCanvasPostionImpl( aPosition ) );
    }
};

template < typename taCanvasImpl >
class TAbstractCanvas : public TAbstractReadOnlyCanvas< taCanvasImpl >
{
public:
    using TCanvasImpl = taCanvasImpl;
    using TPixel = typename TCanvasImpl::TPixel;

    /**
     * @brief Sets a pixel value located at the current coordinates.
     *
     * @param aPosition The {x, y} coordinates of the pixel.
     * @param TPixel A pixel value to set.
     *
     */
    inline void
    SetPixel( const TPosition& aPosition, TPixel aPixelValue ) noexcept
    {
        return static_cast< TCanvasImpl* >( this )->SetPixel( aPosition, aPixelValue );
    }

    /**
     * @brief Fills entire canvas with provided pixel value.
     *
     * @param TPixel A pixel value to fill the canvas with.
     */
    inline void
    FillWith( TPixel aPixelValue ) NOEXCEPT
    {
        static_cast< TCanvasImpl* >( this )->FillWithImpl( aPixelValue );
    }

    /**
     * @brief Clears the canvas
     */
    inline void
    Clear( ) NOEXCEPT
    {
        static_cast< TCanvasImpl* >( this )->FillWithImpl( TPixel{ } );
    }

    inline void
    MergeCanvas( const TPosition& aStartPosition,
                 TCanvasImpl& aSourceCanvas,
                 const TPosition& aSourceFrom,
                 const TPosition& aSourceTo ) NOEXCEPT
    {
        static_cast< TCanvasImpl* >( this )->MergeCanvasImpl( aStartPosition, aSourceCanvas,
                                                              aSourceFrom, aSourceTo );
    }

    /**
     * @brief Gets the implementation defined RAW buffer pointer for read/write operations.
     *
     * @return The implementation-defined RAW buffer pointer. Returns nullptr in case the buffer
     * is not accessible for implementation or not ready for use.
     */
    inline std::uint8_t*
    GetRawBuffer( ) NOEXCEPT
    {
        static_cast< TCanvasImpl* >( this )->GetRawBufferImpl( );
    }

protected:
    inline void
    FillWithImpl( TPixel aPixelValue ) NOEXCEPT
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

    inline void
    MergeCanvasImpl( const TPosition& aStartPosition,
                     TCanvasImpl& aSourceCanvas,
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
                SetPixel( { targetX, targetY }, aSourceCanvas.GetPixel( { sourceX, sourceY } ) );
            }
        }
    }
};

}  // namespace AbstractPlatform
