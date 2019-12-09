file(GLOB_RECURSE ENGINE_SRC *.cpp *.c)
file(GLOB_RECURSE ENGINE_HEADERS *.h)

list(REMOVE_ITEM ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
message(${ENGINE_SRC})

add_library(goslayer ${ENGINE_SRC} ${ENGINE_HEADERS})

add_executable(goslayer-executable main.cpp)

# use Websocketpp as depandancy
find_package(Boost REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
  message(FATAL_ERROR)
endif()

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(Websocketpp)
if(NOT Websocketpp_FOUND)
  message(FATAL_ERROR "websocketpp library not found you can install it from here: https://github.com/zaphoyd/websocketpp.git")
endif()

target_link_libraries(goslayer-executable goslayer ${Boost_LIBRARIES})
set_property(TARGET goslayer-executable APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
