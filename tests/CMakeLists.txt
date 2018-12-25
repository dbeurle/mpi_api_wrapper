
foreach(test all_reduce send_receive broadcast custom_data)
    add_executable(${test} ${test}.cpp)

    add_dependencies(${test} catch)

    target_link_libraries(${test} LINK_PUBLIC ${MPI_CXX_LIBRARIES} mpi_api)

    if(ENABLE_COVERAGE)
        set_target_properties(${test} PROPERTIES COMPILE_FLAGS "--coverage")
        target_link_libraries(${test} PRIVATE --coverage)
    endif()

    add_test(NAME ${test}
             COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2
             ${CMAKE_CURRENT_BINARY_DIR}/${test})
endforeach()
