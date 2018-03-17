Reduction operations
====================

Some parallel algorithms require a reduction operation of a particular value.  This is so that each process can agree on a particular value for a variable.  For this we need to use some `reduction` operator.  For example, an all reduce operation of a scalar by performing a sum operation is ::

    auto const count = mpi::all_reduce(1, mpi::sum{});

which should be equal to the number of processes in the communicator ``mpi::size()``.  There are other instances of a reduction operation.  This could be that a given process needs the minimum value of all the other process's rank ::

    auto const process_one_min = mpi::reduce(mpi::rank(), mpi::min{}, 1);

    // process_one_min is only meaningful for process 1 (which should be 0)

Here we have also introduced the wrapped function ``mpi::rank()`` which returns an `int` holding the rank of the current process.

No other reduction operations are currently supported.
