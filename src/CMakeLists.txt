file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(robogo ${SRC_FILES} ${HEADER_FILES})
add_executable(robogo-executable main.cpp)
target_link_libraries(robogo-executable robogo)
