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
    printf("Global Say = %s\n",contex);
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
    OuterClass()
    {
        printf("OuterClass\n");
    }

    ~OuterClass()
    {
        printf("~OuterClass\n");
    }

    void Say(char* world)
    {
        printf("*****OuterClass::Say: %s******\n",world);
    }
};

int main(void)
{
    const void* key = ClassInfo<int>::getClassKey();
    key = ClassInfo<float>::getConstKey();
    key = ClassInfo<int>::getStaticKey();
    key = ClassInfo<float>::getClassKey();
    key = ClassInfo<int>::getConstKey();
    key = ClassInfo<float>::getStaticKey();
    lua_State* L =  luaL_newstate();
    LuaBridge luaBridge(L);

    luabridge::getGlobalNamespace(L)
        .addCFunction("LuaFnAdd",LuaFnAdd)
        .beginClass <OuterClass> ("OuterClass")
        .addConstructor<void(*)()>()
        .addFunction("Say",&OuterClass::Say)
        .endClass ();
    int top = lua_gettop(L);
    luaBridge.LoadFile("../script/111111.lua");
    int ret = luaBridge.Call<int>("x11111_PrintG");
    printf("ret = %d\n", ret);
    printf("-------------------\n");
    LuaRegisterCFunc(luaBridge, "Add", int(int,int), Add);
    LuaRegisterCFunc(luaBridge, "Sub", int(int,int), Sub);
    LuaRegisterCFunc(luaBridge, "Say", void(const char*), Say);
    LuaRegisterLuaFunc(luaBridge, "LuaFnAdd", LuaFnAdd);
    ret = luaBridge.Call<int>("x11111_callfailedtest", 1, 2,200,100,"Hello lua");
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