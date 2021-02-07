# Get the problem name from the current directory
get_filename_component(problem homework2 NAME)

cmake_minimum_required(VERSION 2.8.12.2)


project(${problem} C)
find_package(MPI)

# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)
add_executable(${problem} ${SRC_FILES})

include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})
if(MPI_COMPILE_FLAGS)
  set_target_properties(${problem} PROPERTIES
    COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(${problem} PROPERTIES
    LINK_FLAGS "${MPI_C_LINK_FLAGS}")
endif()



target_link_libraries(${problem} ${MPI_C_LIBRARIES})