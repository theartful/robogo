include(GoogleTest)
find_package(Threads REQUIRED)

file(GLOB SRC_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_executable(robogo-tests ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(robogo-tests robogo Threads::Threads gtest gtest_main)
gtest_discover_tests(robogo-tests
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
