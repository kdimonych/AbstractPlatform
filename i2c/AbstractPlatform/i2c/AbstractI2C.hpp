#pragma once

#include <AbstractPlatform/common/Platform.hpp>
#include <AbstractPlatform/common/PlatformLiteral.hpp>
#include <AbstractPlatform/common/Memory.hpp>
#include <AbstractPlatform/common/ErrorCode.hpp>

#include <cstdint>
#include <memory>

namespace AbstractPlatform
{
/// @brief I2C bus interface
class IAbstractI2CBus
{
public:
    virtual ~IAbstractI2CBus( ) = default;

    /**
     * @brief Attempt to write specified number of bytes to address, blocking.
     *
     * @param aDeviceAddress 7-bit address of device to write to.
     * @param aDataSource Not null pointer to data to send.
     * @param aDataLength Length of data in bytes to send.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return int Number of bytes written, or AbstractPlatform::KGenericError if address not
     * acknowledged, no device present.
     * Note: The aDataSource buffer's guaranteed lifetime is no longer than the Write() call.
     */
    virtual int Write( std::uint8_t aDeviceAddress,
                       const std::uint8_t* aDataSource,
                       size_t aDataLength,
                       bool aNoStop ) NOEXCEPT = 0;

    /**
     * @brief Attempt to read specified number of bytes from address, blocking.
     *
     * @param aDeviceAddress 7-bit address of device to read from.
     * @param aDataDestination Not null pointer to buffer to receive data.
     * @param aDataLength Length of data in bytes to receive.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return Number of bytes read, or AbstractPlatform::KGenericError if address not acknowledged
     * or no device present.
     * Note: The aDataDestination buffer's guaranteed lifetime is no longer than the Write() call.
     */
    virtual int Read( std::uint8_t aDeviceAddress,
                      std::uint8_t* aDataDestination,
                      size_t aDataLength,
                      bool aNoStop ) NOEXCEPT = 0;
};

/**
 * @brief The CI2CBus represents the i2c bus to communicate with.
 *
 */
struct CI2CBus
{
    /**
     * @brief Creates CI2CBus bus class out of the IAbstractI2CBus reference.
     *
     * @param aI2CBus The reference to the i2c bus interface implementation.
     */
    constexpr CI2CBus( IAbstractI2CBus& aI2CBus )
        : iI2CBus{ aI2CBus }
    {
    }

    /**
     * @brief Attempt to write specified number of bytes to address, blocking.
     *
     * @param aDeviceAddress 7-bit address of device to write to.
     * @param aDataSource Pointer to data to send.
     * @param aDataLength Length of data in bytes to send.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return int Number of bytes written, or AbstractPlatform::KGenericError if address not
     * acknowledged, no device present.
     */
    inline int
    Write( std::uint8_t aDeviceAddress,
           const std::uint8_t* aDataSource,
           size_t aDataLength,
           bool aNoStop ) NOEXCEPT
    {
        return iI2CBus.Write( aDeviceAddress, aDataSource, aDataLength, aNoStop );
    }

    /**
     * @brief Attempt to read specified number of bytes from address, blocking.
     *
     * @param aDeviceAddress 7-bit address of device to read from.
     * @param aDataDestination Pointer to buffer to receive data.
     * @param aDataLength Length of data in bytes to receive.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return Number of bytes read, or AbstractPlatform::KGenericError if address not acknowledged
     * or no device present.
     */
    inline int
    Read( std::uint8_t aDeviceAddress,
          std::uint8_t* aDataDestination,
          size_t aDataLength,
          bool aNoStop ) NOEXCEPT
    {
        return iI2CBus.Read( aDeviceAddress, aDataDestination, aDataLength, aNoStop );
    }

    /**
     * @brief Reads the register from the previously used register address
     *
     * @tparam taTRegister The type of the register receiving variable.
     * @param aDeviceAddress 7-bit address of device to write to.
     * @param aRegisterValue The register receiving variable.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return true If operation succeed, otherwise - false
     */
    template < typename taTRegister >
    inline bool
    ReadLastRegisterRaw( std::uint8_t aDeviceAddress,
                         taTRegister& aRegisterValue,
                         bool aNoStop = false ) NOEXCEPT
    {
        return iI2CBus.Read( aDeviceAddress, reinterpret_cast< std::uint8_t* >( &aRegisterValue ),
                             sizeof( aRegisterValue ), aNoStop )
               != sizeof( aRegisterValue );
    }

    /**
     * @brief Reads the register from the I2C device.
     *
     * @tparam taTRegisterAddress The type of the register address value.
     * @tparam taTRegister The type of the register receiving variable.
     * @param aDeviceAddress 7-bit address of device to write to.
     * @param aRegisterAddress The register address.
     * @param aRegisterValue The register receiving variable.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return true If operation succeed, otherwise - false
     */
    template < typename taTRegisterAddress, typename taTRegister >
    bool
    ReadRegisterRaw( std::uint8_t aDeviceAddress,
                     taTRegisterAddress aRegisterAddress,
                     taTRegister& aRegisterValue,
                     bool aNoStop = false ) NOEXCEPT
    {
        return ( iI2CBus.Write( aDeviceAddress, &aRegisterAddress, sizeof( aRegisterAddress ),
                                true )
                 == sizeof( aRegisterAddress ) )
               && ( ReadLastRegisterRaw( aDeviceAddress, aRegisterValue, aNoStop )
                    != sizeof( aRegisterValue ) );
    }

    /**
     * @brief Writes the register to the I2C device.
     *
     * @tparam taTRegisterAddress The type of the register address value.
     * @tparam taTRegister The type of the register source variable.
     * @param aDeviceAddress 7-bit address of device to write to.
     * @param aRegisterAddress The register address.
     * @param aRegisterValue The register source variable.
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return true If operation succeed, otherwise - false
     */
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

        return iI2CBus.Write( aDeviceAddress, dataPack, sizeof( dataPack ), aNoStop )
               != sizeof( dataPack );
    }

    IAbstractI2CBus& iI2CBus;
};

}  // namespace AbstractPlatform