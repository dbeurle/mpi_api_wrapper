
#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

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

TEST_CASE("Reduction operations")
{
    SECTION("Basic checks")
    {
        REQUIRE(mpi::size() == 2);
        REQUIRE(mpi::rank() < mpi::size());
    }
    SECTION("Scalar valued all reduce")
    {
        REQUIRE(mpi::all_reduce(1, mpi::sum{}) == mpi::size());

        REQUIRE(mpi::all_reduce(1, mpi::prod{}) == 1);

        REQUIRE(mpi::all_reduce(mpi::rank(), mpi::max{}) == mpi::size() - 1);

        REQUIRE(mpi::all_reduce(mpi::rank(), mpi::min{}) == 0);
    }
    SECTION("Vector valued all reduce")
    {
        std::vector<int> vec{{1, 1, 1}};

        for (auto const& sum : mpi::all_reduce(vec, mpi::sum{}))
        {
            REQUIRE(sum == mpi::size());
        }
        for (auto const prod : mpi::all_reduce(vec, mpi::prod{}))
        {
            REQUIRE(prod == 1);
        }

        REQUIRE(mpi::all_reduce(mpi::rank(), mpi::max{}) == mpi::size() - 1);

        REQUIRE(mpi::all_reduce(mpi::rank(), mpi::min{}) == 0);
    }
    SECTION("Scalar valued reduce")
    {
        auto const root_sum = mpi::reduce(1, mpi::sum{}, 1);
        if (mpi::rank() == 1)
        {
            REQUIRE(root_sum == mpi::size());
        }

        auto const root_prod = mpi::reduce(1, mpi::prod{}, 1);
        if (mpi::rank() == 1)
        {
            REQUIRE(root_prod == 1);
        }

        auto const root_max = mpi::reduce(mpi::rank(), mpi::max{}, 1);
        if (mpi::rank() == 1)
        {
            REQUIRE(root_max == mpi::size() - 1);
        }

        auto const root_min = mpi::reduce(mpi::rank(), mpi::min{}, 1);
        if (mpi::rank() == 1)
        {
            REQUIRE(root_min == 0);
        }
    }
    SECTION("Vector valued reduce")
    {
        std::vector<int> array(2, mpi::rank());

        auto const root_sum = mpi::reduce(array, mpi::sum{}, 1);
        if (mpi::rank() == 1)
        {
            for (auto i : root_sum)
            {
                REQUIRE(i == 1);
            }
        }

        auto const root_prod = mpi::reduce(array, mpi::prod{}, 1);
        if (mpi::rank() == 1)
        {
            for (auto i : root_prod)
            {
                REQUIRE(i == 0);
            }
        }

        auto const root_max = mpi::reduce(array, mpi::max{}, 1);
        if (mpi::rank() == 1)
        {
            for (auto i : root_max)
            {
                REQUIRE(i == 1);
            }
        }

        auto const root_min = mpi::reduce(array, mpi::min{}, 1);
        if (mpi::rank() == 1)
        {
            for (auto i : root_min)
            {
                REQUIRE(i == 0);
            }
        }
    }
}
