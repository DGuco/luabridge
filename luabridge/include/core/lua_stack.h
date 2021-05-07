/******************************************************************************
* https://github.com/DGuco/luabridge
*
* Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.

* Copyright 2019, Dmitry Tarakanov
* Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
* Copyright 2007, Nathan Reed
* Copyright (C) 2004 Yong Lin.  All rights reserved.
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

#ifndef  __LUA_STACK_H__
#define  __LUA_STACK_H__

#include <cstring>
#include "lua_file.h"
#include "lua_variant.h"
#include "lua_library.h"
#include "lua_library.h"
#include "lua_helpers.h"
#include "user_data.h"
#include <string>
namespace luabridge
{

class LuaStack
{
public:
    /**
     * 检测lua 堆栈中的参数类型
     * @param Index
     * @param isLuaError 如果类型错误是否抛出lua异常
     * @return
     */
    static bool CheckLuaArg_Num(lua_State *L, int Index, bool isLuaError = true);
    static bool CheckLuaArg_Integer(lua_State *L, int Index, bool isLuaError = true);
    static bool CheckLuaArg_Str(lua_State *L, int Index, bool isLuaError = true);
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
    static void LuaAssert(lua_State *L, bool condition, int argindex, const char *err_msg);
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

void LuaStack::LuaStackInfo(lua_State *L)
{
    printf("==========================Lua stack info start=====================================\n");
    char szMsg[1024];
    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);

    for (int i = 0;; i++) {
        if (lua_getstack(L, i, &trouble_info) == 0)
            break;
        lua_getinfo(L, "Snl", &trouble_info);

        sprintf(szMsg, "name:(%s) what:(%s) short:(%s) linedefined:(%d) currentline:(%d)",
                trouble_info.name,
                trouble_info.what,
                trouble_info.short_src,
                trouble_info.linedefined,
                trouble_info.currentline);
        printf("%s\n", szMsg);
        sprintf(szMsg, "%s(%d): /t%s",
                trouble_info.short_src,
                trouble_info.currentline,
                trouble_info.name);

        printf("%s\n", szMsg);
    }
    printf("==========================Lua stack info start=====================================\n");

}

bool LuaStack::CheckLuaArg_Num(lua_State *L, int Index, bool isLuaError)
{
    if (lua_isnumber(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    char szMsg[1024];
    if (lua_isnil(L, Index)) {
        sprintf(szMsg, "Lua function(%s), %d arg is null", trouble_info.name, Index);
    }
    else if (lua_isstring(L, Index)) {
        sprintf(szMsg,
                "Lua function(%s) %d arg type error('%s') it's not number",
                trouble_info.name,
                Index,
                lua_tostring(L, Index));
    }
    else {
        sprintf(szMsg, "Lua function(%s), %d arg type error", trouble_info.name, Index);
    }

    LuaStackInfo(L);
    if (isLuaError) {
        LuaAssert(L, false, szMsg);
    }
    return false;
}

bool LuaStack::CheckLuaArg_Integer(lua_State *L, int Index, bool isLuaError)
{
    if (lua_isinteger(L, Index))
        return true;

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    char szMsg[1024];
    if (lua_isnil(L, Index)) {
        sprintf(szMsg, "Lua function(%s), %d arg is null", trouble_info.name, Index);
    }
    else if (lua_isstring(L, Index)) {
        sprintf(szMsg,
                "Lua function(%s) %d arg type error('%s') it's not number",
                trouble_info.name,
                Index,
                lua_tostring(L, Index));
    }
    else {
        sprintf(szMsg, "Lua function(%s), %d arg type error", trouble_info.name, Index);
    }

    LuaStackInfo(L);
    if (isLuaError) {
        LuaAssert(L, false, szMsg);
    }
    return false;
}

bool LuaStack::CheckLuaArg_Str(lua_State *L, int Index, bool isLuaError)
{
    if (lua_isstring(L, Index))
        return true;

    char szMsg[1024];

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    //int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 1, &trouble_info) || lua_getstack(L, 0, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (lua_isnil(L, Index)) {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg is null", trouble_info.name, Index);
    }
    else {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg type error", trouble_info.name, Index);
    }

    sprintf(szMsg, "name:(%s) namewhat:(%s) what:(%s) source:(%s) short:(%s) linedefined:(%d) currentline:(%d)\n",
            trouble_info.name,
            trouble_info.namewhat,
            trouble_info.what,
            trouble_info.source,
            trouble_info.short_src,
            trouble_info.linedefined,
            trouble_info.currentline
    );
    LuaStackInfo(L);
    if (isLuaError) {
        LuaAssert(L, false, szMsg);
    }
    return false;
}

void LuaStack::LuaAssert(lua_State *L, bool condition, int argindex, const char *err_msg)
{
    luaL_argcheck(L, condition, argindex, err_msg);
}

void LuaStack::LuaAssert(lua_State *L, bool condition, const char *err_msg)
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
        luaL_error(L, "assert fail: %s `%s' (%s)", ar.namewhat, ar.name, err_msg);
    }
}

int LuaStack::GetParamCount(lua_State *L)
{
    return lua_gettop(L);
}

void LuaStack::DefaultDebugLuaErrorInfo(const char *FunName, const char *Msg)
{
    printf("Call fun:[%s] failed,msg:[%s]\n", FunName, Msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct Stack;

template<>
struct Stack<void>
{
    static void push(lua_State *L)
    {
    }
};

//------------------------------------------------------------------------------
/**
    Receive the lua_State* as an argument.
*/
template<>
struct Stack<lua_State *>
{
    static lua_State *get(lua_State *L, int)
    {
        return L;
    }
};

//------------------------------------------------------------------------------
/**
    Push a lua_CFunction.
*/
template<>
struct Stack<lua_CFunction>
{
    static void push(lua_State *L, lua_CFunction f)
    {
        lua_pushcfunction(L, f);
    }

    static lua_CFunction get(lua_State *L, int index)
    {
        return lua_tocfunction(L, index);
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `int`.
*/
template<>
struct Stack<int>
{
    static void push(lua_State *L, int value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
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
     **/
    static int get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <int> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<int> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<int>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned int`.
*/
template<>
struct Stack<unsigned int>
{
    static void push(lua_State *L, unsigned int value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static unsigned int get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <unsigned int> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<unsigned int> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<unsigned int>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned char`.
*/
template<>
struct Stack<unsigned char>
{
    static void push(lua_State *L, unsigned char value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static unsigned char get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <unsigned char> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<unsigned char> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<unsigned char>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `short`.
*/
template<>
struct Stack<short>
{
    static void push(lua_State *L, short value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
     * @param check
     */
    static short get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <short> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<short> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<short>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned short`.
*/
template<>
struct Stack<unsigned short>
{
    static void push(lua_State *L, unsigned short value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static unsigned short get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <unsigned short> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<unsigned short> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<unsigned short>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `long`.
*/
template<>
struct Stack<long>
{
    static void push(lua_State *L, long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static long get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <long> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<long> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<long>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `unsigned long`.
*/
template<>
struct Stack<unsigned long>
{
    static void push(lua_State *L, unsigned long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static long get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <unsigned long> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<unsigned long> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<unsigned long>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `long long`.
 */
template<>
struct Stack<long long>
{
    static void push(lua_State *L, long long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }

    /*
    * @param check
    */
    static long long get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <long long> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<long long> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<long long>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
 * Stack specialization for `unsigned long long`.
 */
template<>
struct Stack<unsigned long long>
{
    static void push(lua_State *L, unsigned long long value)
    {
        lua_pushinteger(L, static_cast <lua_Integer> (value));
    }
    static unsigned long long get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <unsigned long long> (luaL_checkinteger(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Integer(L, index, false)) {
                throw std::runtime_error("Stack<unsigned long long> CheckLuaArg_Integer failed");
            }
            else {
                return static_cast<unsigned long long>(lua_tointeger(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `float`.
*/
template<>
struct Stack<float>
{
    static void push(lua_State *L, float value)
    {
        lua_pushnumber(L, static_cast <lua_Number> (value));
    }

    static float get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <float> (luaL_checknumber(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Num(L, index, false)) {
                throw std::runtime_error("Stack<float> CheckLuaArg_Num failed");
            }
            else {
                return static_cast<float>(lua_tonumber(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `double`.
*/
template<>
struct Stack<double>
{
    static void push(lua_State *L, double value)
    {
        lua_pushnumber(L, static_cast <lua_Number> (value));
    }

    static double get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return static_cast <double> (luaL_checknumber(L, index));
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Num(L, index, false)) {
                throw std::runtime_error("Stack<double> CheckLuaArg_Num failed");
            }
            else {
                return static_cast<double>(lua_tonumber(L, index));
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `bool`.
*/
template<>
struct Stack<bool>
{
    static void push(lua_State *L, bool value)
    {
        lua_pushboolean(L, value ? 1 : 0);
    }

    static bool get(lua_State *L, int index, bool luaerror = false)
    {
        return lua_toboolean(L, index) ? true : false;
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `char`.
*/
template<>
struct Stack<char>
{
    static void push(lua_State *L, char value)
    {
        lua_pushlstring(L, &value, 1);
    }

    static char get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return luaL_checkstring(L, index)[0];
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Str(L, index, false)) {
                throw std::runtime_error("Stack<char> CheckLuaArg_Str failed");
            }
            else {
                if (lua_isnil(L,index))
                {
                    return ' ';
                }
                return lua_tostring(L, index)[0];
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `const char*`.
*/
template<>
struct Stack<char const *>
{
    static void push(lua_State *L, char const *str)
    {
        if (str != 0)
            lua_pushstring(L, str);
        else
            lua_pushnil(L);
    }

    static char const *get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror) {
            return luaL_checkstring(L, index);
        }
        else {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Str(L, index, false)) {
                throw std::runtime_error("Stack<char const *> CheckLuaArg_Str failed");
            }
            else {
                if (lua_isnil(L,index))
                {
                    return "";
                }
                return lua_tostring(L, index);
            }
        }
    }
};

//------------------------------------------------------------------------------
/**
    Stack specialization for `std::string`.
*/
template<>
struct Stack<std::string>
{
    static void push(lua_State *L, std::string const &str)
    {
        lua_pushlstring(L, str.data(), str.size());
    }

    static std::string get(lua_State *L, int index, bool luaerror = false)
    {
        if (luaerror)
        {
            LuaStack::CheckLuaArg_Str(L, index, true);
            if (lua_isnil(L,index))
            {
                return "";
            }
            size_t len;
            const char *str = lua_tolstring(L, index, &len);
            return std::string(str, len);
        }else
        {
            //抛出c++异常
            if (!LuaStack::CheckLuaArg_Str(L, index, false)) {
                throw std::runtime_error("Stack<std::string> CheckLuaArg_Str failed");
            }
            else {
                if (lua_isnil(L,index))
                {
                    return "";
                }
                size_t len;
                const char *str = lua_tolstring(L, index, &len);
                return std::string(str, len);
            }
        }
//        size_t len;
//        if (lua_type(L, index) == LUA_TSTRING) {
//            const char *str = lua_tolstring(L, index, &len);
//            return std::string(str, len);
//        }
//
//        // Lua reference manual:
//        // If the value is a number, then lua_tolstring also changes the actual value in the stack to a string.
//        // (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
//        lua_pushvalue(L, index);
//        const char *str = lua_tolstring(L, -1, &len);
//        std::string string(str, len);
//        lua_pop(L, 1); // Pop the temporary string
//        return string;
    }
};

template<class T>
struct StackOpSelector<T &, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, T &value)
    {
        Stack<T>::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<const T &, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, const T &value)
    {
        Stack<T>::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<T *, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, T *value)
    {
        Stack<T>::push(L, *value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct StackOpSelector<const T *, false>
{
    typedef T ReturnType;

    static void push(lua_State *L, const T *value)
    {
        Stack<T>::push(L, *value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Stack<T>::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<T &>
{
    typedef StackOpSelector<T &, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, T &value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<const T &>
{
    typedef StackOpSelector<const T &, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, const T &value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<T *>
{
    typedef StackOpSelector<T *, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, T *value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Helper::get(L, index, luaerror);
    }
};

template<class T>
struct Stack<const T *>
{
    typedef StackOpSelector<const T *, IsUserdata<T>::value> Helper;
    typedef typename Helper::ReturnType ReturnType;

    static void push(lua_State *L, const T *value)
    {
        Helper::push(L, value);
    }

    static ReturnType get(lua_State *L, int index, bool luaerror = false)
    {
        return Helper::get(L, index, luaerror);
    }
};

} // namespace luabridge

#endif  //__LUA_STACK_H__