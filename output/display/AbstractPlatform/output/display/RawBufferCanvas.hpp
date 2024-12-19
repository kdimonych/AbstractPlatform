#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>
#include <AbstractPlatform/output/display/AbstractDisplay.hpp>
#include <AbstractPlatform/common/BinaryOperations.hpp>

#include <cstdint>

namespace AbstractPlatform
{

/**
 * @brief Determines geometric pixel layout across a page
 */
enum class PixelLayout
{
    /**
     * @brief The vertical pixel layout
     *   p0    p1  ...  pm
     *  [px0] [px0]    [px0]
     *  [px1] [px1]    [px1]
     *   ...
     *  [pxn] [pxn]    [pxn]
     *
     *   Where p* - page, px* - pixel, n - last pixel index in the page, m - last page index in the
     * block
     */
    Vertical,

    /**
     * @brief The horizontal pixel layout
     *   p0 [px0] [px1] ... [pxn]
     *   p1 [px0] [px1] ... [pxn]
     *   ...
     *   pm [px0] [px1] ... [pxn]
     *
     *   Where p* - page, px* - pixel, n - last pixel index in the page, m - last page index in the
     * block
     */
    Horizontal
};

/**
 * @brief Determines geometric page layout across a screen
 */
enum class PageLayout
{
    /**
     * @brief The vertical page layout
     *    [pg_0]   [pg_(n-1)*1+1]  ... [pg_(n-1)*(m-1)+1]
     *    [pg_1]   [pg_(n-1)*1+2]  ... [pg_(n-1)*(m-1)+2]
     *     ...          ...                    ...
     *  [pg_(n-1)] [pg_(n-1)*1+n]  ... [pg_(n-1)*(m-1)+n]
     *  |      -
     *  |    / |
     *  |  |   |
     *  | /    |
     *  V      V
     *  Where n - number of pages in a line, m - number of lines
     */
    Vertical,
    /**
     * @brief The horizontal page layout
     *       [pg_0]             [pg_1]       ...     [pg_(i-1)]
     *   [pg_(i-1)*1+1]    [pg_(i-1)*1+2]    ...   [pg_(i-1)*1+i]
     *        ...                ...                    ...
     * [pg_(i-1)*(k-1)+1] [pg_(i-1)*(k-1)+2] ... [pg_(i-1)*(k-1)+n]
     *
     *  ----->
     *     /
     *   /
     *  V
     *  ----->
     *  Where i - number of pages in a line, m - number of lines
     */
    Horizontal
};

template < typename taPixelValue, typename taPageType >
struct PageHelper
{
};

template < typename taPageType >
struct PageHelper< TBitPixel, taPageType >
{
    /**
     * @brief Returns number of pixel per page.
     *
     * @return constexpr size_t Number of pixel per page.
     */
    static constexpr size_t
    PixelsPerPage( )
    {
        return sizeof( taPageType ) * 8 / TBitPixel::Bits( );
    }

    /**
     * @brief Returns the pixel shift within the page.
     * Note that the calculation is based on the assumption that pages are arranged as a simple
     * grid.
     *
     *            [+][+][+]        [+][+][+]
     * Like this: [+][+][+], not a  [+][+][+] !!
     *            [+][+][+]        [+][+][+]
     *
     * @param aPixelLayout Pixels layout direction
     * @param aX Pixel X coordinate
     * @param aY Pixel Y coordinate
     * @return constexpr size_t The pixel shift within the page.
     */
    static constexpr size_t
    PixelShift( PixelLayout aPixelLayout, int aX, int aY )
    {
        return aPixelLayout == PixelLayout::Horizontal ? PixelsPerPage( ) - aX % PixelsPerPage( )
                                                       : aY % PixelsPerPage( );
    }
};

template < typename taPageType >
struct PageHelper< TRGBPixel, taPageType >
{
    /**
     * @brief Returns number of pixel per page.
     *
     * @return constexpr size_t Number of pixel per page.
     */
    static constexpr size_t
    PixelsPerPage( )
    {
        return sizeof( taPageType ) * 8 / TRGBPixel::Bits( );
    }

    /**
     * @brief Returns the pixel shift within the page.
     * Note that the calculation is based on the assumption that pages are arranged as a simple
     * grid.
     *
     *            [+][+][+]        [+][+][+]
     * Like this: [+][+][+], not a  [+][+][+] !!
     *            [+][+][+]        [+][+][+]
     *
     * @param aPixelLayout Pixels layout direction
     * @param aX Pixel X coordinate
     * @param aY Pixel Y coordinate
     * @return constexpr size_t The pixel shift within the page.
     */
    static constexpr size_t
    PixelShift( PixelLayout aPixelLayout, int aX, int aY )
    {
        return aPixelLayout == PixelLayout::Horizontal ? PixelsPerPage( ) - aX % PixelsPerPage( )
                                                       : aY % PixelsPerPage( );
    }
};

struct TBitmapVerticalLayout
{
    static constexpr auto KPixelLayout = PixelLayout::Vertical;
    using TPixel = TBitPixel;

    TBitmapVerticalLayout( size_t aBufferSize, size_t aPixelWidth )
        : iBufferSize{ aBufferSize }
        , iPixelWidth{ aPixelWidth }
        , iCurrentPageIndex{ 0 }
        , iCurrentPagePixelBitIndex{ 0 }
    {
        assert( ( iBufferSize * KPixelsPerPage ) % iPixelWidth == 0 );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    constexpr int
    PixelWidth( ) const NOEXCEPT
    {
        return iPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    constexpr int
    PixelHeight( ) const NOEXCEPT
    {
        return ( iBufferSize * KPixelsPerPage ) / iPixelWidth;
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
    constexpr void
    SetPosition( int aX, int aY ) NOEXCEPT
    {
        assert( aX >= 0 );
        assert( aY >= 0 );
        assert( aX < PixelWidth( ) );
        assert( aY < PixelHeight( ) );

        iCurrentPageIndex = GetPageIndexByPixelCoordinate( iPixelWidth, aX, aY );
        iCurrentPagePixelBitIndex = PageHelper< TPixel, TPage >::PixelShift( KPixelLayout, aX, aY );
    }

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return TPosition Current coordinate
     */
    inline TPosition
    GetPosition( ) const NOEXCEPT
    {
        const size_t x = iCurrentPageIndex % iPixelWidth;
        const size_t y
            = iCurrentPageIndex / iPixelWidth * KPixelsPerPage + iCurrentPagePixelBitIndex;
        const TPosition result{ static_cast< int >( x ), static_cast< int >( y ) };
        return result;
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @param aBuffer A buffer pointer to get the pixel value from.
     * @return TPixel The value of the pixel.
     */
    inline TPixel
    GetPixel( const std::uint8_t* aBuffer ) const NOEXCEPT
    {
        assert( aBuffer != nullptr );

        const auto page = static_cast< TPage >( aBuffer[ iCurrentPageIndex ] );
        return TPixel{ CheckBit( page, iCurrentPagePixelBitIndex ) };
    }

    /**
     * @brief Sets a pixel value located at the current coordinates.
     *
     * @param aBuffer A buffer pointer to set the pixel value to.
     * @param aPixelValue A pixel value to set.
     */
    inline void
    SetPixel( std::uint8_t* aBuffer, TPixel aPixelValue ) NOEXCEPT
    {
        assert( aBuffer != nullptr );

        auto& page = aBuffer[ iCurrentPageIndex ];
        page = aPixelValue.iPixelValue ? SetBit( page, iCurrentPagePixelBitIndex )
                                       : ClearBit( page, iCurrentPagePixelBitIndex );
    }

private:
    using TPage = std::uint8_t;
    static constexpr auto KPixelsPerPage = PageHelper< TPixel, TPage >::PixelsPerPage( );

    static constexpr size_t
    GetPageIndexByPixelCoordinate( size_t aColumns, size_t aX, size_t aY )
    {
        return ( aY / KPixelsPerPage ) * aColumns + aX;
    }

    const size_t iBufferSize;
    const size_t iPixelWidth;
    size_t iCurrentPageIndex;
    std::uint8_t iCurrentPagePixelBitIndex;
};

struct TBitmapHorizontalLayout
{
    static constexpr auto KPixelLayout = PixelLayout::Horizontal;
    using TPixel = TBitPixel;

    TBitmapHorizontalLayout( size_t aBufferSize, size_t aPixelWidth )
        : iBufferSize{ aBufferSize }
        , iPixelWidth{ aPixelWidth }
        , iCurrentPageIndex{ 0 }
        , iCurrentPagePixelBitIndex{ 0 }
    {
        assert( ( iBufferSize * KPixelsPerPage ) % iPixelWidth == 0 );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    constexpr int
    PixelWidth( ) const NOEXCEPT
    {
        return iPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    constexpr int
    PixelHeight( ) const NOEXCEPT
    {
        return ( iBufferSize * KPixelsPerPage ) / iPixelWidth;
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
    constexpr void
    SetPosition( int aX, int aY ) NOEXCEPT
    {
        assert( aX >= 0 );
        assert( aY >= 0 );
        assert( aX < PixelWidth( ) );
        assert( aY < PixelHeight( ) );

        iCurrentPageIndex = aX / KPixelsPerPage + ( aY * PagesPerLine( ) );
        iCurrentPagePixelBitIndex = PageHelper< TPixel, TPage >::PixelShift( KPixelLayout, aX, aY );
    }

    /**
     * @brief Get the Position the current coordinate of the modification start from, e.g. the
     * pixel to modify.
     *
     * @return TPosition Current coordinate
     */
    inline TPosition
    GetPosition( ) const NOEXCEPT
    {
        auto dx
            = PageHelper< TPixel, TPage >::PixelShift( KPixelLayout, iCurrentPagePixelBitIndex, 0 );

        const size_t x = iCurrentPageIndex % PagesPerLine( ) * KPixelsPerPage + dx;
        const size_t y = iCurrentPageIndex / PagesPerLine( );
        const TPosition result{ static_cast< int >( x ), static_cast< int >( y ) };
        return result;
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @param aBuffer A buffer pointer to get the pixel value from.
     * @return TPixel The value of the pixel.
     */
    inline TPixel
    GetPixel( const std::uint8_t* aBuffer ) const NOEXCEPT
    {
        assert( aBuffer != nullptr );

        const auto page = static_cast< TPage >( aBuffer[ iCurrentPageIndex ] );
        return TPixel{ CheckBit( page, iCurrentPagePixelBitIndex ) };
    }

    /**
     * @brief Sets a pixel value located at the current coordinates.
     *
     * @param aBuffer A buffer pointer to set the pixel value to.
     * @param aPixelValue A pixel value to set.
     */
    inline void
    SetPixel( std::uint8_t* aBuffer, TPixel aPixelValue ) NOEXCEPT
    {
        assert( aBuffer != nullptr );

        auto& page = aBuffer[ iCurrentPageIndex ];
        page = aPixelValue.iPixelValue ? SetBit( page, iCurrentPagePixelBitIndex )
                                       : ClearBit( page, iCurrentPagePixelBitIndex );
    }

private:
    using TPage = std::uint8_t;
    static constexpr auto KPixelsPerPage = PageHelper< TPixel, TPage >::PixelsPerPage( );

    static constexpr size_t
    GetPageIndexByPixelCoordinate( size_t aColumns, size_t aX, size_t aY )
    {
        return ( aY / KPixelsPerPage ) * aColumns + aX;
    }

    constexpr size_t
    PagesPerLine( ) const NOEXCEPT
    {
        return ( iPixelWidth + KPixelsPerPage - 1 ) / KPixelsPerPage;
    }

    const size_t iBufferSize;
    const size_t iPixelWidth;
    size_t iCurrentPageIndex;
    std::uint8_t iCurrentPagePixelBitIndex;
};

template < typename taPixelValue,
           PixelLayout taPixelLayout,
           PageLayout taPageLayout,
           typename taPageType >
class TRawBufferNavigationView
{
};

template < PixelLayout taPixelLayout, PageLayout taPageLayout, typename taPageType >
class TRawBufferNavigationView< TBitPixel, taPixelLayout, taPageLayout, taPageType >
    : virtual TAbstractCanvasNavigation< TBitPixel >
{
    using TPage = taPageType;
    static constexpr std::uint8_t KPixelsPerPage
        = PageHelper< TBitPixel, taPageType >::PixelsPerPage( );

public:
    using TPixel = typename TAbstractReadOnlyCanvas< TBitPixel >::TPixel;

    virtual ~TRawBufferNavigationView( ) = default;
    TRawBufferNavigationView( size_t aBufferSize, size_t aRowPixelWidth ),
        iBufferSize{ aBufferSize }, iRowPixelWidth{ aRowPixelWidth }, iCurrentPageIndex{ 0 },
        iCurrentPagePixelBitIndex{ 0 }
    {
        assert( iBufferSize == 0 || iBuffer != nullptr );
        assert( iBufferSize % iRowPixelWidth == 0 );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    int
    PixelWidth( ) const NOEXCEPT override
    {
        return iRowPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int A pixel height of the canvas.
     */
    int
    PixelHeight( ) const NOEXCEPT override
    {
        return iBufferSize / iRowPixelWidth;
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
    void
    SetPosition( int aX, int aY ) NOEXCEPT override
    {
        assert( aX >= 0 );
        assert( aY >= 0 );
        assert( aX < PixelWidth( ) );
        assert( aY < PixelHeight( ) );

        iCurrentPageIndex = GetPageIndexByPixelCoordinate( iRowPixelWidth, aX, aY );
        iCurrentPagePixelBitIndex = GetPagePixelBitIndexByPixelYCoordinate( aY );
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
        const size_t x = iCurrentPageIndex % iRowPixelWidth;
        const size_t y
            = iCurrentPageIndex / iRowPixelWidth * KPixelsPerPage + iCurrentPagePixelBitIndex;
        const TPosition result{ static_cast< int >( x ), static_cast< int >( y ) };
        return result;
    }

private:
    static constexpr size_t
    GetPageIndexByPixelCoordinate( size_t aRowPixelWidth, size_t aX, size_t aY )
    {
        return ( aY / KPixelsPerPage ) * aRowPixelWidth + aX;
    }

    static constexpr TPage
    GetPagePixelBitIndexByPixelYCoordinate( size_t aY )
    {
        return aY % KPixelsPerPage;
    }

    const size_t iBufferSize;
    const size_t iRowPixelWidth;
    size_t iCurrentPageIndex;
    std::uint8_t iCurrentPagePixelBitIndex;
};

template < typename taPixelValue,
           PixelLayout taPixelLayout,
           PageLayout taPageLayout,
           typename taPageType = std::uint8_t >
class TRawBufferView
{
};

template < PixelLayout taPixelLayout, PageLayout taPageLayout, typename taPageType >
class TRawBufferView< TBitPixel, taPixelLayout, taPageLayout, taPageType >
    : TAbstractReadOnlyCanvas< TBitPixel >
{
    using TPage = taPageType;
    static constexpr std::uint8_t KPixelsPerPage = sizeof( TPage );

public:
    using TPixel = typename TAbstractReadOnlyCanvas< TBitPixel >::TPixel;

    virtual ~TRawBufferView( ) = default;
    TRawBufferView( std::uint8_t* aBuffer, size_t aBufferSize, size_t aRowPixelWidth )
        : iBuffer{ aBuffer }
        , iBufferSize{ aBufferSize }
        , iRowPixelWidth{ aRowPixelWidth }
        , iCurrentPageIndex{ 0 }
        , iCurrentPagePixelBitIndex{ 0 }
    {
        assert( iBufferSize == 0 || iBuffer != nullptr );
        assert( iBufferSize % iRowPixelWidth == 0 );
    }

    /**
     * @brief Returns pixel width of the canvas.
     *
     * @return int A pixel width of the canvas.
     */
    int
    PixelWidth( ) const NOEXCEPT override
    {
        return iRowPixelWidth;
    }

    /**
     * @brief Returns pixel height of the canvas.
     *
     * @return int
     */
    int
    PixelHeight( ) const NOEXCEPT override
    {
        return iBufferSize / iRowPixelWidth;
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
    void
    SetPosition( int aX, int aY ) NOEXCEPT override
    {
        assert( aX >= 0 );
        assert( aY >= 0 );
        assert( aX < PixelWidth( ) );
        assert( aY < PixelHeight( ) );

        iCurrentPageIndex = GetPageIndexByPixelCoordinate( iRowPixelWidth, aX, aY );
        iCurrentPagePixelBitIndex = GetPagePixelBitIndexByPixelYCoordinate( aY );
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
        const size_t x = iCurrentPageIndex % iRowPixelWidth;
        const size_t y
            = iCurrentPageIndex / iRowPixelWidth * KPixelsPerPage + iCurrentPagePixelBitIndex;
        const TPosition result{ static_cast< int >( x ), static_cast< int >( y ) };
        return result;
    }

    /**
     * @brief Gets a pixel value located at the current coordinates.
     *
     * @return TPixel The value of the pixel.
     */
    TPixel
    GetPixel( ) const NOEXCEPT override
    {
        using namespace AbstractPlatform;
        const auto page = iBuffer[ iCurrentPageIndex ];
        return TPixel{ CheckBit( page, iCurrentPagePixelBitIndex ) };
    }

private:
    static constexpr size_t
    GetPageIndexByPixelCoordinate( size_t aRowPixelWidth, size_t aX, size_t aY )
    {
        return ( aY / KPixelsPerPage ) * aRowPixelWidth + aX;
    }

    static constexpr TPage
    GetPagePixelBitIndexByPixelYCoordinate( size_t aY )
    {
        return aY % KPixelsPerPage;
    }

    const TPage* iBuffer;
    const size_t iBufferSize;
    const size_t iRowPixelWidth;
    size_t iCurrentPageIndex;
    std::uint8_t iCurrentPagePixelBitIndex;
};

}  // namespace AbstractPlatform