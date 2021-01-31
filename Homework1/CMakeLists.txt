# Get the problem name from the current directory
get_filename_component(problem homework1 NAME)

cmake_minimum_required(VERSION 2.8.12.2)

project(${problem} C)

# Add all cpp and h files
file(GLOB SRC_FILES
    "*.h"
    "*.c"
)

add_executable(homework1 ${SRC_FILES})