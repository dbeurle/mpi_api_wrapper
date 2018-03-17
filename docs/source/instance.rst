Creating an MPI instance
========================

In order to use the MPI library, we need to create an MPI instance.  To accomplish this, we simply create an instantiation of an ``mpi::instance`` object ::

    int main(int argc, char* argv[])
    {
        mpi::instance instance(argc, argv);

        // Do parallel work

        return 0;
    }

Notice that we do not need to call any functions for finalising an MPI program, since this is handled by the destructor of the instance object.  This helps when exiting a program from multiple locations to ensure the parallel environment is correctly finalised.

We can take this one step further an allow a creation of an MPI environment where the multiple threads can call the MPI functions.  For this we add an additional construction parameter ``mpi::thread`` which takes the values ``single``, ``funnelled``, ``serialised`` and ``multiple`` which becomes ::

    int main(int argc, char* argv[])
    {
        mpi::instance instance(argc, argv, mpi::thread::single);

        // Do parallel work

        return 0;
    }
