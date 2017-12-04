
#pragma once

#include <array>
#include <iostream>
#include <type_traits>
#include <vector>

#include <mpi.h>

namespace mpi
{
//! Contains the type aliases for the communicators in MPI
//! Functions default to the world communicator
enum class communicator { world, self };

//! The types of threading models allowed in MPI
enum class thread : int {
    single = MPI_THREAD_SINGLE,
    funneled = MPI_THREAD_FUNNELED,
    serialized = MPI_THREAD_SERIALIZED,
    multiple = MPI_THREAD_MULTIPLE
};

/** Alias the status type for implementation independence */
using status = MPI_Status;

/** @return The process number in the communicator */
int rank(communicator const comm = communicator::world)
{
    int processor_rank;

    MPI_Comm_rank(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
                  &processor_rank);

    return processor_rank;
}

/** @return The number of processors in the communicator */
int size(communicator const comm = communicator::world)
{
    int number_of_processors;

    MPI_Comm_size(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
                  &number_of_processors);

    return number_of_processors;
}

void barrier(communicator const comm = communicator::world)
{
    MPI_Barrier(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

struct sum
{
    static auto constexpr tag = MPI_SUM;
};

struct min
{
    static auto constexpr tag = MPI_MIN;
};

struct max
{
    static auto constexpr tag = MPI_MAX;
};

struct prod
{
    static auto constexpr tag = MPI_PROD;
};

template <typename T>
struct data_type;

template <>
struct data_type<float>
{
    static auto constexpr tag = MPI_FLOAT;
};

template <>
struct data_type<double>
{
    static auto constexpr tag = MPI_DOUBLE;
};

template <>
struct data_type<int>
{
    static auto constexpr tag = MPI_INT;
};

template <>
struct data_type<long>
{
    static auto constexpr tag = MPI_LONG;
};

template <>
struct data_type<bool>
{
    // BUG Will this work if std::vector<bool> packs booleans into an integer
    // as the memcpy behind the scenes will probably copy too little
    static auto constexpr tag = MPI_INT;
};

/**
 * Perform an MPI send operation on types which are able to have primitive
 * operations defined on it.  This function call is only for scalar (primitive)
 * values.
 */

template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value> send(
    T const send_value,
    int const destination_process,
    communicator const comm = communicator::world)
{
    int message_tag;
    MPI_Send(&send_value,
             1,
             data_type<T>::tag,
             destination_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> receive(
    int const source_process,
    communicator const comm = communicator::world)
{
    int message_tag;
    T recieve_value;

    MPI_Recv(&recieve_value,
             1,
             data_type<T>::tag,
             source_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
             MPI_STATUS_IGNORE);

    return recieve_value;
}

/**
 * Perform an MPI Allreduce operation on types which are able to have primitive
 * operations defined on it.  This function call is only for scalar (primitive)
 * values.
 */
// clang-format off
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> all_reduce(T const local_reduction_variable,
                                                                    Operation_Tp&& operation,
                                                                    communicator const comm = communicator::world)
// clang-format on
{
    T reduction_variable;

    MPI_Allreduce(&local_reduction_variable,
                  &reduction_variable,
                  1,
                  data_type<T>::tag,
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

/**
 * Perform an MPI Allreduce operation on a vector of types which are able to have
 * primitive operations defined on it.
 *
 * This function allocates a vector of the same type and size of the input vector
 * and fills it with the result from the all_reduce operation
 */
// clang-format off
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<T>::value, std::vector<T>> all_reduce(std::vector<T> const& local_reduction_variable,
                                                                                 Operation_Tp&& operation,
                                                                                 communicator const comm = communicator::world)
// clang-format on
{
    std::vector<T> reduction_variable(local_reduction_variable.size());

    MPI_Allreduce(local_reduction_variable.data(),
                  reduction_variable.data(),
                  reduction_variable.size(),
                  data_type<T>::tag,
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

// clang-format off
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, std::vector<T>> all_to_all(T const local_data,
                                                                                 communicator const comm = communicator::world)
// clang-format on
{
    std::vector<T> collected_data(mpi::size(comm));

    MPI_Alltoall(&local_data,
                 1,
                 data_type<T>::tag,
                 collected_data.data(),
                 collected_data.size(),
                 data_type<T>::tag,
                 comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

// clang-format off
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, std::vector<T>> all_to_all(std::vector<T> const& local_data,
                                                                                 communicator const comm = communicator::world)
// clang-format on
{
    std::vector<T> collected_data(local_data.size() * mpi::size(comm));

    MPI_Alltoall(local_data.data(),
                 local_data.size(),
                 data_type<T>::tag,
                 collected_data.data(),
                 collected_data.size(),
                 data_type<T>::tag,
                 comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

thread initialise(int argc, char** argv, thread const thread_required)
{
    // MPI thread level provided
    auto thread_provided = static_cast<int>(thread_required);

    MPI_Init_thread(&argc, &argv, static_cast<int>(thread_required), &thread_provided);

    return static_cast<thread>(thread_provided);
}

void initialise(int argc, char** argv) { MPI_Init(&argc, &argv); }

void finalise() { MPI_Finalize(); }

/**
 * Creates an instance of the MPI environment, automatically handling the
 * initialisation and destruction of the MPI environment.  Calls to
 * MPI_init and MPI_finalize are no longer required.
 */
class instance
{
public:
    instance(int argc, char** argv) { mpi::initialise(argc, argv); }

    instance(int argc, char** argv, thread const thread_required)
    {
        auto const provided = mpi::initialise(argc, argv, thread_required);

        if (mpi::rank() == 0)
        {
            switch (provided)
            {
                case mpi::thread::single:
                    std::cout << "MPI_Init_thread level = MPI_THREAD_SINGLE\n";
                    break;
                case mpi::thread::funneled:
                    std::cout << "MPI_Init_thread level = MPI_THREAD_FUNNELED\n";
                    break;
                case mpi::thread::serialized:
                    std::cout << "MPI_Init_thread level = MPI_THREAD_SERIALIZED\n";
                    break;
                case mpi::thread::multiple:
                    std::cout << "MPI_Init_thread level = MPI_THREAD_MULTIPLE\n";
                    break;
                default:
                    std::cout << "MPI_Init_thread level = ???\n";
            }
        }
    }

    ~instance() { mpi::finalise(); }
};
}
