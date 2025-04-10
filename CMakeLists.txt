cmake_minimum_required(VERSION 3.13)

project(abstract-platform CXX)

# Set the minimum required C++ standard for the entire workspace
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler-specific extensions

# Add the common subdirectory first to ensure the common library is built before
# any other libraries
add_subdirectory(common)
# Add the i2c subdirectory. This will add the I2C interface abstarction layer
# support.
add_subdirectory(i2c)
# Add the putput subdirectory. This will add the output (terminal, logs,
# display) interface abstarction layer support.
add_subdirectory(output)
# Add the tensor subdirectory. This will add the tensor support library
add_subdirectory(tensor)
