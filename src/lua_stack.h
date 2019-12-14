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

#define int64 long long

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

public:
    template<typename R>
    inline R GetValue(int index = -1)
    {
        return CLuaValue<R,0>()(*this, index);
    }

    inline LuaTable GetTable(int index)
    {
        return CLuaValue<LuaTable,0>()(*this, index);
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

    /**
     * 检测lua 堆栈中的参数类型
     * @param Index
     * @param isLuaError 如果类型错误是否抛出lua异常
     * @return
     */
    bool CheckLuaArg_Num(int Index, bool isLuaError = true);
    bool CheckLuaArg_Str(int Index, bool isLuaError = true);
    /**
     * print lua stack info 打印lua 堆栈信息
     * @param L
     */
    void LuaStackInfo();
    /**
     * Lua Asset,if asset failed  throw a lua lua error
     * @param condition
     * @param argindex
     * @param err_msg
     */
    void LuaAssert(bool condition, int argindex, const char *err_msg);
    void LuaAssert(bool condition, const char *err_msg);
    /**
     * dbug lua error info 打印lua 错误日志message
     * @param FunName
     * @param Msg
     */
    void DefaultDebugLuaErrorInfo(const char *FunName, const char *Msg);
    template<class T>
    int LuaNewObject(lua_State *L);

    template<class T>
    int LuaDelObject(lua_State *L);

    template<const char *GetLuaTypeName()>
    int LuaImplementIndex(lua_State *L);

	template<class T>
	void DelGlobalObject(const char* name)
	{
		lua_getglobal(m_pluaVM, name);
		LuaDelObject<T>(m_pluaVM);
		lua_settop(m_pluaVM, -2);

		lua_pushnil(m_pluaVM);
		lua_setglobal(m_pluaVM, name);
	}
private:
    template<typename R,int __>
    struct CLuaValue
    {
        R operator()(CLuaStack& L, int index)
        {
            throw std::runtime_error("Bad  type for CLuaValue");
        }
    };

    template<int __>
    struct CLuaValue<float,__>
    {
        float operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return static_cast<float>(lua_tonumber(L, index));
        }
    };

    template<int __>
    struct CLuaValue<double,__>
    {
        double operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return lua_tonumber(L, index);
        }
    };

    template<int __>
    struct CLuaValue<long double,__>
    {
        long double operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return lua_tonumber(L, index);
        }
    };

    template<int __>
    struct CLuaValue<int,__>
    {
        int operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return static_cast<int>(lua_tonumber(L, index));
        }
    };

    template<int __>
    struct CLuaValue<long,__>
    {
        long operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return static_cast<long>(lua_tonumber(L, index));
        }
    };

    template<int __>
    struct CLuaValue<int64,__>
    {
        int64 operator()(CLuaStack& L, int index)
        {
            if (!L.CheckLuaArg_Str(index))
                return 0;

            return atol(lua_tostring(L, index));
        }
    };

    template<int __>
    struct CLuaValue<const char *,__>
    {
        const char *operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Str(index);
            return lua_tostring(L, index);
        }
    };

    template<int __>
    struct CLuaValue<std::string,__>
    {
        std::string operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Str(index);
            return std::string(lua_tostring(L, index), lua_strlen(L, index));
        }
    };

    template<int __>
    struct CLuaValue<bool,__>
    {
        bool operator()(CLuaStack& L, int index)
        {
            L.CheckLuaArg_Num(index);
            return lua_toboolean(L, index) != 0;
        }
    };

    template<typename R,int __>
    struct CLuaValue<R *,__>
    {
        R *operator()(CLuaStack& L, int index)
        {
            R **pobj = static_cast<R **>( luaL_checkudata(L, index, R::GetLuaTypeName()));
            luaL_argcheck(L, pobj != NULL && *pobj != NULL, index, "Invalid object");
            //pop one from stack
            lua_settop(L, (-1) - 1);
            return *pobj;
        }
    };

    template<int __>
    struct CLuaValue<CLuaVariant,__>
    {
        LuaTable GetTable(CLuaStack& L,int index)
        {
            LuaTable table;
            lua_pushvalue(L, index);
            if (lua_istable(L, -1)) {
                lua_pushnil(L);
                for (;;) {
                    if (lua_next(L, -2) == 0)
                        break;
                    lua_pushvalue(L, -2);
                    lua_pushvalue(L, -2);
                    CLuaVariant key = CLuaValue<CLuaVariant,0>()(L,-2);
                    CLuaVariant value = CLuaValue<CLuaVariant,0>()(L,-1);
                    table[key] = value;
                    lua_pop(L, 3);
                }
            }
            lua_pop(L, 1);
            return table;
        }

        CLuaVariant operator()(CLuaStack& L, int index)
        {
            CLuaVariant value;
            if (lua_istable(L, index))
                value.Set(GetTable(L,index));
            else {
                if (lua_isboolean(L, index))
                    value.Set(CLuaValue<bool,0>()(L,index));
                else {
                    if (lua_isnumber(L, index))
                        value.Set(CLuaValue<double,0>()(L,index));
                    else if (lua_isstring(L, index))
                        value.Set(CLuaValue<std::string,0>()(L,index));
                }
            }
            return value;
        }
    };


    template<int __>
    struct CLuaValue<LuaTable,__>
    {
        LuaTable operator()(CLuaStack& L, int index)
        {
            LuaTable table;
            lua_pushvalue(L, index);
            if (lua_istable(L, -1)) {
                lua_pushnil(L);
                for (;;) {
                    if (lua_next(L, -2) == 0)
                        break;
                    lua_pushvalue(L, -2);
                    lua_pushvalue(L, -2);
                    CLuaVariant key = CLuaValue<CLuaVariant,0>()(L,-2);
                    CLuaVariant value = CLuaValue<CLuaVariant,0>()(L,-1);

                    table[key] = value;
                    lua_pop(L, 3);
                }
            }
            lua_pop(L, 1);
            return table;
        }
    };
protected:
    lua_State *m_pluaVM;
    int m_iTopIndex;
};


void CLuaStack::LuaStackInfo()
{
    printf("==========================Lua stack info start=====================================\n" );
    char szMsg[1024];
    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);

    for (int i = 0;; i++) {
        if (lua_getstack(m_pluaVM, i, &trouble_info) == 0)
            break;
        lua_getinfo(m_pluaVM, "Snl", &trouble_info);

        sprintf( szMsg, "name:(%s) what:(%s) short:(%s) linedefined:(%d) currentline:(%d)",
                 trouble_info.name,
                 trouble_info.what,
                 trouble_info.short_src,
                 trouble_info.linedefined,
                 trouble_info.currentline);
        printf( "%s\n",szMsg );
        sprintf(szMsg, "%s(%d): /t%s",
                trouble_info.short_src,
                trouble_info.currentline,
                trouble_info.name);

        printf( "%s\n",szMsg );
    }
    printf("==========================Lua stack info start=====================================\n" );

}

bool CLuaStack::CheckLuaArg_Num(int Index, bool isLuaError)
{
    if (lua_isnumber(m_pluaVM, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(m_pluaVM, 0, &trouble_info) || lua_getstack(m_pluaVM, 1, &trouble_info))
        lua_getinfo(m_pluaVM, "Snl", &trouble_info);

    char szMsg[1024];
    if (lua_isnil(m_pluaVM, Index)) {
        sprintf(szMsg, "Lua function(%s), %d arg is null", trouble_info.name, Index);
    }
    else if (lua_isstring(m_pluaVM, Index)) {
        sprintf(szMsg,
                "Lua function(%s) %d arg type error('%s') it's not number",
                trouble_info.name,
                Index,
                lua_tostring(m_pluaVM, Index));
    }
    else {
        sprintf(szMsg, "Lua function(%s), %d arg type error", trouble_info.name, Index);
    }

    LuaStackInfo();
    if (isLuaError)
    {
        LuaAssert(false,szMsg);
    }
    return false;
}

bool CLuaStack::CheckLuaArg_Str(int Index, bool isLuaError)
{
    if (lua_isstring(m_pluaVM, Index))
        return true;

    char szMsg[1024];

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(m_pluaVM, 1, &trouble_info) || lua_getstack(m_pluaVM, 0, &trouble_info))
        lua_getinfo(m_pluaVM, "Snl", &trouble_info);

    if (lua_isnil(m_pluaVM, Index)) {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg is null", trouble_info.name, Index);
    }
    else {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg type error", trouble_info.name, Index);
    }

    sprintf( szMsg, "name:(%s) namewhat:(%s) what:(%s) source:(%s) short:(%s) linedefined:(%d) currentline:(%d)\n",
             trouble_info.name,
             trouble_info.namewhat,
             trouble_info.what,
             trouble_info.source,
             trouble_info.short_src,
             trouble_info.linedefined,
             trouble_info.currentline
    );
    LuaStackInfo();
    if (isLuaError)
    {
        LuaAssert(false,szMsg);
    }
    return false;
}

void CLuaStack::LuaAssert(bool condition, int argindex, const char *err_msg)
{
    luaL_argcheck(m_pluaVM, condition, argindex, err_msg);
}

void CLuaStack::LuaAssert(bool condition, const char *err_msg)
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

void CLuaStack::DefaultDebugLuaErrorInfo(const char *FunName, const char *Msg)
{
    printf("Call fun:[%s] failed,msg:[%s]\n",FunName,Msg);
}

template<class T>
int CLuaStack::LuaNewObject(lua_State *L)
{
    T *obj = new T;
    T **pobj = static_cast<T **>( lua_newuserdata((L), sizeof(T *)));
    *pobj = obj;

    luaL_getmetatable(L, T::GetLuaTypeName());
    lua_setmetatable(L, -2);

    return 1;  /* new userdatum is already on the stack */
}

template<class T>
int CLuaStack::LuaDelObject(lua_State *L)
{
    T **pobj = static_cast<T **>( luaL_checkudata(L, -1, T::GetLuaTypeName()));
    luaL_argcheck(L, pobj != NULL, -1, "Ojbect type missing");

    delete *pobj;
    *pobj = NULL;

    return 0;
}

template<const char *GetLuaTypeName()>
int CLuaStack::LuaImplementIndex(lua_State *L)
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
        lua_gettop(L);
        return lua_gettop(L) - narg;
    }
    return 1;
}
#endif  //__LUA_STACK_H__