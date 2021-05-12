//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2019, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>

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

#ifndef __FUNCTION_TRAITS_H__
#define __FUNCTION_TRAITS_H__

#include <functional>
#include "type_list.h"

namespace luabridge
{

//==============================================================================
/**
    Traits for function pointers.

    There are three types of functions: global, non-const member, and const
    member. These templates determine the type of function, which class type it
    belongs to if it is a class member, the const-ness if it is a member
    function, and the type information for the return value and argument list.

    Expansions are provided for functions with up to 8 parameters. This can be
    manually extended, or expanded to an arbitrary amount using C++11 features.
*/
template<class MemFn, class D = MemFn>
struct FuncTraits
{
};

/* Ordinary function pointers. */

template<class R, class... ParamList>
struct FuncTraits<R (*)(ParamList...)>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = false;
    using DeclType = R (*)(ParamList...);
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    //每个参数的类型
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<ParamList...>>::type;
    };
};


template<class T, class R, class... ParamList>
struct FuncTraits<R (T::*)(ParamList...)>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = false;
    using DeclType = R (T::*)(ParamList...);
    using ClassType = T;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    //每个参数的类型
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<ParamList...>>::type;
    };
};

/* Const member function pointers. */

template<class T, class R, class... ParamList>
struct FuncTraits<R (T::*)(ParamList...) const>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = true;
    static bool const isConstMemberFunction = true;
    using DeclType = R (T::*)(ParamList...) const;
    using ClassType = T;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    //每个参数的类型
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<ParamList...>>::type;
    };
};

/* std::function */
template<class R, class... ParamList>
struct FuncTraits<std::function<R(ParamList...)>>
{
    enum
    {
        arity = sizeof...(ParamList)
    };
    static bool const isMemberFunction = false;
    static bool const isConstMemberFunction = false;
    using DeclType = std::function<R(ParamList...)>;
    using ReturnType = R;
    using Params = typename MakeTypeList<ParamList...>::Result;

    //每个参数的类型
    template<size_t I>
    struct args
    {
        static_assert(I < arity, "index is out of range, index must less than sizeof Args");
        using type = typename std::tuple_element<I, std::tuple<ParamList...>>::type;
    };
};

//get参数的类型
#define  LUA_PARAM_TYPE(n) typename FuncTraits<FT>::template args<n>::type
//==============================================================================
/**
 * Traits class for unrolling the type list values into function arguments.
 */
template<class FT, size_t NUM_PARAMS>
struct Caller;

template<class FT>
struct Caller<FT, 0>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn();
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)();
    }
};

template<class FT>
struct Caller<FT, 1>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1));
    }
};

template<class FT>
struct Caller<FT, 2>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2));
    }
};

template<class FT>
struct Caller<FT, 3>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3));
    }
};

template<class FT>
struct Caller<FT, 4>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4));
    }
};

template<class FT>
struct Caller<FT, 5>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5));
    }
};

template<class FT>
struct Caller<FT, 6>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6));
    }
};


template<class FT>
struct Caller<FT, 7>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7));
    }
};

template<class FT>
struct Caller<FT, 8>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8));
    }
};

template<class FT>
struct Caller<FT, 9>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9));
    }
};

template<class FT>
struct Caller<FT, 10>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10));
    }
};

template<class FT>
struct Caller<FT, 11>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,11));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,11));
    }
};

template<class FT>
struct Caller<FT, 12>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,12));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,12));
    }
};

template<class FT>
struct Caller<FT, 13>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,13));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,13));
    }
};

template<class FT>
struct Caller<FT, 14>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,13),
                  Stack<LUA_PARAM_TYPE(13)>::get(L,14));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,13),
                          Stack<LUA_PARAM_TYPE(13)>::get(L,14));
    }
};

template<class FT>
struct Caller<FT, 15>
{
    using FuncType = typename FuncTraits<FT>::DeclType ;
    using ReturnType = typename FuncTraits<FT>::ReturnType ;

    static ReturnType f(lua_State*L,FuncType &fn)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,13),
                  Stack<LUA_PARAM_TYPE(13)>::get(L,14),
                  Stack<LUA_PARAM_TYPE(14)>::get(L,15));
    }

    template<class T>
    static ReturnType f(lua_State*L,T *obj, FuncType &fn)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,1),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,2),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,3),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,4),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,5),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,6),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,7),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,8),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,9),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,10),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,11),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,12),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,13),
                          Stack<LUA_PARAM_TYPE(13)>::get(L,14),
                          Stack<LUA_PARAM_TYPE(14)>::get(L,15));
    }
};

template<class ReturnType, class FT>
static ReturnType doCall(lua_State* L,FT &fnl)
{
    return Caller<FT,FuncTraits<FT>::arity>::f(L, fnl);
}

template<class ReturnType,class T,class FT>
static ReturnType doCall(lua_State* L,T *obj, FT &fn)
{
    return Caller<ReturnType,FuncTraits<FT>::arity>::f(L,obj, fn);
}

//==============================================================================
/**
    Traits for function pointers.

    There are three types of functions: global, non-const member, and const
    member. These templates determine the type of function, which class type it
    belongs to if it is a class member, the const-ness if it is a member
    function, and the type information for the return value and argument list.

    Expansions are provided for functions with up to 8 parameters. This can be
    manually extended, or expanded to an arbitrary amount using C++11 features.
*/
template<class MemFn, class D = MemFn>
struct FuncCall
{
};

/* Ordinary function pointers. */
template<class R, class... ParamList>
struct FuncCall<R (*)(ParamList...)>
{
    using DeclType = typename FuncTraits<R (*)(ParamList...)>::DeclType;
    static R call(lua_State* L,const DeclType &fp)
    {
        return doCall<R>(L,fp);
    }
};

template<class T, class R, class... ParamList>
struct FuncCall<R (T::*)(ParamList...)>
{
    using DeclType = typename FuncTraits< R (T::*)(ParamList...)>::DeclType;;
    using ClassType = T;

    static R call(lua_State* L,ClassType *obj, const DeclType &fp)
    {
        return doCall<R>(L,obj, fp);
    }
};

/* Const member function pointers. */

template<class T, class R, class... ParamList>
struct FuncCall<R (T::*)(ParamList...) const>
{
    using DeclType = typename FuncTraits<R (T::*)(ParamList...) const>::DeclType;
    using ClassType = T;

    static R call(lua_State* L,const ClassType *obj, const DeclType &fp)
    {
        return doCall<R>(L,obj, fp);
    }
};

/* std::function */
template<class R, class... ParamList>
struct FuncCall<std::function<R(ParamList...)>>
{
    using DeclType = typename FuncTraits<std::function<R(ParamList...)>>::DeclType;;

    static R call(lua_State* L,DeclType &fn)
    {
        return doCall<R>(L,fn);
    }
};

template<class ReturnType, class Params, int startParam>
struct Invoke
{
    template<class Fn>
    static int run(lua_State *L, Fn &fn)
    {
        if (!(LuaHelper::GetParamCount(L) == FuncTraits<Fn>::arity)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncCall<Fn>::call(L,fn));
            return 1;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        if (!(LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncCall<MemFn>::call(L,object, fn));
            return 1;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }
};

template<class Params, int startParam>
struct Invoke<void, Params, startParam>
{
    template<class Fn>
    static int run(lua_State *L, Fn &fn)
    {
        if (!(LuaHelper::GetParamCount(L) == FuncTraits<Fn>::arity)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        try {
            FuncCall<Fn>::call(L,fn);
            return 0;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        if (!(LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        try {
            FuncCall<MemFn>::call(L,object, fn);
            return 0;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }
};

} // namespace luabridge

#endif