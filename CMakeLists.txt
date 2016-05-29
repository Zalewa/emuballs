cmake_minimum_required(VERSION 3.5.1)
project(bakingpi-emu)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCC)
	add_definitions(-Wall -Werror=return-type)
endif()

add_subdirectory(src)
add_subdirectory(tests)
