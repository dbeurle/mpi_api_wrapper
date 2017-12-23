
#pragma once

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

/** Alias the request type for implementation independence */
using request = MPI_Request;

/** @return The process number in the communicator */
int rank(communicator const comm = communicator::world)
{
    int processor_rank;

    MPI_Comm_rank(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF, &processor_rank);

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

/*----------------------------------------------------------------------------*
 *                  Reduction operations for MPI data types                   *
 *----------------------------------------------------------------------------*/

struct sum
{
    MPI_Op const tag = MPI_SUM;
};

struct min
{
    MPI_Op const tag = MPI_MIN;
};

struct max
{
    MPI_Op const tag = MPI_MAX;
};

struct prod
{
    MPI_Op const tag = MPI_PROD;
};

/*----------------------------------------------------------------------------*
 *               Template specialisations for MPI data types                  *
 *----------------------------------------------------------------------------*/

template <class T>
struct data_type;

template <>
struct data_type<int>
{
    static MPI_Datatype value_type() { return MPI_INT; }
};

template <>
struct data_type<long int>
{
    static MPI_Datatype value_type() { return MPI_LONG_INT; }
};

template <>
struct data_type<long long int>
{
    static MPI_Datatype value_type() { return MPI_LONG_LONG_INT; }
};

template <>
struct data_type<char>
{
    static MPI_Datatype value_type() { return MPI_CHAR; }
};

template <>
struct data_type<float>
{
    static MPI_Datatype value_type() { return MPI_FLOAT; }
};

template <>
struct data_type<double>
{
    static MPI_Datatype value_type() { return MPI_DOUBLE; }
};

template <>
struct data_type<long double>
{
    static MPI_Datatype value_type() { return MPI_LONG_DOUBLE; }
};

/*----------------------------------------------------------------------------*
 *                         BLOCKING SEND RECEIVE                              *
 *----------------------------------------------------------------------------*/

/**
 * Perform an MPI send operation on types which are able to have primitive
 * operations defined on it.  This function call is only for scalar (primitive)
 * values.
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value> send(T const send_value,
                                                           int const destination_process,
                                                           int const message_tag = 0,
                                                           communicator const comm = communicator::world)
{
    MPI_Send(&send_value,
             1,
             data_type<T>::value_type(),
             destination_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

/**
 * Perform an MPI send operation on types which are able to have primitive
 * operations defined on it.  This function call is only for vector types.
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value> send(
    T const& send_vector,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    MPI_Send(send_vector.data(),
             send_vector.size(),
             data_type<typename T::value_type>::value_type(),
             destination_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> receive(
    int const source_process, int const message_tag = 0, communicator const comm = communicator::world)
{
    T recieve_value;

    MPI_Recv(&recieve_value,
             1,
             data_type<T>::value_type(),
             source_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
             MPI_STATUS_IGNORE);

    return recieve_value;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> receive(
    int const source_process, int const message_tag = 0, communicator const comm = communicator::world)
{
    mpi::status probe_status;

    // Probe for an incoming message from the sending process
    MPI_Probe(source_process,
              message_tag,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
              &probe_status);

    int buffer_size;

    MPI_Get_count(&probe_status, data_type<typename T::value_type>::value_type(), &buffer_size);

    T receive_buffer(buffer_size);

    MPI_Recv(receive_buffer.data(),
             receive_buffer.size(),
             data_type<typename T::value_type>::value_type(),
             source_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
             MPI_STATUS_IGNORE);

    return receive_buffer;
}

/*----------------------------------------------------------------------------*
 *                        ASYNCHRONOUS SEND RECEIVE                           *
 *----------------------------------------------------------------------------*/

/**
 * Perform an asynchronous MPI send operation on types which are able to have primitive
 * operations defined on it.  This function call is only for scalar (primitive)
 * values.
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, request> send_async(
    T const& send_async_value,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    request async_send_request;

    MPI_Isend(&send_async_value,
              1,
              data_type<T>::value_type(),
              destination_process,
              message_tag,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
              &async_send_request);

    return async_send_request;
}

/**
 * Perform an asynchronous MPI send operation on types which are able to have
 * primitive operations defined on it.  This function call is only for vector types.
 */
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, request> send_async(
    T const& send_async_vector,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    request async_send_request;

    MPI_Isend(send_async_vector.data(),
              send_async_vector.size(),
              data_type<typename T::value_type>::value_type(),
              destination_process,
              message_tag,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
              &async_send_request);

    return async_send_request;
}

/**
 * Wait until the asynchronous send operation in request is finished.
 * \sa request
 * \sa status
 * \sa wait_all
 * @param async_request
 * @return The status of the asynchronous operation
 */
inline status wait(request async_request)
{
    status wait_status;
    MPI_Wait(&async_request, &wait_status);
    return wait_status;
}

/**
 * Wait until all the asynchronous send operation in requests is finished.
 * \sa request
 * \sa status
 * \sa wait
 * @param async_requests
 * @return A vector of statuses of the asynchronous operation
 */
inline std::vector<status> wait_all(std::vector<request>& async_requests)
{
    int const count = async_requests.size();

    std::vector<status> statuses(count);

    MPI_Waitall(count, async_requests.data(), statuses.data());

    return statuses;
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
    T local_data, int const host_processor = 0, communicator const comm = communicator::world)
{
    MPI_Bcast(&local_data,
              1,
              data_type<T>::value_type(),
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
    T local_data, int const host_processor = 0, communicator const comm = communicator::world)
{
    MPI_Bcast(local_data.data(),
              local_data.size(),
              data_type<typename T::value_type>::value_type(),
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
               data_type<T>::value_type(),
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
               data_type<typename T::value_type>::value_type(),
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
    T local_reduction_variable, Operation_Tp&& operation, communicator const comm = communicator::world)
{
    T reduction_variable;

    MPI_Allreduce(&local_reduction_variable,
                  &reduction_variable,
                  1,
                  data_type<T>::value_type(),
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

/**
 * Perform an MPI Allreduce operation on a vector of types which are able to
 * have primitive operations defined on it.
 *
 * This function allocates a vector of the same type and size of the input
 * vector and fills it with the result from the all_reduce operation
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
                  data_type<typename T::value_type>::value_type(),
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> all_to_all(
    T const local_data, communicator const comm = communicator::world)
{
    T collected_data(mpi::size(comm));

    MPI_Alltoall(&local_data,
                 1,
                 data_type<T>::value_type(),
                 collected_data.data(),
                 collected_data.size(),
                 data_type<T>::value_type(),
                 comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> all_to_all(
    T const& local_data, communicator const comm = communicator::world)
{
    T collected_data(local_data.size() * mpi::size(comm));

    MPI_Alltoall(local_data.data(),
                 local_data.size(),
                 data_type<typename T::value_type>::value_type(),
                 collected_data.data(),
                 collected_data.size(),
                 data_type<typename T::value_type>::value_type(),
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
