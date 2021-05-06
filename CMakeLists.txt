cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_COMPILER g++)

project(lbridge)

add_subdirectory(lua-5.1.5)
add_subdirectory(luabridge)
