//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2007, Nathan Reed

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#ifndef __LUA_HELPERS_H__
#define __LUA_HELPERS_H__

#include "lua_library.h"
#include <cassert>
#include <cstring>

namespace luabridge
{

// These are for Lua versions prior to 5.2.0.
//
#if LUA_VERSION_NUM < 502
inline int lua_absindex(lua_State *L, int idx)
{
    if (idx > LUA_REGISTRYINDEX && idx < 0)
        return lua_gettop(L) + idx + 1;
    else
        return idx;
}

inline void lua_rawgetp(lua_State *L, int idx, void const *p)
{
    idx = lua_absindex(L, idx);
    lua_pushlightuserdata(L, const_cast <void *> (p));
    lua_rawget(L, idx);
}

inline void lua_rawsetp(lua_State *L, int idx, void const *p)
{
    idx = lua_absindex(L, idx);
    lua_pushlightuserdata(L, const_cast <void *> (p));
    // put key behind value
    lua_insert(L, -2);
    lua_rawset(L, idx);
}

#define LUA_OPEQ 1
#define LUA_OPLT 2
#define LUA_OPLE 3

inline int lua_compare(lua_State *L, int idx1, int idx2, int op)
{
    switch (op) {
    case LUA_OPEQ:return lua_equal(L, idx1, idx2);
        break;

    case LUA_OPLT:return lua_lessthan(L, idx1, idx2);
        break;

    case LUA_OPLE:return lua_equal(L, idx1, idx2) || lua_lessthan(L, idx1, idx2);
        break;

    default:return 0;
    };
}

inline int get_length(lua_State *L, int idx)
{
    return int(lua_objlen(L, idx));
}

#else
inline int get_length(lua_State *L, int idx)
{
    lua_len(L, idx);
    int len = int(luaL_checknumber(L, -1));
    lua_pop (L, 1);
    return len;
}

#endif

#ifndef LUA_OK
# define LUABRIDGE_LUA_OK 0
#else
# define LUABRIDGE_LUA_OK LUA_OK
#endif

/** Get a table value, bypassing metamethods.
*/
inline void rawgetfield(lua_State *L, int index, char const *key)
{
    assert (lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_rawget(L, index);
}

/** Set a table value, bypassing metamethods.
*/
inline void rawsetfield(lua_State *L, int index, char const *key)
{
    assert (lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

/** Returns true if the value is a full userdata (not light).
*/
inline bool isfulluserdata(lua_State *L, int index)
{
    return lua_isuserdata(L, index) && !lua_islightuserdata (L, index);
}

/** Test lua_State objects for global equality.

    This can determine if two different lua_State objects really point
    to the same global state, such as when using coroutines.

    @note This is used for assertions.
*/
inline bool equalstates(lua_State *L1, lua_State *L2)
{
    return lua_topointer(L1, LUA_REGISTRYINDEX) ==
        lua_topointer(L2, LUA_REGISTRYINDEX);
}

class LuaHelper
{
public:
    /**
     * 检测lua 堆栈中的参数类型
     * @param Index
     * @param isLuaError 如果类型错误是否抛出lua异常
     * @return
     */
    static bool CheckLuaArg_Num(lua_State *L, int Index);
    static bool CheckLuaArg_Integer(lua_State *L, int Index);
    static bool CheckLuaArg_Str(lua_State *L, int Index);
    /**
     * print lua stack info 打印lua 堆栈信息
     * @param L
     */
    static void LuaStackInfo(lua_State *L);
    /**
     * Lua Asset,if asset failed  throw a lua lua error
     * @param condition
     * @param argindex
     * @param err_msg
     */
    static void LuaAssert(lua_State *L, bool condition, const char *err_msg);

    /**
     * param count
     * @return
     */
    static int GetParamCount(lua_State *L);
    /**
     * dbug lua error info 打印lua 错误日志message
     * @param FunName
     * @param Msg
     */
    static void DefaultDebugLuaErrorInfo(const char *FunName, const char *Msg);
};

void LuaHelper::LuaStackInfo(lua_State *L)
{
    printf("==========================Lua stack info start=====================================\n");
    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    for (int i = 0;; i++) {
        if (lua_getstack(L, i, &trouble_info) == 0)
            break;
        lua_getinfo(L, "Snl", &trouble_info);

        printf("name[%s] what[%s] short_src[%s] linedefined[%d] currentline[%d]\n",
               trouble_info.name,
               trouble_info.what,
               trouble_info.short_src,
               trouble_info.linedefined,
               trouble_info.currentline);
    }
    printf("==========================Lua stack info end=====================================\n");

}

bool LuaHelper::CheckLuaArg_Num(lua_State *L, int Index)
{
    if (lua_isnumber(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (lua_isnil(L, Index)) {
        printf("Lua function[%s], arg[%d] is null \n", trouble_info.name, Index);
    }
    else {
        printf("Lua function[%s], arg[%d] type error not number \n", trouble_info.name, Index);
    }

    LuaStackInfo(L);
    return false;
}

bool LuaHelper::CheckLuaArg_Integer(lua_State *L, int Index)
{
    if (lua_isinteger(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (lua_isnil(L, Index)) {
        printf("Lua function[%s], arg[%d] is null \n", trouble_info.name, Index);
    }
    else {
        printf("Lua function[%s], arg[%d] type error not integer \n", trouble_info.name, Index);
    }

    LuaStackInfo(L);
    return false;
}

bool LuaHelper::CheckLuaArg_Str(lua_State *L, int Index)
{
    if (lua_isstring(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 1, &trouble_info) || lua_getstack(L, 0, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (lua_isnil(L, Index)) {
        printf("Lua function[%s], arg[%d] is null \n", trouble_info.name, Index);
    }
    else {
        printf("Lua function[%s], arg[%d] type error not str \n", trouble_info.name, Index);
    }

    LuaStackInfo(L);
    return false;
}

void LuaHelper::LuaAssert(lua_State *L, bool condition, const char *err_msg)
{
    if (!condition) {
        lua_Debug ar;
        lua_getstack(L, 0, &ar);
        lua_getinfo(L, "n", &ar);
        if (NULL == ar.name) {
            ar.name = "?";
        }
        if (NULL == ar.namewhat) {
            ar.namewhat = "?";
        }
        /*
 *@param check:
 * lua_toxxx和luaL_checkxxx的区别，这两个函数都是从lua栈上获取一个值，但是在检查到类型不符时候，lua_toxxx只是返回null或者默认值；
 * 而luaL_check则是会抛出一个异常，下面的代码不会再继续执行；这里就需要注意了，lua里面使用的异常并不是c++的异常，只是使用了c的setjump和longjump来实现到恢复点的跳转，
 * 所以并不会有C++所期望的栈的展开操作，所以在C++里面看来是异常安全的代码，此时也是“不安全”的，也不能保证异常安全，比如
 * Function1(lua_state state)
 * {
     TestClass tmp();
     luaL_checkstring(state,1);
 * }
 * 当上面的luaL_checkstring出现异常时候，TestClass的析构函数并不会被调用，假如你需要在析构函数里面释放一些资源，可能会导致资源泄露、锁忘记释放等问题。
 * 所以在使用luaL_checkxxx时候，需要很小心，在luaL_checkxxx之前尽量不要申请一些需要之后释放的资源，尤其是加锁函数,智能指针和auto锁也不能正常工作。
 * 如果用g++重新编译lua源码不会有问题
 **/
#ifdef COMPILE_LUA_WITH_CXX
        luaL_error(L, "assert fail: %s `%s' (%s)", ar.namewhat, ar.name, err_msg);
#else
        char Msg[512];
        sprintf(Msg,"assert fail: %s `%s' (%s)",ar.namewhat, ar.name, err_msg);
        throw std::runtime_error("Msg");
#endif

    }
}

int LuaHelper::GetParamCount(lua_State *L)
{
    return lua_gettop(L);
}

void LuaHelper::DefaultDebugLuaErrorInfo(const char *FunName, const char *Msg)
{
    printf("Call fun:[%s] failed,msg:[%s]\n", FunName, Msg);
}

} // namespace luabridge

#endif