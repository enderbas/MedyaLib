cmake_minimum_required(VERSION 3.21)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
include(${CMAKE_TOOLCHAIN_FILE})
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
project(MedyaLib)

include_directories(${CMAKE_CURRENT_LIST_DIR})

set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/)
set(PROJECT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} )
add_subdirectory(${PROJECT_ROOT}/cpp)
