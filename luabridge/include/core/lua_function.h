/******************************************************************************
* https://github.com/DGuco/luabridge
*
* Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
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

#ifndef __LUA_STD_FUNCTION_H_
#define __LUA_STD_FUNCTION_H_

#include "lua_stack.h"
#include <functional>
#include <type_traits>

namespace luabridge
{
template<typename T>
struct function_traits;

//普通函数
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
    enum
    {
        arity = sizeof...(Args)
    };
    typedef Ret function_type(Args...);
    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;
    typedef Ret(*pointer)(Args...);

    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
    };
};

//函数指针
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)>: function_traits<Ret(Args...)>
{
};

//std::function
template<typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>>: function_traits<Ret(Args...)>
{
};

//member function
#define FUNCTION_TRAITS(...) \
    template <typename ReturnType, typename ClassType, typename... Args>\
    struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{}; \

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)

//函数对象
template<typename Callable>
struct function_traits: function_traits<decltype(&Callable::operator())>
{
};

// ------------------------
// Functin Marshal
// ------------------------

#define  LUA_PARAM_TYPE(n) typename function_traits<FT>::template args<0>::type

template<int N, typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,false, "Parameters number too many.");
        return 0;
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<0, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        return f();
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<1, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 1, "Request 1 param");
        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,0));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<2, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {

        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 2, "Request 2 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<3, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 3, "Request 3 param");
        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<4, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 4, "Request 4 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<5, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 5, "Request 5 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<6, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 6, "Request 6 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5),
                 Stack<LUA_PARAM_TYPE(5)>::get(l,6));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<7, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 7, "Request 7 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5),
                 Stack<LUA_PARAM_TYPE(5)>::get(l,6),
                 Stack<LUA_PARAM_TYPE(6)>::get(l,7));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<8, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 8, "Request 8 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5),
                 Stack<LUA_PARAM_TYPE(5)>::get(l,6),
                 Stack<LUA_PARAM_TYPE(6)>::get(l,7),
                 Stack<LUA_PARAM_TYPE(7)>::get(l,8));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<9, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 9, "Request 9 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5),
                 Stack<LUA_PARAM_TYPE(5)>::get(l,6),
                 Stack<LUA_PARAM_TYPE(6)>::get(l,7),
                 Stack<LUA_PARAM_TYPE(7)>::get(l,8),
                 Stack<LUA_PARAM_TYPE(8)>::get(l,9));
    }
};

template<typename FT, typename STD_FUNCTION>
struct CLuaCFunctionWrapN<10, FT, STD_FUNCTION>
{
    inline typename STD_FUNCTION::result_type operator()(lua_State *l, STD_FUNCTION f)
    {
        LuaStack::LuaAssert(l,LuaStack::GetParamCount(l) == 10, "Request 10 param");

        return f(Stack<LUA_PARAM_TYPE(0)>::get(l,1),
                 Stack<LUA_PARAM_TYPE(1)>::get(l,2),
                 Stack<LUA_PARAM_TYPE(2)>::get(l,3),
                 Stack<LUA_PARAM_TYPE(3)>::get(l,4),
                 Stack<LUA_PARAM_TYPE(4)>::get(l,5),
                 Stack<LUA_PARAM_TYPE(5)>::get(l,6),
                 Stack<LUA_PARAM_TYPE(6)>::get(l,7),
                 Stack<LUA_PARAM_TYPE(7)>::get(l,8),
                 Stack<LUA_PARAM_TYPE(8)>::get(l,9),
                 Stack<LUA_PARAM_TYPE(9)>::get(l,10));
    }
};

template<typename FT, typename STD_FUNCTION>
struct lua_function
{
    static STD_FUNCTION fn;

    static int Call(lua_State *L)
    {
        Stack<typename std::function<FT>::result_type>::push(L,CLuaCFunctionWrapN<function_traits<FT>::arity, FT, STD_FUNCTION>()(L, fn));
        return 1;
    }

    static int Call0(lua_State *L)
    {
        CLuaCFunctionWrapN<function_traits<FT>::arity, FT, STD_FUNCTION>()(L, fn);
        return 0;
    }
};

template<typename FT, typename STD_FUNCTION>
STD_FUNCTION lua_function<FT, STD_FUNCTION>::fn;

template<typename FT, typename STD_FUNCTION, typename R>
struct LuaCFunctionWrapI
{
    inline lua_CFunction operator()(STD_FUNCTION f)
    {
        lua_function<FT, STD_FUNCTION>::fn = f;
        return &lua_function<FT, STD_FUNCTION>::Call;
        //	return 0;
    }
};

template<typename FT, typename STD_FUNCTION>
struct LuaCFunctionWrapI<FT, STD_FUNCTION, void>
{
    inline lua_CFunction operator()(STD_FUNCTION f)
    {
        lua_function<FT, STD_FUNCTION>::fn = f;
        return &lua_function<FT, STD_FUNCTION>::Call0;
    }
};

template<typename FT, typename F>
inline lua_CFunction LuaCFunctionWrap(F f)
{
    return LuaCFunctionWrapI<FT, std::function<FT>, typename std::function<FT>::result_type>()(std::function<FT>(f));
}

}

#endif //__LUA_STD_FUNCTION_H_