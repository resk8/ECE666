cmake_minimum_required(VERSION 2.8.12.2)

find_package(MPI)
include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})

# Get the problem name from the current directory

add_definitions(-DMYDEBUG)
# Add all cpp and h files
file(GLOB SRC_FILES_1 "mpi_quicksort.c")
file(GLOB SRC_FILES_2 "mpi_matrixmul.c")

add_executable(quicksort_d ${SRC_FILES_1})
add_executable(matmult_d ${SRC_FILES_2})

set_target_properties(quicksort_d 
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY 
                      "${CMAKE_SOURCE_DIR}/Homework2/bin")

set_target_properties(matmult_d 
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY 
                      "${CMAKE_SOURCE_DIR}/Homework2/bin")

if(MPI_COMPILE_FLAGS)
  set_target_properties(quicksort_d PROPERTIES
    COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
  set_target_properties(matmult_d PROPERTIES
    COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(quicksort_d PROPERTIES
    LINK_FLAGS "${MPI_C_LINK_FLAGS}")
  set_target_properties(matmult_d PROPERTIES
    LINK_FLAGS "${MPI_C_LINK_FLAGS}")
endif()

target_link_libraries(quicksort_d ${MPI_C_LIBRARIES})
target_link_libraries(matmult_d ${MPI_C_LIBRARIES})
