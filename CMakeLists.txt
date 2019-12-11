cmake_minimum_required(VERSION 3.6)
project(luabridge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        test/main.cpp
        src/lua_stack.h
        src/lua_function.h
        src/lua_variant.h
        src/lua_bridge.h
        src/lua_file.h
        src/lua_value.h)

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
)

add_executable(luabridge ${SOURCE_FILES})
target_link_libraries(luabridge lua dl)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
