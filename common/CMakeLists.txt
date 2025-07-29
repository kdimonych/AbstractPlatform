cmake_minimum_required(VERSION 3.13)

project(abstract-platform.common CXX)

option(ENABLE_ASSERTS_FOR_COMMON "Enable asserts for abstract-platform.common"
       ON)

set(HEADER_LIST
    AbstractPlatform/common/ArrayHelper.hpp
    AbstractPlatform/common/Buffer.hpp
    AbstractPlatform/common/BinaryOperations.hpp
    AbstractPlatform/common/ErrorCode.hpp
    AbstractPlatform/common/BitOperations.hpp
    AbstractPlatform/common/impl/BitOperationsImpl.hpp
    AbstractPlatform/common/Duplicator.hpp
    AbstractPlatform/common/Memory.hpp)

set(SOURCE_LIST)

# Add library cpp files
add_library(abstract-platform.common INTERFACE)

if(NOT ENABLE_ASSERTS_FOR_COMMON)
  target_compile_definitions(abstract-platform.common INTERFACE NDEBUG)
endif()

# Associate the sources with the library Note: The source files are not
# compiled, as this is an interface library.
target_sources(
  abstract-platform.common
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
  target_compile_definitions(abstract-platform.common INTERFACE __EXCEPTIONS)
else()
  target_compile_definitions(abstract-platform.common INTERFACE -fno-exceptions)
endif()

target_link_libraries(abstract-platform.common
                      INTERFACE abstract-platform.platform)

# Add include directory
target_include_directories(abstract-platform.common
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(test)
