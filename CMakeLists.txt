cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(luabridge)

add_subdirectory(lua-5.3.6)
add_subdirectory(luabridge)