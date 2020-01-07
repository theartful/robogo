find_path(CATCH_INCLUDE_DIR "catch.hpp")
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

file(GLOB_RECURSE TEST_SRC *.cpp)
add_executable(robogo-unit-tests ${TEST_SRC})
target_link_libraries(robogo-unit-tests robogo)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(robogo-unit-tests)
