include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(machine STATIC
	machine/memory.cpp)
