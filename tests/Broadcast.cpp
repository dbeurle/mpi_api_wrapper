
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

TEST_CASE("Broadcast communication")
{
    SECTION("Scalar") { REQUIRE(mpi::broadcast(10) == 10); }
    SECTION("Vector")
    {
        std::vector<int> bcast_vector{{1, 2}};

        auto const bcast = mpi::broadcast(bcast_vector);

        if (mpi::rank() == 1)
        {
            REQUIRE(bcast.at(0) == 1);
            REQUIRE(bcast.at(1) == 2);
        }
    }
}
