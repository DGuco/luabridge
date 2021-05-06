cmake_minimum_required(VERSION 3.6)
project(lua)

set(CMAKE_C_COMPILER g++)#用c++编译器重新编译lua https://blog.codingnow.com/2015/05/lua_c_api.html(如何正确使用lua api)
set(CMAKE_CXX_COMPILER g++)

add_compile_options(-O2 -Wall -g -pipe -Wextra -std=c++11)

set(LUA_HEADER_FILES
        lapi.h
        lauxlib.h
        lcode.h
        ldebug.h
        lfunc.h
        llimits.h
        ldo.h
        llex.h
        lopcodes.h
        lparser.h
        lmem.h
        lobject.h
        lua.h
        luaconf.h
        lualib.h
        lundump.h
        lstate.h
        lstring.h
        ltable.h
        ltm.h
        lzio.h
        lvm.h
        lgc.h)

set(LUA_SOURCE_FILES
        lapi.c
        lauxlib.c
        lbaselib.c
        lcode.c
        ldblib.c
        ldebug.c
        ldo.c
        ldump.c
        lfunc.c
        lgc.c
        linit.c
        liolib.c
        llex.c
        lmathlib.c
        lmem.c
        loadlib.c
        lobject.c
        lopcodes.c
        loslib.c
        lparser.c
        lstate.c
        lstring.c
        lstrlib.c
        ltable.c
        ltablib.c
        ltm.c
        lundump.c
        lvm.c
        lzio.c
        print.c
        )

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
)

add_library(lua
             ${LUA_HEADER_FILES}
             ${LUA_SOURCE_FILES})

target_link_libraries(lua dl)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
