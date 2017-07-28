set(NAME emurun)

set(SOURCES emurun.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} emuballs)
