
#pragma once

#include <complex>
#include <cstdint>

/// \file data_types.hpp

namespace mpi
{
/*----------------------------------------------------------------------------*
 *               Template specialisations for MPI data types                  *
 *----------------------------------------------------------------------------*/

template <class T>
struct data_type;

template <>
struct data_type<char>
{
    static MPI_Datatype value_type() { return MPI_UNSIGNED_CHAR; }
};

template <>
struct data_type<bool>
{
    static MPI_Datatype value_type() { return MPI_C_BOOL; }
};

template <>
struct data_type<std::int8_t>
{
    static MPI_Datatype value_type() { return MPI_INT8_T; }
};

template <>
struct data_type<std::int16_t>
{
    static MPI_Datatype value_type() { return MPI_INT16_T; }
};

template <>
struct data_type<std::int32_t>
{
    static MPI_Datatype value_type() { return MPI_INT32_T; }
};

template <>
struct data_type<std::int64_t>
{
    static MPI_Datatype value_type() { return MPI_INT64_T; }
};

template <>
struct data_type<std::uint8_t>
{
    static MPI_Datatype value_type() { return MPI_UINT8_T; }
};

template <>
struct data_type<std::uint16_t>
{
    static MPI_Datatype value_type() { return MPI_UINT16_T; }
};

template <>
struct data_type<std::uint32_t>
{
    static MPI_Datatype value_type() { return MPI_UINT32_T; }
};

template <>
struct data_type<std::uint64_t>
{
    static MPI_Datatype value_type() { return MPI_UINT64_T; }
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

template <>
struct data_type<std::complex<float>>
{
    static MPI_Datatype value_type() { return MPI_C_FLOAT_COMPLEX; }
};

template <>
struct data_type<std::complex<double>>
{
    static MPI_Datatype value_type() { return MPI_C_DOUBLE_COMPLEX; }
};

template <>
struct data_type<std::complex<long double>>
{
    static MPI_Datatype value_type() { return MPI_C_LONG_DOUBLE_COMPLEX; }
};
}
