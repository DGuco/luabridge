cmake_minimum_required(VERSION 3.6)
project(luabridge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pipe -std=c++11")

set(SOURCE_FILES
        main.cpp
        lua_stack.h
        lua_function.h
        lua_variant.h
        lua_bridge.h
        lua_file.h
        )

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(luabridge ${SOURCE_FILES})
target_link_libraries(luabridge lua dl)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
