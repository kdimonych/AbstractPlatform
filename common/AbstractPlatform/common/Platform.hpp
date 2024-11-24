#pragma once

#ifdef __EXCEPTIONS
#define NOEXCEPT noexcept
#include <exception>
#else
#define NOEXCEPT
#endif