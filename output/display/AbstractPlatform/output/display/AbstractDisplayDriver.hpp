#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/output/display/Pixel.hpp>
#include <AbstractPlatform/output/display/AbstractCanvas.hpp>
#include <AbstractPlatform/output/display/Drawer.hpp>
#include <cstdint>
#include <cassert>
#include <memory>

namespace AbstractPlatform
{

template < typename taPixelValue >
class AbstarctDisplayDriver;

template < typename taPixelValue >
class TCanvas
{
public:
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

private:
    friend class AbstarctDisplayDriver< taPixelValue >;
    std::unique_ptr< TAbstractCanvas > iImpl;
};

template < typename taPixelValue >
class TPixelMask : public TCanvas< taPixelValue >
{
public:
private:
    friend class AbstarctDisplayDriver< taPixelValue >;
};

template < typename taPixelValue >
class AbstarctDisplayDriver
{
public:
    using TCanvas = typename TCanvas< taPixelValue >;
    // enum CanvasType
    // {
    //     Canvas,
    //     TransparencyMask
    // };

    struct BlendData
    {
        TCanvas iCanvas;
        TPosition iPosition;
    };

    virtual ~AbstarctDisplayDriver( ) = default;

    virtual TCanvas CreateCanvas( size_t aWidth, size_t aHeight ) const NOEXCEPT = 0;
    virtual TPixelMask CreatePixelMask( ) const NOEXCEPT = 0;
    virtual TCanvas Blend( std::vector< const BlendData > aCanvases ) NOEXCEPT = 0;
    virtual void Show( TCanvas aCanvas ) NOEXCEPT = 0;
};

}  // namespace AbstractPlatform
