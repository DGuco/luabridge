//
// Created by dguco on 19-12-9.
//

#include <stdio.h>
#include "../src/lua_bridge.h"


//int LuaFnAdd(lua_State* l)
//{
//    int a = lua_tonumber(l, 1);
//    int b = lua_tonumber(l, 2);
//    lua_pushnumber(l,a + b);
//    return 1;
//}

int LuaFnAdd(int a,int b)
{
    return a + b;
}

int main(void) {
//    luaL_openlibs(L);
//
//    luaL_dofile(L, "../test/111111.lua");
//    lua_register(L, "LuaFnAdd",LuaFnAdd);

    lua_State *L = luaL_newstate();
    CLuaBridge luaBridge;
    luaBridge.Init(L);
    LuaRegisterFunc(luaBridge,"LuaFnAdd",int(int,int),LuaFnAdd);
    luaBridge.LoadFile("../test/111111.lua");
    int ret = luaBridge.Call<int>("x11111_test");
    printf("ret = %d\n",ret);
    lua_close(L);
    return 0;
}