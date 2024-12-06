#pragma once
#include <AbstractPlatform/common/Platform.hpp>

namespace AbstractPlatform
{
using TErrorCode = int;
static constexpr TErrorCode KOk = 0;
static constexpr TErrorCode KGenericError = -1;
static constexpr TErrorCode KInvalidArgumentError = -2;
static constexpr TErrorCode KInvalidVendor = -3;

#define RETURN_ON_ERROR( aErrorCode )      \
    if ( auto errorCode = ( aErrorCode ) ) \
    {                                      \
        return errorCode;                  \
    }

#ifdef __EXCEPTIONS
template < int taError, const char* const taDescription >
class EBase : public std::exception
{
public:
    constexpr int
    error( ) const NOEXCEPT
    {
        return taError;
    }

    const char*
    what( ) const NOEXCEPT override
    {
        return taDescription;
    }
};
static constexpr char KGenericErrorDescription[] = "Internal error";
static constexpr char KInvalidArgumentDescription[] = "Invalid argument";
static constexpr char KInvalidVendorDescription[] = "Invalid vendor";
using EGenericError = EBase< KGenericError, KGenericErrorDescription >;
using EInvalidArgumentError = EBase< KInvalidArgumentError, KInvalidArgumentDescription >;
using EInvalidVendorError = EBase< KInvalidVendor, KInvalidVendorDescription >;

static inline int
ThrowOnError( int aErrorCode )
{
    switch ( aErrorCode )
    {
    case KOk:
        return aErrorCode;
        break;
    case KInvalidArgumentError:
        throw EInvalidArgumentError{ };
        break;
    case KInvalidVendor:
        throw EInvalidVendorError{ };
        break;
    case KGenericError:
    default:
        throw EGenericError{ };
        break;
    }
    return aErrorCode;
}
#endif
}  // namespace AbstractPlatform