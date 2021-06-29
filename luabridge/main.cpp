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

int SpaceAdd(int a, int b)
{
    printf("lua call SpaceAdd,a = %d,b = %d\n", a, b);
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
public:
    static  int data;
};


struct OuterClass1
{
    OuterClass1(int a)
    {
        printf("OuterClass1,a = %d\n", a);
    }

    ~OuterClass1()
    {
        printf("~OuterClass1\n");
    }

    void Say(char *world)
    {
        printf("*****OuterClass1::Say: %s******\n", world);
    }
public:
    static  int data;
};

int OuterClass::data = 0;

std::function<int(int, int)> func = [](int a, int b) -> int
{
    printf("c++ Lambda func add a = %d,b = %d\n",a,b);
    return a + b;
};

int main()
{
    lua_State *L = luaL_newstate();
    LuaBridge luaBridge(L);
    luaBridge.LoadFile("../script/111111.lua");

    BEGIN_NAMESPACE_CLASS(luaBridge,"space",OuterClass, "OuterClass")
        CLASS_ADD_CONSTRUCTOR(void(*)(int))
        CLASS_ADD_FUNC("Say",&OuterClass::Say)
    END_NAMESPACE_CLASS

    BEGIN_NAMESPACE(luaBridge,"space")
        REGISTER_SPACE_CFUNC("Add", SpaceAdd)
    END_NAMESPACE

    BEGIN_CLASS(luaBridge, OuterClass, "OuterClass")
            CLASS_ADD_CONSTRUCTOR(void(*)(int))
            CLASS_ADD_FUNC("Say",&OuterClass::Say)
            CLASS_ADD_STATIC_PROPERTY("data",&OuterClass::data)
            LuaHelper::DumpTable(L,-1,std::cout,2);
            LuaHelper::DumpTable(L,-2,std::cout,2);
            LuaHelper::DumpTable(L,-3,std::cout,2);
            LuaHelper::DumpTable(L,-4,std::cout,2);
    END_CLASS

    BEGIN_CLASS(luaBridge, OuterClass1, "OuterClass1")
        CLASS_ADD_CONSTRUCTOR(void(*)(int))
        CLASS_ADD_FUNC("Say",&OuterClass1::Say)
//        LuaHelper::DumpTable(L,-1,std::cout,2);
//        LuaHelper::DumpTable(L,-2,std::cout,2);
//        LuaHelper::DumpTable(L,-3,std::cout,2);
//        LuaHelper::DumpTable(L,-4,std::cout,2);
    END_CLASS

    REGISTER_GLOBAL_CFUNC(luaBridge, "Add", Add)
    REGISTER_GLOBAL_CFUNC(luaBridge, "LambdaAdd", func)
    REGISTER_GLOBAL_CFUNC(luaBridge, "Sub", Sub)
    REGISTER_GLOBAL_CFUNC(luaBridge, "Say", Say)
    REGISTER_GLOBAL_CFUNC(luaBridge, "LuaFnAdd", LuaFnAdd)

    lua_getglobal(L, "_G");
    LuaHelper::DumpTable(L,-1,std::cout,2);
    lua_pop(L,-1);
    int ret = luaBridge.CallLuaFunc<int>("x11111_callfailedtest", 1, 2, 200, 100, "Hello lua");
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