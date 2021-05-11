/******************************************************************************
* Name: LuaBridge for C++
*
* Author: DGuco(杜国超)
* Date: 2019-12-07 17:15
* E-Mail: 1139140929@qq.com
*
* Copyright (C) 2019 DGuco(杜国超).  All rights reserved.
*
* License: The MIT License (http://www.opensource.org/licenses/mit-license.php)
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
#ifndef  __LUA_BRIDGE_H__
#define  __LUA_BRIDGE_H__

#include <map>
#include <string>
#include <utility>
#include "lua_file.h"

namespace luabridge
{
class LuaBridge
{
public:
    /**
     * Construce
     * @param VM
     */
    LuaBridge(lua_State *VM);
    /**
     * destruct
     */
    ~LuaBridge();
    /**
     * load lua file
     * @param filePath
     * @return
     */
    bool LoadFile(const std::string &filePath);
    bool LoadFile(const char *filePath);

    /**
     * register cfunction
     * @param func func name 函数名
     * @param f
     */
    void Register(const char *func, lua_CFunction f);
    /**
     * Call Lua function
     * @param func 函数名
     * func:	Lua function name
     * R:		Return type. (void, float, double, int, long, bool, const char*, std::string)
     * Sample:	double f = lua.Call<double>("test0", 1.0, 3, "param");
     */
    /**
     * Call Lua function
     * @tparam R    返回类型
     * @tparam Args 函数参数列表
     * @param func  函数名
     * @param args  函数参数列表
     * @return R
     */
    template<typename R, typename ...Args>
    R Call(const char *func, const Args... args);
    /**
     *
     * @param func 函数名
     * @param sig  函数签名
     * 格式如p*[:r*]或p*[>r*], 冒号或者大于号前面为参数，后面为返回值，每个字母为每一个参数的类型
     * 类型表示为 f 或 e 表示float； i 或 n 或 d 表示整数； b 表示bool； s 表示字元串S 表示char* 数组，前面是长度，后面是char*指针
     * @param ...
     * @return  返回 返回值如果为 NULL， 表示调用成功。否则返回错误信息
     */
    // 例1︰ double f; const char* error_msg = lua.Call(const char* scriptName, "test01", "nnnn:f", 1,2,3,4,&f);
    // 例2︰ const char* s; int len; const char* error_msg = lua.Call(const char* scriptName, "test01", "S:S", 11, "Hello\0World", &len, &s);
    const char *Call(const char *func, const char *sig, ...);
private:
    //把参数压栈
    int PushToLua();
    template<typename T>
    int PushToLua(const T &t);
    template<typename First, typename... Rest>
    int PushToLua(const First &first, const Rest &...rest);

    inline void SafeBeginCall(const char *func);
    template<typename R, int __>
    inline R SafeEndCall(const char *func, int nArg);

    template<int __>
    inline void SafeEndCall(const char *func, int nArg);
private:
    lua_State *m_pLuaVM;
    int m_iTopIndex;
};

LuaBridge::LuaBridge(lua_State *VM)
    : m_pLuaVM(VM), m_iTopIndex(0)
{
    // initialize lua standard library functions
    luaopen_base(m_pLuaVM);
    luaopen_table(m_pLuaVM);
    luaopen_string(m_pLuaVM);
    luaopen_math(m_pLuaVM);
    luaopen_debug(m_pLuaVM);
    luaopen_utf8(m_pLuaVM);
    LuaException::EnableExceptions(m_pLuaVM);
}

LuaBridge::~LuaBridge()
{
    if (NULL != m_pLuaVM) {
        lua_close(m_pLuaVM);
    }
}

bool LuaBridge::LoadFile(const std::string &filePath)
{
    int ret = luaL_dofile(m_pLuaVM, filePath.c_str());
    if (ret != 0) {
        throw std::runtime_error("Lua loadfile failed,error:" + ret);
    }
    return 0;
}

bool LuaBridge::LoadFile(const char *filePath)
{
    int ret = luaL_dofile(m_pLuaVM, filePath);
    if (ret != 0) {
        throw std::runtime_error("Lua loadfile failed,error:" + ret);
    }
    return 0;
}

void LuaBridge::Register(const char *func, lua_CFunction f)
{
    char Buf[256] = {0};
    strcpy(Buf, func);
    lua_register(m_pLuaVM, Buf, f);
}

int LuaBridge::PushToLua()
{
    return 0;
}

template<typename T>
int LuaBridge::PushToLua(const T &t)
{
    Stack<T>::push(m_pLuaVM, t);
    return 1;
}

template<typename First, typename... Rest>
int LuaBridge::PushToLua(const First &first, const Rest &...rest)
{
    Stack<First>::push(m_pLuaVM, first);
    return PushToLua(rest...);
}

void LuaBridge::SafeBeginCall(const char *func)
{
    //记录调用前的堆栈索引
    m_iTopIndex = lua_gettop(m_pLuaVM);
    lua_getglobal(m_pLuaVM, func);
}

template<typename R, int __>
R LuaBridge::SafeEndCall(const char *func, int nArg)
{
    if (lua_pcall(m_pLuaVM, nArg, 1, 0) != LUA_OK) {
        LuaHelper::DefaultDebugLuaErrorInfo(func, lua_tostring(m_pLuaVM, -1));
        //恢复调用前的堆栈索引
        lua_settop(m_pLuaVM, m_iTopIndex);
        return 0;
    }
    else {
        try
        {
            R r = Stack<R>::get(m_pLuaVM, -1);
            //恢复调用前的堆栈索引
            lua_settop(m_pLuaVM, m_iTopIndex);
            return r;
        }
        catch (std::exception &e) {
            //恢复调用前的堆栈索引
            lua_settop(m_pLuaVM, m_iTopIndex);
            LuaHelper::DefaultDebugLuaErrorInfo(func, e.what());
            return 0;
        }
    }
}

template<int __>
void LuaBridge::SafeEndCall(const char *func, int nArg)
{
    if (lua_pcall(m_pLuaVM, nArg, 0, 0) != 0) {
        LuaHelper::DefaultDebugLuaErrorInfo(func, lua_tostring(m_pLuaVM, -1));
    }
    lua_settop(m_pLuaVM, m_iTopIndex);
}

template<typename R, typename ...Args>
R LuaBridge::Call(const char *func, const Args... args)
{
    SafeBeginCall(func);
    PushToLua(args...);
    return SafeEndCall<R, 0>(func, sizeof...(args));
}

const char *LuaBridge::Call(const char *func, const char *sig, ...)
{
    va_list vl;
    va_start(vl, sig);

    lua_getglobal(m_pLuaVM, func);

    /* 壓入調用參數 */
    int narg = 0;
    while (*sig) {  /* push arguments */
        switch (*sig++) {
        case 'f':    /* 浮點數 */
        case 'e':    /* 浮點數 */
            lua_pushnumber(m_pLuaVM, va_arg(vl, double));
            break;

        case 'i':    /* 整數 */
        case 'n':    /* 整數 */
        case 'd':    /* 整數 */
            lua_pushnumber(m_pLuaVM, va_arg(vl, int));
            break;

        case 'b':    /* 布爾值 */
            lua_pushboolean(m_pLuaVM, va_arg(vl, int));
            break;

        case 's':    /* 字元串 */
            lua_pushstring(m_pLuaVM, va_arg(vl, char *));
            break;

        case 'S':    /* 字元串 */
        {
            int len = va_arg(vl, int);
            lua_pushlstring(m_pLuaVM, va_arg(vl, char *), len);
        }
            break;

        case '>':
        case ':':goto L_LuaCall;

        default:
            //assert(("Lua call option is invalid!", false));
            //error(m_pLuaVM, "invalid option (%c)", *(sig - 1));
            lua_pushnumber(m_pLuaVM, 0);
        }
        narg++;
        luaL_checkstack(m_pLuaVM, 1, "too many arguments");
    }

    L_LuaCall:
    int nres = static_cast<int>(strlen(sig));
    const char *sresult = NULL;
    if (lua_pcall(m_pLuaVM, narg, nres, 0) != 0) {
        sresult = lua_tostring(m_pLuaVM, -1);
        nres = 1;
    }
    else {
        // 取得返回值
        int index = -nres;
        while (*sig) {
            switch (*sig++) {
            case 'f':    /* 浮点数 float*/
            case 'e':    /* 浮点数 float*/
                *va_arg(vl, double *) = lua_tonumber(m_pLuaVM, index);
                break;

            case 'i':    /* 整数 */
            case 'n':    /* 整数 */
            case 'd':    /* 整数 */
                *va_arg(vl, int *) = static_cast<int>(lua_tonumber(m_pLuaVM, index));
                break;

            case 'b':    /* bool */
                *va_arg(vl, int *) = static_cast<int>(lua_toboolean(m_pLuaVM, index));
                break;

            case 's':    /* string */
                *va_arg(vl, const char **) = lua_tostring(m_pLuaVM, index);
                break;

            case 'S':    /* string */
            {
                size_t len;
                const char *str = lua_tolstring(m_pLuaVM, index, &len);
                *va_arg(vl, int *) = static_cast<int>(len);
                *va_arg(vl, const char **) = str;
            }
                break;

            default:
                break;
            }
            index++;
        }
    }
    va_end(vl);

    lua_pop(m_pLuaVM, nres);
    return sresult;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * LuaRegisterCFunc:
 * int func(int a,int b,int c)
 * {
 *  ......
 *  return 0;
 * }
 * void func(int a,int b,int c)
 * {
 *  ......
 *  do someting
 * }
 *
 * LuaRegisterLuaFunc:
 * int func(lua_State *L)
 * {
 *    int para1 = lua_tonumber(L,1);
 *    int para2 = lua_tonumber(L,2);
 *    .....
 *    paran = lua_tonumber(L,n) or lua_tostring(L,n) ....;
 *
 *    .....
 *    lua_pushnumber(L,1);  //1 return
 *    lua_pushnumber(L,2);  //2 return
 *    ....
 *    lua_pushnumber(L,n);  //n return
 *    return n;
 * }
 */

#define LuaRegisterCFunc(luaBridge, funcname, type, func)                       \
    luaBridge.Register(funcname, (LuaCFunctionWrap<__COUNTER__,type>(func)))
#define LuaRegisterLuaFunc(luaBridge, funcname, func)                           \
    luaBridge.Register(funcname, func)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //__LUA_BRIDGE_H__