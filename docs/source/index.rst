mpi_api_wrapper documentation
=============================

This project aims to provide a C++ interface to the MPI C library.  The official C++ bindings did little to make the MPI library natural to a C++ programmer.

A more natural interface is achieved through the use of advanced C++ features that perform the correct type checks, enforce ``const`` correctness and reduces the number of errors associated with using the MPI C bindings.

To maintain consistency with the standard C++ library, the project mimics the naming conventions and aims to provide a zero-cost abstraction on top of the C bindings.  It is important to note that some function names will change to be clearer and their MPI C equivalents will be noted.

Please note that due to limited developer resources, not all functions will be wrapped.  However, if you feel that there is a missing feature please open an issue.

Contents
========

.. toctree::
   :maxdepth: 2

   quickstart
   license
   contact

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
