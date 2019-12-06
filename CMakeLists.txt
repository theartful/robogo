cmake_minimum_required(VERSION 3.8)
project(goslayer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake.modules/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# set default build type
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "DEBUG" CACHE STRING
	     "Choose the type of build, options are: Debug Release" FORCE)
endif()
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

# set NDEBUG
if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	set(NDEBUG 0)
elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
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

# use c++17
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(COMPILER_WARNINGS
	    "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
	    -Wdisabled-optimization -Wformat=2 -Winit-self -Wstrict-overflow=5 \
	    -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Wshadow \
	    -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo \
	    -Wundef -Wswitch-default -Werror -Wno-unused")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
elseif (MSVC)
	# check that MSVC supports c++17
	if (MSVC_VERSION GREATER_EQUAL "1900")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG("/std:c++17" _cpp_latest_flag_supported)
		if (_cpp_latest_flag_supported)
			add_compile_options("/std:c++17")
		endif()
	else()
		message(FATAL_ERROR "C++17 is required to build goslayer, but it isn't \
		        supported by this compiler")
	endif()
	set(COMPILER_WARNINGS "/Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "/Ot")
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_WARNINGS} ${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

################ testing ###############
option(ENABLE_TESTING "Enable testing" OFF)

if (ENABLE_TESTING)
	enable_testing()
	add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()
