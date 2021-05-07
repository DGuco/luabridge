//
// Created by dguco on 19-12-9.
//

#include <stdio.h>
#include "lua_bridge.h"

using namespace luabridge;

int Add(int a, const char* b)
{
    return a;
}

int LuaFnAdd(lua_State *L)
{
    try {
        int a = Stack<int>::get(L, 1);
        int b = Stack<int>::get(L, 2);
        printf("a = %d,b = %d\n", a, b);
        lua_pushinteger(L, a + b);
        return 1;
    }catch (std::exception e)
    {
        std::cout << "LuaFnAdd catch exception: " << e.what() << std::endl;
        return 0;
    }
}

int main(void)
{
    LuaBridge luaBridge(luaL_newstate());
    luaBridge.LoadFile("../script/111111.lua");
    LuaRegisterCFunc(luaBridge, "Add", int(int, const char*), Add);
    LuaRegisterLuaFunc(luaBridge, "LuaFnAdd", LuaFnAdd);
    int ret = luaBridge.Call<int>("x11111_test", 1, 2);
    printf("ret = %d\n", ret);
    printf("-------------------\n");
//    ret = luaBridge.Call<int>("x11111_test",10,20);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
//    ret = luaBridge.Call<int>("x11111_test",100,200);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
//    ret = luaBridge.Call<int>("x11111_test",1000,2000);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
    return 0;
}