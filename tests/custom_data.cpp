
#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "mpi.hpp"

#include <cstddef>
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

TEST_CASE("Custom datatype sending")
{
    SECTION("Contiguous struct")
    {
        struct contiguous
        {
            double value0;
            double value1;
        };

        auto data_type = mpi::create_contiguous_type<contiguous, double>();
        mpi::commit(data_type);
    }
    SECTION("Custom struct type")
    {
        struct simple_struct
        {
            int index;
            float value;
        };

        std::vector<simple_struct> simple_data(10);

        std::generate(begin(simple_data), end(simple_data), [value = 0.0f]() mutable {
            simple_struct data;

            data.index = mpi::rank();
            data.value = value;
            value + 1.0;

            return data;
        });

        // elements per block
        std::array<int, 2> block_lengths{1, 1};
        // offsets in the struct
        std::array<mpi::address_int, 2> displacements{offsetof(simple_struct, index),
                                                      offsetof(simple_struct, value)};
        // types for each field
        std::array<mpi::type, 2> types{mpi::data_type<decltype(simple_struct::index)>::value_type(),
                                       mpi::data_type<decltype(simple_struct::value)>::value_type()};

        auto data_type = mpi::create_struct_type<simple_struct>(block_lengths, displacements, types);
        mpi::commit(data_type);
    }
    SECTION("")
    {
        // #include <cmath>
        // #include <cstddef>
        // #include <cstdlib>
        // #include <iostream>
        // #include <mpi.h>
        //
        //         constexpr int DOUBLE_MAX = 10;
        //
        //         struct CustomData
        //         {
        //             int n_values;
        //             double values[DOUBLE_MAX];
        //         };
        //
        //         int main(int argc, char** argv)
        //         {
        //             MPI_Init(&argc, &argv);
        //
        //             int rank, size;
        //
        //             MPI_Comm_size(MPI_COMM_WORLD, &size);
        //             MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        //
        //             constexpr int n_structure_per_process = 5; // M = 5
        //
        //             // Random generator init
        //             srand(rank * 10);
        //
        //             // Creating the dataset
        //             CustomData data[n_structure_per_process];
        //
        //             // Generating the data
        //             for (int i = 0; i < n_structure_per_process; ++i)
        //             {
        //                 // Terrible way of generating random numbers, don't reproduce this at home
        //                 data[i].n_values = rand() % DOUBLE_MAX + 1;
        //                 for (int j = 0; j < DOUBLE_MAX; ++j)
        //                     data[i].values[j] = (j < data[i].n_values ? (double)rand() / (double)RAND_MAX
        //                                                               : 0.0);
        //             }
        //
        //             // 1- Here, create all the properties to call MPI_Type_create_struct
        //             MPI_Aint displacements[2] = {offsetof(CustomData, n_values),
        //                                          offsetof(CustomData, values)};
        //             int block_lengths[2] = {1, 10};
        //             MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
        //             MPI_Datatype custom_dt;
        //
        //             // 2- Create the type, and commit it
        //             MPI_Type_create_struct(2, block_lengths, displacements, types, &custom_dt);
        //
        //             MPI_Type_commit(&custom_dt);
        //
        //             // Gathering the data
        //             CustomData* gathered_data = nullptr;
        //
        //             if (rank == 0)
        //             {
        //                 gathered_data = new CustomData[n_structure_per_process * size];
        //             }
        //
        //             MPI_Gather(data,
        //                        n_structure_per_process,
        //                        custom_dt,
        //                        gathered_data,
        //                        n_structure_per_process,
        //                        custom_dt,
        //                        0,
        //                        MPI_COMM_WORLD);
        //
        //             // And printing it
        //             if (rank == 0)
        //             {
        //                 for (int i = 0; i < size; ++i)
        //                 {
        //                     for (int j = 0; j < n_structure_per_process; ++j)
        //                     {
        //                         // Linear index
        //                         int data_id = i * n_structure_per_process + j;
        //
        //                         std::cout << "Data structure " << data_id << " : [";
        //                         int N = gathered_data[data_id].n_values;
        //
        //                         for (int k = 0; k < N; ++k)
        //                         {
        //                             std::cout << gathered_data[data_id].values[k]
        //                                       << (k == N - 1 ? "]" : "; ");
        //                         }
        //                         std::cout << std::endl;
        //                     }
        //                 }
        //             }
        //
        //             MPI_Finalize();
        //
        //             return 0;
        //         }
    }
}
