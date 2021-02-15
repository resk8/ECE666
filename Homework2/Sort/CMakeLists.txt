# Get the problem name from the current directory
get_filename_component(problem quicksort NAME)
#project(${problem} C)

add_definitions(-DMYDEBUG)
# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)

add_executable(${problem} ${SRC_FILES})
set_target_properties(${problem} 
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY 
                      "${CMAKE_SOURCE_DIR}/Homework2/Sort/bin")

if(MPI_COMPILE_FLAGS)
  set_target_properties(${problem} PROPERTIES
    COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(${problem} PROPERTIES
    LINK_FLAGS "${MPI_C_LINK_FLAGS}")
endif()

target_link_libraries(${problem} ${MPI_C_LIBRARIES})