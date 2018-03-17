Sending and receiving messages
==============================

In order to do any meaningful parallel computation, the program needs to communicate data with other processors in the network.  One of the most basic tools in the MPI toolbox is the ``send`` and ``receive`` functions.  There are two types; blocking and non-blocking.

The function names are constant and type tags are used to disambiguate the function calls.  For example, a simple blocking send of ``value`` to process 1 is  ::

    mpi::send(mpi::blocking{}, 1.0, 1);

and the corresponding receive from process 0 on process 1 is ::

    auto const recv_value = mpi::recieve<double>(0);

To receive a value from a process we must specify the type as a template parameter, otherwise the compiler cannot know what to allocate to receive the value.  We can make this extra safe by using ``decltype(the_variable_name)`` in the template parameter if needed.

In the above example, we saw the use of a default constructed empty ``struct`` type ``mpi::blocking``.  Likewise, we can perform a non-blocking send and receive.  By doing a non-blocking send, we must promise not to use the data until after the data buffer is safe to use.  In order to know when it is safe, the non-blocking send will return a ``request`` object that we can query the MPI runtime with.

For a non-blocking send of a value of 1.0 to process 1 ::

    double value{1.0};

    auto const request = mpi::send(mpi::async{}, 1.0, 1);

    // Do some computation while we wait but don't touch value!

    auto const status = mpi::wait(request);

    // Phew now it's safe to overwrite value

These methods are generic enough to extend naturally to a ``std::vector<T>`` class, or any contiguous storage type that has a ``.resize(entries)`` and a ``T::value_type`` type alias.  Compile-time errors will be produced if the type is not a primitive type such as ``int``, ``double`` etc.
