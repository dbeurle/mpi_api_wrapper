cmake_minimum_required(VERSION 3.0)

project(cppmpi)

set(CMAKE_CXX_STANDARD 14)

find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_CXX_INCLUDE_PATH})

add_subdirectory(external/catch)
include_directories(${CATCH_INCLUDE_DIR} include)

include(CTest)
enable_testing()
add_subdirectory(tests)
