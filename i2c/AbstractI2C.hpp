#include <cstdint>
#include "Platform.hpp"

namespace AbstractPlatform
{
class IAbstractI2CBus
{
public:
    virtual ~IAbstractI2CBus( ) = default;

    /**
     * @brief Attempt to write specified number of bytes to address, blocking
     *
     * @param aDeviceAddress 7-bit address of device to write to
     * @param aSrc Pointer to data to send
     * @param aLen Length of data in bytes to send
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return int Number of bytes written, or IAbstractI2CBus::KGenericError if address not
     * acknowledged, no device present.
     */
    virtual int Write( std::uint8_t aDeviceAddress,
                       const std::uint8_t* aSrc,
                       size_t aLen,
                       bool aNoStop ) NOEXCEPT = 0;

    /**
     * @brief Attempt to read specified number of bytes from address, blocking
     *
     * @param aDeviceAddress 7-bit address of device to read from
     * @param aDst Pointer to buffer to receive data
     * @param aLen Length of data in bytes to receive
     * @param aNoStop If true, master retains control of the bus at the end of the transfer (no Stop
     * is issued), and the next transfer will begin with a Restart rather than a Start.
     * @return Number of bytes read, or IAbstractI2CBus::KGenericError if address not acknowledged
     * or no device present.
     */
    virtual int Read( std::uint8_t aDeviceAddress,
                      std::uint8_t* aDst,
                      size_t aLen,
                      bool aNoStop ) NOEXCEPT = 0;

    template < typename taTRegister >
    inline bool
    ReadLastRegisterRaw( std::uint8_t aDeviceAddress, taTRegister& aRegisterValue ) NOEXCEPT
    {
        return Read( aDeviceAddress, reinterpret_cast< std::uint8_t* >( &aRegisterValue ),
                     sizeof( aRegisterValue ), false )
               != sizeof( aRegisterValue );
    }

    template < typename taTRegisterAddress, typename taTRegister >
    bool
    ReadRegisterRaw( std::uint8_t aDeviceAddress,
                     taTRegisterAddress aRegisterAddress,
                     taTRegister& aRegisterValue ) NOEXCEPT
    {
        return ( Write( aDeviceAddress, &aRegisterAddress, sizeof( aRegisterAddress ), true )
                 == sizeof( aRegisterAddress ) )
               && ( ReadLastRegisterRaw( aDeviceAddress, aRegisterValue )
                    != sizeof( aRegisterValue ) );
    }

    template < typename taTRegisterAddress, typename taTRegister >
    bool
    WriteRegisterRaw( std::uint8_t aDeviceAddress,
                      taTRegisterAddress aRegisterAddress,
                      taTRegister aRegisterValue ) NOEXCEPT
    {
        std::uint8_t dataPack[ sizeof( aRegisterAddress ) + sizeof( aRegisterValue ) ];
        std::memcpy( dataPack, &aRegisterAddress, sizeof( aRegisterAddress ) );
        std::memcpy( dataPack + sizeof( aRegisterAddress ), &aRegisterValue,
                     sizeof( aRegisterValue ) );

        return Write( aDeviceAddress, dataPack, sizeof( dataPack ), false ) != sizeof( dataPack );
    }
};

}  // namespace AbstractPlatform