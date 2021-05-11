//
// Created by dguco on 19-12-9.
//

#include <stdio.h>
#include "lua_bridge.h"

using namespace luabridge;

int Add(int a, int b)
{
    printf("lua call Add,a = %d,b = %d\n",a,b);
    return a + b;
}

int Sub(int a, int b)
{
    printf("lua call Sub,a = %d,b = %d\n",a,b);
    return a - b;
}

void Say(const char *contex)
{
    printf("Say = %s\n",contex);
}

class Test
{
public:
    Test()
    {
        printf("Test()\n");
    }
    ~Test()
    {
        printf("~Test()\n");
    }
};

int LuaFnAdd(lua_State *L)
{
    Test* test;
    int a = Stack<int>::get(L, 1);
    int b = Stack<int>::get(L, 2);
    printf("!!!!a = %d,b = %d !!!!!\n", a, b);
    lua_pushinteger(L, a + b);
    return 1;
//    try {
//        int a = Stack<int>::get(L, 1);
//        int b = Stack<int>::get(L, 2);
//        printf("a = %d,b = %d\n", a, b);
//        lua_pushinteger(L, a + b);
//        return 1;
//    }catch (std::exception& e)
//    {
//        std::cout << "LuaFnAdd catch exception: " << e.what() << std::endl;
//        return 0;
//    }
}

int main(void)
{
    lua_State* L =  luaL_newstate();
    LuaBridge luaBridge(L);
    luaBridge.LoadFile("../script/111111.lua");
    luabridge::getGlobalNamespace (L);
    LuaRegisterCFunc(luaBridge, "Add", int(int,int), Add);
    LuaRegisterCFunc(luaBridge, "Sub", int(int,int), Sub);
    LuaRegisterCFunc(luaBridge, "Say", void(const char*), Say);
    LuaRegisterLuaFunc(luaBridge, "LuaFnAdd", LuaFnAdd);
    int ret = luaBridge.Call<int>("x11111_test1", 1, 2,200,100,"Hello lua");
    printf("retMsg = %s\n", ret);
    printf("-------------------\n");
    ret = luaBridge.Call<int>("x11111_test", 1, 2);
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