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

int Add(int a,int b)
{
    return a + b;
}


int LuaFnAdd(lua_State *L)
{
    int a = lua_tonumber(L,1);
    int b = lua_tonumber(L,2);
    printf("a = %d,b = %d\n",a,b);
    lua_pushnumber(L,a + b);
    return 1;
}

int main(void) {
//    luaL_openlibs(L);
//
//    luaL_dofile(L, "../test/111111.lua");
//    lua_register(L, "LuaFnAdd",LuaFnAdd);

    lua_State *L = luaL_newstate();
    CLuaBridge luaBridge;
    luaBridge.Init(L);
    //LuaRegisterFunc(luaBridge,"Add",int(int,int),Add);
    luaBridge.LoadFile("../test/111111.lua");
    LuaRegisterLuaFunc(luaBridge,"LuaFnAdd",LuaFnAdd);
    int ret = luaBridge.Call<int>("x11111_test");
    printf("ret = %d\n",ret);
    return 0;
}