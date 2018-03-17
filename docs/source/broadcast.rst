Broadcasting
============

A broadcast operation takes the root process and sends the data out to all of the processors.

For example, if you are the root process you can send a value of `10` to all the other processes in the communicator ::

    auto const broad_casted = mpi::broadcast(10);

    // All processes now have the value of 10
