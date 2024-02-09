cmake_minimum_required(VERSION 3.0.0)
project(lab6 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

link_directories(lib/parameter lib/parser)

add_subdirectory(lib/parameter)
add_subdirectory(lib/parser)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)
