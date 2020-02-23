
#define CATCH_CONFIG_RUNNER

#include <catch.hpp>
#include <vector>

#include "mpi.hpp"

constexpr auto NUM_PROCESSORS = 2;

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

TEST_CASE("Gather operations")
{
    SECTION("Basic checks")
    {
        REQUIRE(mpi::size() == 2);
        REQUIRE(mpi::rank() < mpi::size());
    }
    SECTION("Small send")
    {
        std::vector<int> gather_vector{{1, mpi::rank()}};

        auto const result = mpi::gather(gather_vector, 0);

        if (mpi::rank() == 0)
        {
            REQUIRE(result.at(0) == 1);
            REQUIRE(result.at(1) == 0);
            REQUIRE(result.at(2) == 1);
            REQUIRE(result.at(3) == 1);
        }
    }
}
