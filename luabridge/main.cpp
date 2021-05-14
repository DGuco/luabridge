//
// Created by dguco on 19-12-9.
//

#include <stdio.h>
#include <functional>
#include "lua_bridge.h"

using namespace luabridge;

int Add(int a, int b)
{
    printf("lua call Add,a = %d,b = %d\n", a, b);
    return a + b;
}

int Sub(int a, int b)
{
    printf("lua call Sub,a = %d,b = %d\n", a, b);
    return a - b;
}

void Say(const char *contex)
{
    printf("Global Say = %s\n", contex);
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
    int a = Stack<int>::get(L, 1);
    int b = Stack<int>::get(L, 2);
    printf("!!!!a = %d,b = %d !!!!!\n", a, b);
    lua_pushinteger(L, a + b);
    return 1;
}

struct OuterClass
{
    OuterClass(int a)
    {
        printf("OuterClass,a = %d\n", a);
    }

    ~OuterClass()
    {
        printf("~OuterClass\n");
    }

    void Say(char *world)
    {
        printf("*****OuterClass::Say: %s******\n", world);
    }
};

static std::function<int(int, int)> func = [](int a, int b) -> int
{
    printf("c++ Lambda  func add a = %d,b = %d",a,b);
    return a + b;
};

int main(void)
{
    lua_State *L = luaL_newstate();
    LuaBridge luaBridge(L);

    luabridge::getGlobalNamespace(L)
        .addCFunction("LuaFnAdd", LuaFnAdd)
        .beginClass<OuterClass>("OuterClass")
        .addConstructor <void(*)(int)>()
        .addFunction("Say", &OuterClass::Say)
        .endClass();
    int top = lua_gettop(L);
    luaBridge.LoadFile("../script/111111.lua");
    int ret = luaBridge.Call<int>("x11111_PrintG");
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    LuaRegisterCFunc(luaBridge, "Add", Add);
    LuaRegisterCFunc(luaBridge, "LambdaAdd", func);
    LuaRegisterCFunc(luaBridge, "Sub", Sub);
    LuaRegisterCFunc(luaBridge, "Say", Say);
    LuaRegisterLuaFunc(luaBridge, "LuaFnAdd", LuaFnAdd);
    ret = luaBridge.Call<int>("x11111_callfailedtest", 1, 2, 200, 100, "Hello lua");
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    ret = luaBridge.Call<int>("x11111_test", 1, 2);
    printf("ret = %d\n", ret);
//    printf("-------------------\n");
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