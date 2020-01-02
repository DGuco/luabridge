cmake_minimum_required(VERSION 3.6)
project(lua53)

set(CMAKE_C_COMPILER g++) #用c++编译器重新编译lua https://blog.codingnow.com/2015/05/lua_c_api.html(如何正确使用lua api)
set(CMAKE_CXX_COMPILER g++)

add_compile_options(-O2 -Wall -g -pipe -Wextra -std=c++11)

set(SOURCE_FILES
        src/lapi.h
        src/lapi.c
        src/lauxlib.h
        src/lauxlib.c
        src/lbaselib.c
        src/lcode.h
        src/lcode.c
        src/ldblib.c
        src/ldebug.h
        src/ldebug.c
        src/ldo.h
        src/ldo.c
        src/ldump.c
        src/lfunc.h
        src/lfunc.c
        src/lgc.h
        src/lgc.c
        src/linit.c
        src/liolib.c
        src/llex.h
        src/llex.c
        src/llimits.h
        src/lmathlib.c
        src/lmem.h
        src/lmem.c
        src/loadlib.c
        src/lobject.h
        src/lobject.c
        src/lopcodes.h
        src/lopcodes.c
        src/loslib.c
        src/lparser.h
        src/lparser.c
        src/lstate.h
        src/lstate.c
        src/lstring.h
        src/lstring.c
        src/lstrlib.c
        src/ltable.h
        src/ltable.c
        src/ltablib.c
        src/ltm.h
        src/ltm.c
        src/lua.h
        src/luaconf.h
        src/lualib.h
        src/lundump.h
        src/lundump.c
        src/lvm.h
        src/lvm.c
        src/lzio.h
        src/lzio.c
        src/print.c
        )

include_directories(
        /usr/include/
        /usr/local/include/
)

link_directories(
        /usr/lib/
        /usr/local/lib/
)

add_library(lua ${SOURCE_FILES})
target_link_libraries(lua dl)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
