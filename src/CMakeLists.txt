file(GLOB_RECURSE ENGINE_SRC *.cpp)
file(GLOB_RECURSE ENGINE_HEADERS *.h)

list(REMOVE_ITEM ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
message(${ENGINE_SRC})

add_library(goslayer ${ENGINE_SRC} ${ENGINE_HEADERS})

add_executable(goslayer-executable main.cpp)

### Communication ###
find_package(RapidJSON)

target_link_libraries(goslayer-executable goslayer)
