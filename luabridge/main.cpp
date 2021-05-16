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
    printf("LuaFnAdd call ,a = %d,b = %d\n", a, b);
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

std::function<int(int, int)> func = [](int a, int b) -> int
{
    printf("c++ Lambda func add a = %d,b = %d\n",a,b);
    return a + b;
};

int main(void)
{
    lua_State *L = luaL_newstate();
    LuaBridge luaBridge(L);
    luaBridge.LoadFile("../script/111111.lua");

    BEGIN_NAMESPACE_CLASS("space",luaBridge,OuterClass,"OuterClass")
        CLASS_ADD_CONSTRUCTOR(void(*)(int))
        CLASS_ADD_FUNC("Say",&OuterClass::Say)
        LuaHelper::DumpTable(L,-1,std::cout);
        LuaHelper::DumpTable(L,-2,std::cout);
        LuaHelper::DumpTable(L,-3,std::cout);
        LuaHelper::DumpTable(L,-4,std::cout);
        LuaHelper::DumpTable(L,-5,std::cout);
    END_NAMESPACE_CLASS

    BEGIN_CLASS(luaBridge,OuterClass,"OuterClass")
            CLASS_ADD_CONSTRUCTOR(void(*)(int))
            CLASS_ADD_FUNC("Say",&OuterClass::Say)
    END_CLASS

    REGISTER_LUA_CFUNC(luaBridge, "Add", Add)
    REGISTER_LUA_CFUNC(luaBridge, "LambdaAdd", func)
    REGISTER_LUA_CFUNC(luaBridge, "Sub", Sub)
    REGISTER_LUA_CFUNC(luaBridge, "Say", Say)
    REGISTER_LUA_CFUNC(luaBridge, "LuaFnAdd", LuaFnAdd)

    int ret = luaBridge.CallLuaFunc<int>("x11111_PrintG");
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    ret = luaBridge.CallLuaFunc<int>("x11111_callfailedtest", 1, 2, 200, 100, "Hello lua");
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    ret = luaBridge.CallLuaFunc<int>("x11111_test", 1, 2);
    printf("ret = %d\n", ret);
//    printf("-------------------\n");
//    ret = luaBridge.CallLuaFunc<int>("x11111_test",10,20);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
//    ret = luaBridge.CallLuaFunc<int>("x11111_test",100,200);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
//    ret = luaBridge.CallLuaFunc<int>("x11111_test",1000,2000);
//    printf("ret = %d\n",ret);
//    printf("-------------------\n");
    return 0;
}