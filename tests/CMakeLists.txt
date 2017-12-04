
foreach(test AllReduceTest SendReceiveTest)
    add_executable(${test} ${test}.cpp)

    add_dependencies(${test} catch)

    target_link_libraries(${test} LINK_PUBLIC ${MPI_CXX_LIBRARIES})

    add_test(NAME ${test}
             COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2
             ${CMAKE_CURRENT_BINARY_DIR}/${test})
endforeach()
