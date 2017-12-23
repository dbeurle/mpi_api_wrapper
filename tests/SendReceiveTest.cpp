
#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "mpi.hpp"

#include <iostream>
#include <numeric>

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

TEST_CASE("Point to point synchronous communication")
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
    SECTION("integer vector")
    {
        if (mpi::rank() == 0)
        {
            mpi::send(std::vector<int>{0, 1, 2, 3, 4}, 1);
        }
        else if (mpi::rank() == 1)
        {
            auto const received_vector = mpi::receive<std::vector<int>>(0);

            REQUIRE(received_vector.size() == 5);
            for (auto i = 0; i < 5; i++)
            {
                REQUIRE(received_vector.at(i) == i);
            }
        }
    }
}
TEST_CASE("Point to point asynchronous communication")
{
    SECTION("float32")
    {
        if (mpi::rank() == 0)
        {
            auto const request = mpi::send_async(2.157864f, 1);

            // Computation happens here

            auto const status = mpi::wait(request);
        }
        else if (mpi::rank() == 1)
        {
            REQUIRE(mpi::receive<float>(0) == Approx(2.157864f));
        }
    }
    SECTION("vector of float32")
    {
        if (mpi::rank() == 0)
        {
            std::vector<float> data_to_send(10, 3.14f);

            auto const request = mpi::send_async(data_to_send, 1);

            // Computation happens here

            auto const status = mpi::wait(request);

            // Now std::vector<float> is deleted after the wait
        }
        else if (mpi::rank() == 1)
        {
            for (auto i : mpi::receive<std::vector<float>>(0))
            {
                REQUIRE(i == Approx(3.14f));
            }
        }
    }
    SECTION("vector of int")
    {
        if (mpi::rank() == 0)
        {
            std::vector<int> data_to_send(10);
            std::iota(std::begin(data_to_send), std::end(data_to_send), 0);

            auto const request = mpi::send_async(data_to_send, 1);

            // Computation happens here

            auto const status = mpi::wait(request);

            // Now std::vector<int> is deleted after the wait
        }
        else if (mpi::rank() == 1)
        {
            auto j = 0;
            for (auto i : mpi::receive<std::vector<int>>(0))
            {
                REQUIRE(i == j++);
            }
        }
    }
    SECTION("vector of int with wait all")
    {
        if (mpi::rank() == 0)
        {
            std::vector<int> data_to_send(10);
            std::iota(std::begin(data_to_send), std::end(data_to_send), 0);

            std::vector<mpi::request> requests;

            requests.emplace_back(mpi::send_async(data_to_send, 1));

            // Computation happens here

            auto const statuses = mpi::wait_all(requests);

            // Now std::vector<int> is deleted after the wait_all
        }
        else if (mpi::rank() == 1)
        {
            auto j = 0;
            for (auto i : mpi::receive<std::vector<int>>(0))
            {
                REQUIRE(i == j++);
            }
        }
    }
}
