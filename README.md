# mpi-bind
Modern c++ bindings for MPI

This was created due to the lack of c++ bindings in modern mpi.  C++ techniques such as SFINAE and template specialisation perform compile time checking and arguments are marked as `const` where applicable.  Namespaces are introduced and the style follows the standard library where possible.  There should be very little overhead for using these bindings.

## Usage

A single header file (include/mpi.hpp) is all that is required.  The rest is for testing purposes.

Instead of lengthy mpi calls using the c interface, the calls become:

```cpp
auto const total_sum = mpi::all_reduce(my_local_sum, mpi::sum{});
```

When sending, the type of the argument is deduced and this passes it through to the mpi call.  However, when receiving a template parameter is required in order to properly deduce the return value:

```cpp
auto const my_new_value = mpi::receive<long>(sending_processor_number);
```

## Contributions

A small subset of the interface is covered by these bindings.  Additional functions and tests are always welcome.
