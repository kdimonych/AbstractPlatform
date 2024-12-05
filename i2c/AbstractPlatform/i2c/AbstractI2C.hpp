#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/PlatformLiteral.hpp>
#include <AbstractPlatform/common/Memory.hpp>

#include <cstdint>

namespace AbstractPlatform
{
/// @brief I2C bus interface
class IAbstractI2CBus
{
public:
    virtual ~IAbstractI2CBus( ) = default;

    /**
     * @brief Attempt to write specified number of bytes to address, blocking
     *
     * @param aDeviceAddress 7-bit address of device to write to
     * @param aDataSource Pointer to data to send
     * @param aDataLength Length of data in bytes to send
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return int Number of bytes written, or IAbstractI2CBus::KGenericError if address not
     * acknowledged, no device present.
     */
    virtual int Write( std::uint8_t aDeviceAddress,
                       const std::uint8_t* aDataSource,
                       size_t aDataLength,
                       bool aNoStop ) NOEXCEPT = 0;

    /**
     * @brief Attempt to read specified number of bytes from address, blocking
     *
     * @param aDeviceAddress 7-bit address of device to read from
     * @param aDataDestination Pointer to buffer to receive data
     * @param aDataLength Length of data in bytes to receive
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return Number of bytes read, or IAbstractI2CBus::KGenericError if address not acknowledged
     * or no device present.
     */
    virtual int Read( std::uint8_t aDeviceAddress,
                      std::uint8_t* aDataDestination,
                      size_t aDataLength,
                      bool aNoStop ) NOEXCEPT = 0;

    template < typename taTRegister >
    inline bool
    ReadLastRegisterRaw( std::uint8_t aDeviceAddress,
                         taTRegister& aRegisterValue,
                         bool aNoStop = false ) NOEXCEPT
    {
        return Read( aDeviceAddress, reinterpret_cast< std::uint8_t* >( &aRegisterValue ),
                     sizeof( aRegisterValue ), aNoStop )
               != sizeof( aRegisterValue );
    }

    /**
     * @brief Reads the register from I2C device.
     *
     * @tparam taTRegisterAddress The register address
     * @tparam taTRegister
     * @param aDeviceAddress
     * @param aRegisterAddress
     * @param aRegisterValue
     * @param aNoStop
     * @return true
     * @return false
     */
    template < typename taTRegisterAddress, typename taTRegister >
    bool
    ReadRegisterRaw( std::uint8_t aDeviceAddress,
                     taTRegisterAddress aRegisterAddress,
                     taTRegister& aRegisterValue,
                     bool aNoStop = false ) NOEXCEPT
    {
        return ( Write( aDeviceAddress, &aRegisterAddress, sizeof( aRegisterAddress ), true )
                 == sizeof( aRegisterAddress ) )
               && ( ReadLastRegisterRaw( aDeviceAddress, aRegisterValue, aNoStop )
                    != sizeof( aRegisterValue ) );
    }

    template < typename taTRegisterAddress, typename taTRegister >
    bool
    WriteRegisterRaw( std::uint8_t aDeviceAddress,
                      taTRegisterAddress aRegisterAddress,
                      taTRegister aRegisterValue,
                      bool aNoStop = false ) NOEXCEPT
    {
        std::uint8_t dataPack[ sizeof( aRegisterAddress ) + sizeof( aRegisterValue ) ];
        ScalarTypeCopy( dataPack, aRegisterAddress );
        ScalarTypeCopy( dataPack + sizeof( aRegisterAddress ), aRegisterValue );

        return Write( aDeviceAddress, dataPack, sizeof( dataPack ), aNoStop ) != sizeof( dataPack );
    }
};
}  // namespace AbstractPlatform