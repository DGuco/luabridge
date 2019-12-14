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
#ifndef  __LUA_BRIDGE_H__
#define  __LUA_BRIDGE_H__

#include <string>
#include "lua_file.h"
#include "lua_stack.h"

#define SCRIPI_NAME_MAX_LEN 128

struct ScriptRecord
{
    char sriptName[SCRIPI_NAME_MAX_LEN];

};

class CLuaBridge: public CLuaStack
{
public:
    CLuaBridge(lua_State *VM)
        : CLuaStack(VM)
    {
        // initialize lua standard library functions
        luaopen_base(m_pluaVM);
        luaopen_table(m_pluaVM);
        luaopen_string(m_pluaVM);
        luaopen_math(m_pluaVM);
    }

    ~CLuaBridge()
    {
        if(NULL != m_pluaVM)
        {
            lua_close(m_pluaVM);
        }
    }

public:
    bool LoadFile(const char *filename)
    {
        return luaL_dofile(m_pluaVM, filename) == 0;
    }

    bool LoadString(const char *buffer)
    {
        return LoadBuffer(buffer, strlen(buffer));
    }

    bool LoadBuffer(const char *buffer, size_t size)
    {
        return (luaL_loadbuffer(m_pluaVM, buffer, size, "LuaWrap") || lua_pcall(m_pluaVM, 0, LUA_MULTRET, 0));
    }

    void Register(const char *func, lua_CFunction f)
    {
        char Buf[256];
        strcpy(Buf, func);
        lua_register(m_pluaVM, Buf, f);
    }

    template<class LUATYPE>
    void Register()
    {
        LUATYPE::LuaOpenLibMember(m_pluaVM);
        LUATYPE::LuaOpenLib(m_pluaVM);
    }

    operator lua_State *()
    {
        return m_pluaVM;
    }

    // Call Lua function
    //   func:	Lua function name
    //   R:		Return type. (void, float, double, int, long, bool, const char*, std::string)
    // Sample:	double f = lua.Call<double>("test0", 1.0, 3, "param");
    template<typename R>
    R Call(const char *func);

    template<typename R, typename P1>
    R Call(const char *func, P1 p1);

    template<typename R, typename P1, typename P2>
    R Call(const char *func, P1 p1, P2 p2);

    template<typename R, typename P1, typename P2, typename P3>
    R Call(const char *func, P1 p1, P2 p2, P3 p3);

    template<typename R, typename P1, typename P2, typename P3, typename P4>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
    R Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10);
    /************************************************************************
    * Here:
    * This function "const char* Call(const char*, const char*, ...)" is come from "Programming in Lua"
    *
    * BOOK:	Programming in Lua
    *		by Roberto Ierusalimschy
    *		Lua.org, December 2003
    *		ISBN 85-903798-1-7
    ************************************************************************/
    /**
     *
     * @param func 函数名
     * @param sig  函数签名
     * 格式如p*[:r*]或p*[>r*], 冒号或者大于号前面为参数，后面为返回值，每个字母为每一个参数的类型
     * 类型表示为 f 或 e 表示float； i 或 n 或 d 表示整数； b 表示bool； s 表示字元串S 表示char* 数组，前面是长度，后面是char*指针
     * @param ...
     * @return  返回 返回值如果为 NULL， 表示调用成功。否则返回错误信息
     */
    // 例1︰ double f; const char* error_msg = lua.Call("test01", "nnnn:f", 1,2,3,4,&f);
    // 例2︰ const char* s; int len; const char* error_msg = lua.Call("test01", "S:S", 11, "Hello\0World", &len, &s);
    const char *Call(const char *func, const char *sig, ...);
};

template<typename R>
R CLuaBridge::Call(const char *func)
{
    SafeBeginCall(func);
    return SafeEndCall<R, 0>(func, 0);
}

template<typename R, typename P1>
R CLuaBridge::Call(const char *func, P1 p1)
{
    SafeBeginCall(func);
    Push(p1);
    return SafeEndCall<R, 0>(func, 1);
}

template<typename R, typename P1, typename P2>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    return SafeEndCall<R, 0>(func, 2);
}

template<typename R, typename P1, typename P2, typename P3>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    return SafeEndCall<R, 0>(func, 3);
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    return SafeEndCall<R>(func, 4);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    return SafeEndCall<R, 0>(func, 5);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    Push(p6);
    return SafeEndCall<R>(func, 6);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    Push(p6);
    Push(p7);
    return SafeEndCall<R, 0>(func, 7);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    Push(p6);
    Push(p7);
    Push(p8);
    return SafeEndCall<R, 0>(func, 8);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    Push(p6);
    Push(p7);
    Push(p8);
    Push(p9);
    return SafeEndCall<R, 0>(func, 9);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
R CLuaBridge::Call(const char *func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
{
    SafeBeginCall(func);
    Push(p1);
    Push(p2);
    Push(p3);
    Push(p4);
    Push(p5);
    Push(p6);
    Push(p7);
    Push(p8);
    Push(p9);
    Push(p10);
    return SafeEndCall<R, 0>(func, 10);
}

const char *CLuaBridge::Call(const char *func, const char *sig, ...)
{
    va_list vl;
    va_start(vl, sig);

    lua_getglobal(m_pluaVM, func);

    /* 壓入調用參數 */
    int narg = 0;
    while (*sig) {  /* push arguments */
        switch (*sig++) {
        case 'f':    /* 浮點數 */
        case 'e':    /* 浮點數 */
            lua_pushnumber(m_pluaVM, va_arg(vl, double));
            break;

        case 'i':    /* 整數 */
        case 'n':    /* 整數 */
        case 'd':    /* 整數 */
            lua_pushnumber(m_pluaVM, va_arg(vl, int));
            break;

        case 'b':    /* 布爾值 */
            lua_pushboolean(m_pluaVM, va_arg(vl, int));
            break;

        case 's':    /* 字元串 */
            lua_pushstring(m_pluaVM, va_arg(vl, char *));
            break;

        case 'S':    /* 字元串 */
        {
            int len = va_arg(vl, int);
            lua_pushlstring(m_pluaVM, va_arg(vl, char *), len);
        }
            break;

        case '>':
        case ':':
            goto L_LuaCall;

        default:
            //assert(("Lua call option is invalid!", false));
            //error(m_pluaVM, "invalid option (%c)", *(sig - 1));
            lua_pushnumber(m_pluaVM, 0);
        }
        narg++;
        luaL_checkstack(m_pluaVM, 1, "too many arguments");
    }

    L_LuaCall:
    int nres = static_cast<int>(strlen(sig));
    const char *sresult = NULL;
    if (lua_pcall(m_pluaVM, narg, nres, 0) != 0) {
        sresult = lua_tostring(m_pluaVM, -1);
        nres = 1;
    }
    else {
        // 取得返回值
        int index = -nres;
        while (*sig) {
            switch (*sig++) {
            case 'f':    /* 浮点数 float*/
            case 'e':    /* 浮点数 float*/
                *va_arg(vl, double *) = lua_tonumber(m_pluaVM, index);
                break;

            case 'i':    /* 整数 */
            case 'n':    /* 整数 */
            case 'd':    /* 整数 */
                *va_arg(vl, int *) = static_cast<int>(lua_tonumber(m_pluaVM, index));
                break;

            case 'b':    /* bool */
                *va_arg(vl, int *) = static_cast<int>(lua_toboolean(m_pluaVM, index));
                break;

            case 's':    /* string */
                *va_arg(vl, const char **) = lua_tostring(m_pluaVM, index);
                break;

            case 'S':    /* string */
                *va_arg(vl, int *) = static_cast<int>(lua_strlen(m_pluaVM, index));
                *va_arg(vl, const char **) = lua_tostring(m_pluaVM, index);
                break;

            default:
                //assert(("Lua call invalid option!", false));
                //error(m_pluaVM, "invalid option (%c)", *(sig - 1));
                ;
            }
            index++;
        }
    }
    va_end(vl);

    lua_pop(m_pluaVM, nres);
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

#if __cplusplus >= 201103L
#include "lua_function.h"
#define LuaRegisterCFunc(luaBridge, funcname, type, func)                           \
    luaBridge.Register(funcname, ( LuaCFunctionWrap<type>(func) ) )
#endif // __cplusplus >= 201103L

#define LuaRegisterLuaFunc(luaBridge, funcname, func)                           \
    luaBridge.Register(funcname, func)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif __LUA_BRIDGE_H__