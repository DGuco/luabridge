//
// Created by dguco on 19-12-9.
//

#include <stdio.h>
#include "../src/lua_bridge.h"

int Add(int a, int b)
{
    return a + b;
}

int LuaFnAdd(lua_State *L)
{
    int a = lua_tonumber(L, 1);
    int b = lua_tonumber(L, 2);
    printf("a = %d,b = %d\n", a, b);
    lua_pushnumber(L, a + b);
    return 1;
}

int main(void)
{
    //    luaL_openlibs(L);
//
//    luaL_dofile(L, "../test/111111.lua");
//    lua_register(L, "LuaFnAdd",LuaFnAdd);

    lua_State *L = luaL_newstate();
    CLuaBridge luaBridge(L);
    luaBridge.LoadFile("../test/111111.lua");
    printf("luaState top = %d\n", lua_gettop(luaBridge));
    LuaRegisterCFunc(luaBridge, "Add", int(int, int), Add);
    LuaRegisterLuaFunc(luaBridge, "LuaFnAdd", LuaFnAdd);
    int ret = luaBridge.Call<int>("x11111_test", 1, 2);
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    ret = luaBridge.Call<int>("x11111_test",10,20);
    printf("ret = %d\n",ret);
    printf("-------------------\n");
    ret = luaBridge.Call<int>("x11111_test",100,200);
    printf("ret = %d\n",ret);
    printf("-------------------\n");
    ret = luaBridge.Call<int>("x11111_test",1000,2000);
    printf("ret = %d\n",ret);
    printf("-------------------\n");
    printf("luaState top = %d\n", lua_gettop(luaBridge));
    return 0;
}