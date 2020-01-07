file(GLOB_RECURSE ENGINE_SRC *.cpp)
file(GLOB_RECURSE ENGINE_HEADERS *.h)

list(REMOVE_ITEM ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
message(${ENGINE_SRC})

add_library(robogo ${ENGINE_SRC} ${ENGINE_HEADERS})

add_executable(robogo-executable main.cpp)
target_link_libraries(robogo-executable robogo)
