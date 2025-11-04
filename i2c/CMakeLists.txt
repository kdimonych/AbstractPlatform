cmake_minimum_required(VERSION 3.13)

project(abstract-platform.i2c)

option(ENABLE_ASSERTS_FOR_I2C "Enable asserts for abstract-platform.i2c" ON)

set(HEADER_LIST AbstractPlatform/i2c/AbstractI2C.hpp)

set(SOURCE_LIST)

# Add library
add_library(abstract-platform.i2c INTERFACE)

if(NOT ENABLE_ASSERTS_FOR_I2C)
  target_compile_definitions(abstract-platform.i2c INTERFACE NDEBUG)
endif()

# Associate the sources with the library Note: The source files are not
# compiled, as this is an interface library.
target_sources(
  abstract-platform.i2c
  INTERFACE ${HEADER_LIST}
  PUBLIC ${SOURCE_LIST})

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
  target_compile_definitions(abstract-platform.i2c INTERFACE __EXCEPTIONS)
else()
  target_compile_definitions(abstract-platform.i2c INTERFACE -fno-exceptions)
endif()

target_link_libraries(abstract-platform.i2c INTERFACE abstract-platform.common)

# Add include directory
target_include_directories(abstract-platform.i2c
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR})
