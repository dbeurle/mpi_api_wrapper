Quick start
============

The easiest way to use this library is to read the documentation and checkout the test examples.  However, a quick demonstration of the code for an all reduce operation of a single scalar is::

    double sum, local_sum = 1.0;
    MPI_Allreduce(&local_sum,
                  &sum,
                  1,
                  MPI_DOUBLE,
                  MPI_SUM,
                  MPI_COMM_WORLD);

where the count and type are specified when they could be deduced.  If we pass a scalar value, then we know the count is 1 and the type is already given by the ``sum`` and ``local_sum`` variable.  If the ``MPI_SUM`` variable or the ``MPI_DOUBLE`` is specified incorrectly, there will be a silent error.  Instead, we write::

    auto const total_sum = mpi::all_reduce(my_local_sum, mpi::sum{});

where the sum is a return value from the function can then be made ``const`` in contrast to the C interface.  Here the function defaults the ``communicator`` to the world type based on the implementation.
