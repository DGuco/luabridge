cmake_minimum_required(VERSION 3.6)
project(luabridge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/lua_stack.h
        src/lua_function.h
        src/lua_variant.cpp
        src/lua_bridge.cpp
        src/lua_bridge.h
        src/lua_file.h)

include_directories(
        /usr/
        /usr/local/
        /usr/local/in
)

link_directories(
        /usr/lib/
        /usr/local/lib/
)

add_executable(luabridge ${SOURCE_FILES})
target_link_libraries(luabridge pthread lua)
