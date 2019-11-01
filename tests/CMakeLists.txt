find_path(CATCH_INCLUDE_DIR "catch.hpp")
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

file(GLOB_RECURSE TEST_SRC *.cpp)
add_executable(goslayer-unit-tests ${TEST_SRC})
target_link_libraries(goslayer-unit-tests goslayer)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(goslayer-unit-tests)
