# Get the problem name from the current directory
get_filename_component(problem homework1 NAME)

cmake_minimum_required(VERSION 2.8.12.2)

project(${problem} C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(${problem} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${problem} "${CMAKE_THREAD_LIBES_INIT}")
endif()

# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)

add_executable(${problem} ${SRC_FILES})