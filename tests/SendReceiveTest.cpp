
#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "mpi.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
    Catch::Session session;

    // writing to session.configData() here sets defaults
    // this is the preferred way to set them

    int returnCode = session.applyCommandLine(argc, argv);

    if (returnCode != 0)
    {
        return returnCode;
    }

    // writing to session.configData() or session.Config() here
    // overrides command line args
    // only do this if you know you need to

    mpi::instance instance(argc, argv);

    return session.run();
}

TEST_CASE("Point to point communication")
{
    SECTION("float32")
    {
        if (mpi::rank() == 0)
        {
            mpi::send(2.157864f, 1);
        }
        else if (mpi::rank() == 1)
        {
            REQUIRE(mpi::receive<float>(0) == Approx(2.157864f));
        }
    }
    SECTION("float64")
    {
        if (mpi::rank() == 0)
        {
            mpi::send(2.157864, 1);
        }
        else if (mpi::rank() == 1)
        {
            REQUIRE(mpi::receive<double>(0) == Approx(2.157864));
        }
    }
    SECTION("int32")
    {
        if (mpi::rank() == 0)
        {
            mpi::send(15, 1);
        }
        else if (mpi::rank() == 1)
        {
            REQUIRE(mpi::receive<int>(0) == 15);
        }
    }
    SECTION("int64")
    {
        if (mpi::rank() == 0)
        {
            mpi::send(15l, 1);
        }
        else if (mpi::rank() == 1)
        {
            REQUIRE(mpi::receive<long>(0) == 15l);
        }
    }
}
