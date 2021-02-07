# Get the problem name from the current directory
get_filename_component(problem ${CMAKE_CURRENT_SOURCE_DIR} NAME)
cmake_minimum_required(VERSION 2.8.12.2)
Project(${problem} C)
enable_language(C)

add_subdirectory(Homework1)
add_subdirectory(Homework2)