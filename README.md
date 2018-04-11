[![Build Status](https://travis-ci.org/dbeurle/mpi_api_wrapper.svg?branch=master)](https://travis-ci.org/dbeurle/mpi_api_wrapper) [![Documentation Status](https://readthedocs.org/projects/cppmpi/badge/?version=latest)](http://cppmpi.readthedocs.io/en/latest/?badge=latest) [![Documentation](https://codedocs.xyz/dbeurle/mpi_api_wrapper.svg)](https://codedocs.xyz/dbeurle/mpi_api_wrapper/)

# mpi_api_wrapper
MPI bindings for modern C++.

This was created due to the absence of C++ bindings in the MPI standard.  The C++ bindings were deprecated because they offered little to no gain for the C++ programmer.  These bindings align themselves with the standard C++ library and perform compile time checking of input values.

## Usage

A single header file (include/mpi.hpp) is all that is required.  The rest of the project is associated with the test suite.

For a usage guide please refer to the `readthedocs` page or check out the unit tests.

## Developers

This project uses CMake to compile the test suite.  The tests are done using both `GCC` and `clang` compilers.  A doxygen code documentation system is also provided.

## Contributions

A small subset of the interface is covered by these bindings due to limited resources.  Additional functions and tests are always welcome.  If there are any wishes, questions or bugs please open an issue.
