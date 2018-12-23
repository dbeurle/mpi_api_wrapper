
#pragma once

#include <type_traits>
#include <vector>

#include <mpi.h>

/// \file mpi.hpp
/// \brief MPI function wrappers

/// Namespace for wrapper functions
namespace mpi
{
/// Contains the type aliases for the communicators in MPI
/// Functions use the world communicator by default
enum class communicator { world, self };

/// The types of threading models allowed in MPI
enum class thread : int {
    single = MPI_THREAD_SINGLE,
    funnelled = MPI_THREAD_FUNNELED,
    serialised = MPI_THREAD_SERIALIZED,
    multiple = MPI_THREAD_MULTIPLE
};

/// Alias the MPI_Status type
using status = MPI_Status;

/// Alias the MPI_Request type
using request = MPI_Request;

/// \param comm The MPI communicator \sa communicator
/// \return The process number in the communicator
inline int rank(communicator const comm = communicator::world)
{
    int processor_rank;

    MPI_Comm_rank(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF, &processor_rank);

    return processor_rank;
}

/// \return The number of processors in the communicator
inline int size(communicator const comm = communicator::world)
{
    int number_of_processors;

    MPI_Comm_size(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
                  &number_of_processors);

    return number_of_processors;
}

/// Don't continue until all processes in the communicator are at this point
inline void barrier(communicator const comm = communicator::world)
{
    MPI_Barrier(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

/*----------------------------------------------------------------------------*
 *                  Reduction operations for MPI data types                   *
 *----------------------------------------------------------------------------*/

/// \class sum
/// \brief \p MPI_SUM operation
struct sum
{
    MPI_Op const tag = MPI_SUM;
};

/// \class min
/// \brief \p MPI_MIN operation
struct min
{
    MPI_Op const tag = MPI_MIN;
};

/// \class max
/// \brief \p MPI_MAX operation
struct max
{
    MPI_Op const tag = MPI_MAX;
};

/// \class prod
/// \brief \p MPI_PROD operation
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
 *                              Synchronous types                             *
 *----------------------------------------------------------------------------*/

/// blocking is a type tag that will not return until the buffer is safe to reuse
struct blocking
{
};

/// async will send data as it pleases from the input buffer.  This buffer
/// cannot be used until a wait has been called to ensure data transfer is
/// complete
/// \sa wait_all()
/// \sa wait()
struct async
{
};

/*----------------------------------------------------------------------------*
 *                               SEND RECEIVE                                 *
 *----------------------------------------------------------------------------*/

/// Perform an MPI send operation on scalar types which are able to have primitive
/// operations defined on it.  This function call is only for scalar (primitive)
/// values.
/// \tparam T Data type to send
/// \param blocking Perform a blocking send
/// \param send_value Data to send
/// \param destination_process Where to send the data
/// \param message_tag Add a tag to the message
/// \param comm Communicator type
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value> send(blocking,
                                                           T send_value,
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

/// Perform an MPI send operation on types which are able to have primitive
/// operations defined on it.  This function call is only for vector types.
/// \tparam T Data type to send
/// \param blocking Perform a blocking send
/// \param send_vector Vector of data to send
/// \param destination_process Where to send the data
/// \param message_tag Add a tag to the message
/// \param comm Communicator type
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value> send(
    blocking,
    T const& send_vector,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    MPI_Send(const_cast<typename T::value_type*>(send_vector.data()),
             send_vector.size(),
             data_type<typename T::value_type>::value_type(),
             destination_process,
             message_tag,
             comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);
}

/// Perform an MPI receive operation on types which are able to have primitive
/// operations defined on it.  This function call is only used for scalar types.
/// \tparam T Data type to receive
/// \param source_process Processor to receive from
/// \param message_tag Matching tag to the message
/// \param comm Communicator type
/// \return Received value
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

/// Perform an MPI receive operation on types which are able to have primitive
/// operations defined on it.  This function call is only used for vector types
/// and automatically probes the size, resizes the return vector and returns the
/// data.
/// \tparam T Data type to receive
/// \param source_process Processor to receive from
/// \param message_tag Matching tag to the message
/// \param comm Communicator type
/// \return Received vector of type T
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> receive(
    int const source_process, int const message_tag = 0, communicator const comm = communicator::world)
{
    ::mpi::status probe_status;

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

/// Perform an asynchronous MPI send operation on types which are able to have primitive
/// operations defined on it.  This function call is only for built-in values.
/// \tparam Value type
/// \param async
/// \param send_value Value to send
/// \param destination_process
/// \param message_tag
/// \param comm Communicator
/// \return An MPI request object \sa request
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, request> send(
    async,
    T const& send_value,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    request async_send_request;

    MPI_Isend(const_cast<T*>(&send_value),
              1,
              data_type<T>::value_type(),
              destination_process,
              message_tag,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
              &async_send_request);

    return async_send_request;
}

/// Perform an asynchronous MPI send operation.  This function call is only for
/// a contiguous vector of built-in types.
/// \tparam Value type
/// \param async
/// \param send_value Vector to send
/// \param destination_process
/// \param message_tag
/// \param comm Communicator
/// \return An MPI request object \sa request
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, request> send(
    async,
    T const& send_data,
    int const destination_process,
    int const message_tag = 0,
    communicator const comm = communicator::world)
{
    request async_send_request;

    MPI_Isend(const_cast<typename T::value_type*>(send_data.data()),
              send_data.size(),
              data_type<typename T::value_type>::value_type(),
              destination_process,
              message_tag,
              comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF,
              &async_send_request);

    return async_send_request;
}

/// Wait until the asynchronous send operation in request is finished.
/// \sa request
/// \sa status
/// \sa wait_all()
/// \param async_request
/// \return The status of the asynchronous operation
inline status wait(request async_request)
{
    status wait_status;
    MPI_Wait(&async_request, &wait_status);
    return wait_status;
}

/// Wait until all the asynchronous send operation in requests is finished.
/// \sa request
/// \sa status
/// \sa wait()
/// \param async_requests A std::vector of request objects
/// \return A std::vector of statuses of the asynchronous operation
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

/// broadcast sends the local data from the host_processor across the
/// communicator which is defaulted to 0. The result can be read out from the
/// \p host_processor and the slave processors are none the wiser.
/// \tparam T Data type
/// \param local_data Value to send if we are the host_processor
/// \param host_processor Process responsible for sending out the data
/// \param comm MPI communicator
/// \return Broadcasted data
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

/// broadcast sends the local data from the host_processor across the
/// communicator which is defaulted to 0.  The result can be read out from the
/// host_processor and the slave processors are none the wiser.
/// \tparam T Value type
/// \param local_data Value to send if we are the host_processor
/// \param host_processor Process responsible for sending out the data
/// \param comm MPI communicator
/// \return The broadcast data
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

/// reduce computes across the Operation_tp and stores the result in
/// \param host_processor (default 0)
/// The result can be read out only from \p host_processor
/// \param local_data Data to be operated on
/// \param operation_type Reduction operation
/// \param host_processor Host processor index
/// \param comm MPI Communicator
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> reduce(
    T local_data,
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

/// Compute an operation and store the result in \p host_processor (default 0)
/// The result can be read out from the host_processor only.
/// \param local_data Data type to reduce
/// \param operation Type of operation
/// \param host_processor
/// \param comm MPI Communicator
template <typename T, typename operation_type>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> reduce(
    T const& local_data,
    operation_type&& operation,
    int const host_processor = 0,
    communicator const comm = communicator::world)
{
    T collected_data(local_data.size());

    MPI_Reduce(const_cast<typename T::value_type*>(local_data.data()),
               collected_data.data(),
               local_data.size(),
               data_type<typename T::value_type>::value_type(),
               operation.tag,
               host_processor,
               comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

/// Perform an MPI Allreduce operation on types which are able to have primitive
/// operations defined on it.  This function call is only for scalar (primitive)
/// values.
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

/// Perform an MPI Allreduce operation on a vector of types which are able to
/// have primitive operations defined on it.
/// This function allocates a vector of the same type and size of the input
/// vector and fills it with the result from the all_reduce operation
template <typename T, typename Operation_Tp>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> all_reduce(
    T const& local_reduction_variable,
    Operation_Tp&& operation,
    communicator const comm = communicator::world)
{
    T reduction_variable(local_reduction_variable.size());

    MPI_Allreduce(const_cast<typename T::value_type*>(local_reduction_variable.data()),
                  reduction_variable.data(),
                  reduction_variable.size(),
                  data_type<typename T::value_type>::value_type(),
                  operation.tag,
                  comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return reduction_variable;
}

/// Perform an MPI all to all operation on a primitive type.
/// This function allocates a vector of the same type and size of the of the
/// communicator to store the results from the other processors.
template <typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, T> all_to_all(
    T local_data, communicator const comm = communicator::world)
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

/// Perform an MPI all to all operation on a vector of primitive types.
/// This function allocates a vector of the same type and size of the of the
/// communicator to store the results from the other processors.
template <typename T>
inline std::enable_if_t<std::is_arithmetic<typename T::value_type>::value, T> all_to_all(
    T const& local_data, communicator const comm = communicator::world)
{
    T collected_data(local_data.size() * mpi::size(comm));

    MPI_Alltoall(const_cast<typename T::value_type*>(local_data.data()),
                 local_data.size(),
                 data_type<typename T::value_type>::value_type(),
                 collected_data.data(),
                 collected_data.size(),
                 data_type<typename T::value_type>::value_type(),
                 comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF);

    return collected_data;
}

/// Initialise a threaded MPI environment
inline void initialise(int argc, char** argv, thread const thread_required)
{
    // MPI thread level provided
    auto thread_provided = static_cast<int>(thread_required);

    MPI_Init_thread(&argc, &argv, static_cast<int>(thread_required), &thread_provided);
}

/// Initialise a non-threaded MPI environment
inline void initialise(int argc, char** argv) { MPI_Init(&argc, &argv); }

/// Finalise the MPI environment
inline void finalise() { MPI_Finalize(); }

/// Abort the MPI program
/// \param error_code What error code to print
/// \param comm MPI Communicator
inline void abort(int const error_code, communicator const comm = communicator::world)
{
    MPI_Abort(comm == communicator::world ? MPI_COMM_WORLD : MPI_COMM_SELF, error_code);
}

/// Creates an instance of the MPI environment, automatically handling the
/// initialisation and destruction of the MPI environment.  Calls to
/// \p MPI_Init and \p MPI_Finalize are no longer required as these are handled by the
/// class constructor and destructor.
class instance
{
public:
    /// Non-threaded MPI environment
    instance(int argc, char** argv) { mpi::initialise(argc, argv); }

    /// Threaded MPI environment
    instance(int argc, char** argv, thread const thread_required)
    {
        mpi::initialise(argc, argv, thread_required);
    }

    ~instance() { mpi::finalise(); }
};
}
