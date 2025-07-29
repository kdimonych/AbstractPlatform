cmake_minimum_required(VERSION 3.13)

project(abstract-platform.platform CXX)

set(HEADER_LIST AbstractPlatform/platform/Platform.hpp
                AbstractPlatform/platform/PlatformLiteral.hpp)

set(SOURCE_LIST)

# Add library cpp files
add_library(abstract-platform.platform INTERFACE)
# Associate the sources with the library Note: The source files are not
# compiled, as this is an interface library.
target_sources(
  abstract-platform.platform
  INTERFACE ${HEADER_LIST}
  PUBLIC ${SOURCE_LIST})

# Detect if the platform is big-endian or little-endian
include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
  add_compile_definitions(__PLATFORM_BIG_ENDIAN)
else()
  add_compile_definitions(__PLATFORM_LITTLE_ENDIAN)
endif()

# Enable or disable exceptions based on the platform or configuration
option(USE_EXCEPTIONS "Enable exceptions" ON)

if(USE_EXCEPTIONS)
  target_compile_definitions(abstract-platform.platform INTERFACE __EXCEPTIONS)
else()
  target_compile_definitions(abstract-platform.platform
                             INTERFACE -fno-exceptions)
endif()

# Add include directory
target_include_directories(abstract-platform.platform
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)
