/******************************************************************************
* Name: LuaBridge for C++
*
* Author: DGuco(杜国超)
* Date: 2019-12-07 17:15
* E-Mail: 1139140929@qq.com
*
* Copyright (C) 2019 DGuco(杜国超).  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef  __LUA_STACK_H__
#define  __LUA_STACK_H__

#include <cstring>
#include "lua_file.h"
#include "lua_variant.h"
#include "lua_value.h"

void SetLuaDebugOutput(LuaOutputDebugFunction pfun);

class CLuaStack
{
public:
    CLuaStack(lua_State *luaVM)
        : m_pluaVM(luaVM)
    {}

    CLuaStack(const CLuaStack &) = delete;

    CLuaStack &operator=(const CLuaStack &__x) = delete;

    bool operator!()
    {
        return m_pluaVM == NULL;
    }

    operator bool()
    {
        return m_pluaVM != NULL;
    }

    operator lua_State *()
    {
        return m_pluaVM;
    }

public:
    inline void Push()
    {
        lua_pushnil(m_pluaVM);
    }

    inline void Push(const char *param)
    {
        lua_pushstring(m_pluaVM, param);
    }

    inline void Push(const std::string &param)
    {
        lua_pushlstring(m_pluaVM, param.c_str(), param.size());
    }

    inline void Push(int param)
    {
        lua_pushnumber(m_pluaVM, param);
    }

    inline void Push(int64 param)
    {
        char str[32];
        sprintf(str, "%ld", param);
        lua_pushlstring(m_pluaVM, str, strlen(str));
    }

    inline void Push(double param)
    {
        lua_pushnumber(m_pluaVM, param);
    }

    inline void Push(bool param)
    {
        lua_pushboolean(m_pluaVM, param);
    }

    inline void Push(lua_CFunction param)
    {
        lua_pushcfunction(m_pluaVM, param);
    }

    inline void Push(const LuaTable &param)
    {
        PushTable(param);
    }

    inline void Push(const CLuaVariant &value)
    {
        switch (value.GetType()) {
            case LUAVARIANTTYPE_NUM:
                Push(value.GetValueAsNum());
                break;
            case LUAVARIANTTYPE_STR:
                Push(value.GetValueAsStdStr());
                break;
            case LUAVARIANTTYPE_TABLE:
                PushTable(value.GetValueAsTable());
                break;
            case LUAVARIANTTYPE_BOOL:
                Push(value.GetValueAsBool());
                break;
            case LUAVARIANTTYPE_NIL:
            default:
                Push();
                break;
        }
    }

    template<typename T>
    inline void Push(T *param)
    {
        T **pobj = static_cast<T **>( lua_newuserdata(m_pluaVM, sizeof(T *)));
        *pobj = param;

        luaL_getmetatable(m_pluaVM, T::GetLuaTypeName());
        lua_setmetatable(m_pluaVM, -2);
    }

public:
    template<typename R>
    inline R Pop(int count = 1)
    {
        R temp = GetValue<R>(-count);
        lua_settop(m_pluaVM, (-count) - 1);
        return temp;
    }

    inline void Pop(int count = 1)
    {
        lua_settop(m_pluaVM, (-count) - 1);
    }

public:
    int GetParamCount()
    {
        return lua_gettop(m_pluaVM);
    }

    template<typename R>
    R GetParam(int id)
    {
        int nTop = GetParamCount();
        LuaAssert(id != 0 && id <= nTop && id >= -nTop, id, "Not enough parameters");
        return GetValue<R>(id);
    }

    inline void LuaAssert(bool condition, int argindex, const char *err_msg)
    {
        luaL_argcheck(m_pluaVM, condition, argindex, err_msg);
    }

    inline void LuaAssert(bool condition, const char *err_msg)
    {
        if (!condition) {
            lua_Debug ar;
            lua_getstack(m_pluaVM, 0, &ar);
            lua_getinfo(m_pluaVM, "n", &ar);
            if (ar.name == NULL)
                ar.name = "?";
            luaL_error(m_pluaVM, "assert fail: %s `%s' (%s)", ar.namewhat, ar.name, err_msg);
        }
    }

    template<typename R>
    R GetGlobal(const char *name)
    {
        lua_getglobal(m_pluaVM, name);
        return Pop<R>();
    }

    template<typename R>
    void SetGlobal(const char *name, R value)
    {
        Push(value);
        lua_setglobal(m_pluaVM, name);
    }

//	template<class T>
//	void DelGlobalObject(const char* name)
//	{
//		lua_getglobal(m_pluaVM, name);
//		LuaDelObject<T>(m_pluaVM);
//		lua_settop(m_pluaVM, -2);
//
//		lua_pushnil(m_pluaVM);
//		lua_setglobal(m_pluaVM, name);
//	}
public:
    template<typename R>
    inline R GetValue(int index = -1)
    {
        return CLuaValue<R>()(m_pluaVM, index);
    }

    inline LuaTable GetTable(int index)
    {
        return CLuaValue<LuaTable>()(m_pluaVM, index);
    }

    inline void PushTable(const LuaTable &table)
    {
        lua_newtable(m_pluaVM);
        LuaTable::const_iterator end = table.end();
        for (LuaTable::const_iterator i = table.begin(); i != end; ++i) {
            Push(i->first);
            Push(i->second);
            lua_settable(m_pluaVM, -3);
        }
    }

protected:
    inline void SafeBeginCall(const char *func)
    {
        //记录调用前的堆栈索引
        m_iTopIndex = lua_gettop(m_pluaVM);
        lua_getglobal(m_pluaVM, func);
    }

    template<typename R, int __>
    R SafeEndCall(const char *func, int nArg)
    {
        if (lua_pcall(m_pluaVM, nArg, 1, 0) != 0) {
            DefaultDebugLuaErrorInfo(func,lua_tostring(m_pluaVM, -1));
            //Pop(); // Pop error message
            return 0;
        }

        R r =  Pop<R>();
        //恢复调用前的堆栈索引
        lua_settop(m_pluaVM,m_iTopIndex);
        return r;
    }

    template<int __>
    void SafeEndCall(const char *func, int nArg)
    {
        if (lua_pcall(m_pluaVM, nArg, 0, 0) != 0) {
            DefaultDebugLuaErrorInfo(func,lua_tostring(m_pluaVM, -1));
        }
        lua_settop(m_pluaVM,m_iTopIndex);
    }
protected:
    lua_State *m_pluaVM;
    int m_iTopIndex;
};


template<class T>
int LuaNewObject(lua_State *L)
{
    T *obj = new T;
    T **pobj = static_cast<T **>( lua_newuserdata((L), sizeof(T *)));
    *pobj = obj;

    luaL_getmetatable(L, T::GetLuaTypeName());
    lua_setmetatable(L, -2);

    return 1;  /* new userdatum is already on the stack */
}

template<class T>
int LuaDelObject(lua_State *L)
{
    T **pobj = static_cast<T **>( luaL_checkudata(L, -1, T::GetLuaTypeName()));
    luaL_argcheck(L, pobj != NULL, -1, "Ojbect type missing");

    delete *pobj;
    *pobj = NULL;

    return 0;
}

template<const char *GetLuaTypeName()>
int Lua_ImplementIndex(lua_State *L)
{
    int narg = lua_gettop(L);
    if (lua_isstring(L, -1)) {
        const char *index = lua_tostring(L, -1);
        luaL_getmetatable(L, GetLuaTypeName());
        lua_pushstring(L, index);
        lua_gettable(L, -2);
        lua_remove(L, -2);
        if (!lua_isnil(L, -1))
            return 1;
        lua_settop(L, -2);
    }
    int narg2 = lua_gettop(L);

    luaL_getmetatable(L, GetLuaTypeName());
    lua_pushstring(L, "__getindex");
    lua_gettable(L, -2);
    lua_remove(L, -2);
    if (!lua_isnil(L, -1)) {
        for (int i = 1; i <= narg; ++i)
            lua_pushvalue(L, i);
        lua_pcall(L, narg, LUA_MULTRET, 0);
        int nnewtop = lua_gettop(L);
        return lua_gettop(L) - narg;
    }
    return 1;
}
#endif  //__LUA_STACK_H__