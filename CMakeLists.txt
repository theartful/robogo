cmake_minimum_required(VERSION 3.8)
project(robogo)

option(ENABLE_TESTING "Enable testing" OFF)
option(REPORT_COVERAGE "Report coverage" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake.modules/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# set default build type
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug" CACHE STRING
		"Choose the type of build, options are: Debug Release" FORCE)
endif()

# set NDEBUG
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(NDEBUG 0)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(NDEBUG 1)
endif()

# configure config.h
configure_file(
	"${PROJECT_SOURCE_DIR}/src/config.h.in"
	"${PROJECT_BINARY_DIR}/src/config.h"
)

include_directories(
	"${PROJECT_SOURCE_DIR}/src/"
	"${PROJECT_BINARY_DIR}/src/"
)

# use c++17 (trying out new features!)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(COMPILER_WARNINGS
		"-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
		-Wdisabled-optimization -Winit-self -Wstrict-overflow=5 \
		-Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Wshadow \
		-Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo \
		-Wundef -Wswitch-default -Werror -Wno-unused")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	if (REPORT_COVERAGE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
		include(CodeCoverage)
		setup_target_for_coverage_lcov(
			NAME coverage
			EXCLUDE
				"${CMAKE_SOURCE_DIR}/tests/**"
				"${CMAKE_SOURCE_DIR}/src/main.cpp"
				"/usr/include/gtest/**"
				"/usr/include/x86_64-linux-gnu/c++/9/bits/**"
				"/usr/include/c++/**"
			EXECUTABLE ctest)
	endif()
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_WARNINGS} ${CMAKE_CXX_FLAGS}")

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

################################### testing ###################################
if (ENABLE_TESTING)
	enable_testing()
	add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()
