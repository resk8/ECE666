# Get the problem name from the current directory
get_filename_component(problem homework1 NAME)
#project(${problem} C) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -O1")
find_package(Threads REQUIRED)
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${problem} "${CMAKE_THREAD_LIBES_INIT}")
endif()

# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)

add_executable(${problem} ${SRC_FILES})