cmake_minimum_required(VERSION 2.8.12.2)

find_package(MPI)
include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})

add_subdirectory(MatrixMul)
add_subdirectory(QuickSort)