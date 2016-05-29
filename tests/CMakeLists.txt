find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(def_module name units)
	add_executable(${name} ${units})
	include_directories(${CMAKE_SOURCE_DIR})
	target_link_libraries(${name} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
	target_compile_definitions(${name} PRIVATE BOOST_TEST_DYN_LINK)
	add_test(NAME ${name} COMMAND ${name})
endfunction()

enable_testing()
add_subdirectory("machine")
