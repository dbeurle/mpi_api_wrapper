
#pragma once

#include <iostream>
#include <type_traits>

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
struct data_type<short>
{
    static auto constexpr tag = MPI_SHORT;
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
    int message_tag = 0;
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
    int message_tag = 0;
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

/*----------------------------------------------------------------------------*
 *                           BROADCAST OPERATIONS                             *
 *----------------------------------------------------------------------------*/

/**
 * broadcast sends the local data from the host_processor across the
 * communicator
 * which is defaulted to 0
 * The result can be read out from the host_processor and the slave processors
 * are none the wiser.
 * @param local_data value to send if we are the host_processor
 * @param host_processor process responsible for sending out the data
 * @param comm MPI communicator
 * @return the broadcast data
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> broadcast(
    T local_data,
    int const host_processor = 0,
    communicator const comm = communicator::world)
{
    MPI_Bcast(&local_data,
              1,
              data_type<T>::tag,
              host_processor,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return local_data;
}

/**
 * broadcast sends the local data from the host_processor across the
 * communicator
 * which is defaulted to 0
 * The result can be read out from the host_processor and the slave processors
 * are none the wiser.
 * @param local_data value to send if we are the host_processor
 * @param host_processor process responsible for sending out the data
 * @param comm MPI communicator
 * @return the broadcast data
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> broadcast(
    T local_data,
    int const host_processor = 0,
    communicator const comm = communicator::world)
{
    MPI_Bcast(local_data.data(),
              local_data.size(),
              data_type<typename T::value_type>::tag,
              host_processor,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return local_data;
}

/*----------------------------------------------------------------------------*
 *                           REDUCTION OPERATIONS                             *
 *----------------------------------------------------------------------------*/

/**
 * reduce computes across the Operation_tp and stores the result in
 * @param host_processor
 * which is defaulted to 0
 * The result can be read out from the host_processor and the slave processors
 * are none the wiser.
 */
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> reduce(
    T const local_data,
    Operation_Tp&& operation_type,
    int const host_processor = 0,
    communicator const comm = communicator::world)
{
    T collected_data;

    MPI_Reduce(&local_data,
               &collected_data,
               1,
               data_type<T>::tag,
               operation_type.tag,
               host_processor,
               comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
    return collected_data;
}

/**
 * reduce computes across the Operation_tp and stores the result in
 * @param host_processor
 * which is defaulted to 0
 * The result can be read out from the host_processor and the slave processors
 * are none the wiser.
 */
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> reduce(
    T const& local_data,
    Operation_Tp&& operation_type,
    int const host_processor = 0,
    communicator const comm = communicator::world)
{
    T collected_data(local_data.size());

    MPI_Reduce(local_data.data(),
               collected_data.data(),
               local_data.size(),
               data_type<typename T::value_type>::tag,
               operation_type.tag,
               host_processor,
               comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

/**
 * Perform an MPI Allreduce operation on types which are able to have primitive
 * operations defined on it.  This function call is only for scalar (primitive)
 * values.
 */
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> all_reduce(
    T const local_reduction_variable,
    Operation_Tp&& operation,
    communicator const comm = communicator::world)
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
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> all_reduce(
    T const& local_reduction_variable,
    Operation_Tp&& operation,
    communicator const comm = communicator::world)
{
    T reduction_variable(local_reduction_variable.size());

    MPI_Allreduce(local_reduction_variable.data(),
                  reduction_variable.data(),
                  reduction_variable.size(),
                  data_type<typename T::value_type>::tag,
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> all_to_all(
    T const local_data,
    communicator const comm = communicator::world)
{
    T collected_data(mpi::size(comm));

    MPI_Alltoall(&local_data,
                 1,
                 data_type<T>::tag,
                 collected_data.data(),
                 collected_data.size(),
                 data_type<T>::tag,
                 comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> all_to_all(
    T const& local_data,
    communicator const comm = communicator::world)
{
    T collected_data(local_data.size() * mpi::size(comm));

    MPI_Alltoall(local_data.data(),
                 local_data.size(),
                 data_type<typename T::value_type>::tag,
                 collected_data.data(),
                 collected_data.size(),
                 data_type<typename T::value_type>::tag,
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
