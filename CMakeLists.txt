cmake_minimum_required(VERSION 3.5)

# Set project name
project(ransomwarePOC)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(MbedTLS)

# Add source files
add_executable(ransomwarePOC
    src/main.cpp
)

set(CMAKE_CXX_FLAGS "-fvisibility=hidden -Ofast -fno-strict-aliasing -pipe")

# Add include directories
target_include_directories(ransomwarePOC PRIVATE include)

target_link_libraries(ransomwarePOC
    stdc++
    mbedtls
    mbedcrypto
    mbedx509
)