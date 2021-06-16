cmake_minimum_required(VERSION 3.6)
project(luabridge)

set(CMAKE_C_COMPILER g++)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-O0 -Wall -g -pipe -Wextra -Wunused-parameter -std=c++11)
add_definitions(-DLUABRIDGE_CXX11 -DCOMPILE_LUA_WITH_CXX)


set(LUA_BRIDGE_HEADER_FILES
        include/core/func_traits.h
        include/core/classinfo.h
        include/core/constructor.h
        include/core/lua_functions.h
        include/core/lua_exception.h
        include/core/lua_helpers.h
        include/core/lua_ref.h
        include/core/namespace.h
        include/core/security.h
        include/core/type_list.h
        include/core/type_traits.h
        include/core/user_data.h
        include/core/lua_library.h
        include/core/lua_stack.h
        include/core/caller.h
        include/lua_file.h
        include/lua_bridge.h)

set(SOURCE_FILES
        main.cpp)

include_directories(
        /usr/include/
        /usr/local/include/
        ${CMAKE_SOURCE_DIR}/lua-5.3.6/
        ${CMAKE_SOURCE_DIR}/luabridge/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(luabridge
        main.cpp
        ${LUA_BRIDGE_HEADER_FILES}
        )

target_link_libraries(luabridge lua dl)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
