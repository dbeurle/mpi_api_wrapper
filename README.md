[![Build Status](https://travis-ci.org/dbeurle/cppmpi.svg?branch=master)](https://travis-ci.org/dbeurle/cppmpi) [![Documentation Status](https://readthedocs.org/projects/cppmpi/badge/?version=latest)](http://cppmpi.readthedocs.io/en/latest/?badge=latest)

# mpi_api_wrapper
Modern MPI bindings for c++

This was created due to the lack of c++ bindings in modern mpi.  Modern c++ techniques such as SFINAE and template specialisation perform compile time checking that results in a clean interface to the MPI C library.  Function arguments are marked as `const` where applicable to maintain const-correctness and helps to reduce the chance of something going wrong.  Compared to the original c++ interface, this interface reduces the amount of work required.

All functions are in the `mpi` namespace the function naming style follows the standard library where possible.  There should be very little overhead when using these bindings with compiler optimisations.

Currently `std::vector` is tested against these bindings and dynamic memory allocation will be performed inside the routines when not using a primative type (a single `int`, `float` etc).  This allows the return argument to me marked as const and reduces the possibilities for bugs in the code.

## Usage

A single header file (include/mpi.hpp) is all that is required.  The rest is for testing purposes.

Instead of lengthy MPI library calls using the c interface, the calls become:

```cpp
auto const total_sum = mpi::all_reduce(my_local_sum, mpi::sum{});
```

because the type of `my_local_sum` is matched to the correct `MPI_DataType` and the size is known.  The default communicator is taken as MPI comm world but this can be specified if required as an additional argument.

When sending, the type of the argument is deduced and this passes it through to the MPI call.  However, when receiving a template parameter is required in order to properly deduce the return value:

```cpp
auto const my_new_value = mpi::receive<long>(sending_processor_number);
```

which is unfortunate but required.  One possibility is to use `decltype(variable)` to shelter the calls from type changes.  For receiving a vector of data the syntax is

```cpp
auto const my_new_vector = mpi::receive<std::vector<double>>(sending_processor_number);
```

where the template parameter is now the vector return type. This will perform an `MPI_Probe` to determine the size and automatically resizes the buffer, fills with data and returns it from the function.

Full documentation will follow shortly, otherwise check out the unit tests.

## Contributions

A small subset of the interface is covered by these bindings.  Additional functions and tests are always welcome.  If there are any wishes, questions or bugs please open an issue.
