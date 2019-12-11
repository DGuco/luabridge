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
    lua_getglobal(L,"x11111_test");
    int ret = lua_pcall(L,0,0,0);
    if (ret != 0)
    {
        const char* mes = lua_tostring(L,-1);
        printf("pcall error %s\n",mes);
    }
    lua_close(L);
    return 0;
}