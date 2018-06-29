cmake_minimum_required(VERSION 3.0)

project(cppmpi)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_COVERAGE "Set compiler flag for coverage analysis" OFF)

find_package(MPI REQUIRED)

add_subdirectory(external/catch)
include_directories(${CATCH_INCLUDE_DIR} include)

add_library(mpi_api INTERFACE)

target_include_directories(mpi_api INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(mpi_api INTERFACE SYSTEM ${MPI_CXX_INCLUDE_PATH})

include(CTest)
enable_testing()

add_subdirectory(tests)

if (ENABLE_COVERAGE)
    add_custom_target(coverage
                      COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
                      # Remove the external libraries to get coverage for neon source only
                      COMMAND lcov --remove coverage.info '/usr/*'
                                                          '${CMAKE_BINARY_DIR}/catch*'
                                                          '${CMAKE_SOURCE_DIR}/tests*' -o coverage.info)
                      # COMMAND genhtml coverage.info --output-directory coverage_output
                      # COMMAND firefox coverage_output/index.html)

endif()
