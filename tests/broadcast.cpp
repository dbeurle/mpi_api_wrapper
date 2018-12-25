
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
    SECTION("Scalar")
    {
        REQUIRE(mpi::broadcast(10) == 10);
        REQUIRE(mpi::broadcast(20) == 20);
    }
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
TEST_CASE("scatter operations")
{
    SECTION("integer")
    {
        // gather results to process 0
        auto const indices = mpi::scatter(std::vector<int>{2, 4}, 1, 0);

        REQUIRE(indices.size() == 1);
        REQUIRE(indices.at(0) == (mpi::rank() == 0 ? 2 : 4));
    }
}
TEST_CASE("gather operations")
{
    SECTION("integer")
    {
        // gather results to process 0
        auto const indices = mpi::gather(std::vector<int>{2, 2, 2}, 3, 0);

        std::cout << mpi::size() << std::endl;

        if (mpi::rank() == 0)
        {
            std::cout << "host processor\n";
            for (auto const& index : indices)
            {
                REQUIRE(index == 2);
            }
        }
    }
}
