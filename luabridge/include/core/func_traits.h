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

//get参数的类型
#define  LUA_PARAM_TYPE(n) typename ArgTypeList<ParamList...>::template args<n>::type

template<size_t NUM_PARAMS,class ReturnType,class... ParamList>
struct Caller;

template<class ReturnType,class... ParamList>
struct Caller<0,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn();
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)();
    }
};

template<class ReturnType,class... ParamList>
struct Caller<1,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<2,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<3,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<4,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<5,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<6,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<7,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<8,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<9,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<10,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<11,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<12,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<13,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<14,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L,startParam + 13));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L,startParam + 13));
    }
};

template<class ReturnType,class... ParamList>
struct Caller<15,ReturnType,ParamList...>
{
    template<class Fn>
    static ReturnType f(lua_State*L,Fn &fn,int startParam)
    {
        return fn(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                  Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                  Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                  Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                  Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                  Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                  Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                  Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                  Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                  Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                  Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                  Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                  Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12),
                  Stack<LUA_PARAM_TYPE(13)>::get(L,startParam + 13),
                  Stack<LUA_PARAM_TYPE(14)>::get(L,startParam + 14));
    }

    template<class T, class MemFn>
    static ReturnType f(lua_State*L,T *obj, MemFn &fn,int startParam)
    {
        return (obj->*fn)(Stack<LUA_PARAM_TYPE(0)>::get(L,startParam + 0),
                          Stack<LUA_PARAM_TYPE(1)>::get(L,startParam + 1),
                          Stack<LUA_PARAM_TYPE(2)>::get(L,startParam + 2),
                          Stack<LUA_PARAM_TYPE(3)>::get(L,startParam + 3),
                          Stack<LUA_PARAM_TYPE(4)>::get(L,startParam + 4),
                          Stack<LUA_PARAM_TYPE(5)>::get(L,startParam + 5),
                          Stack<LUA_PARAM_TYPE(6)>::get(L,startParam + 6),
                          Stack<LUA_PARAM_TYPE(7)>::get(L,startParam + 7),
                          Stack<LUA_PARAM_TYPE(8)>::get(L,startParam + 8),
                          Stack<LUA_PARAM_TYPE(9)>::get(L,startParam + 9),
                          Stack<LUA_PARAM_TYPE(10)>::get(L,startParam + 10),
                          Stack<LUA_PARAM_TYPE(11)>::get(L,startParam + 11),
                          Stack<LUA_PARAM_TYPE(12)>::get(L,startParam + 12),
                          Stack<LUA_PARAM_TYPE(13)>::get(L,startParam + 13),
                          Stack<LUA_PARAM_TYPE(14)>::get(L,startParam + 14));
    }
};

template<class ReturnType, class Fn, class... ParamList>
ReturnType doCall(lua_State*L,const Fn &fn,int startParam)
{
    return Caller<ArgTypeList<ParamList...>::arity,ReturnType,ParamList...>::f(L,fn,startParam);
}

template<class ReturnType, class T, class MemFn, class... ParamList>
static ReturnType doCall(lua_State*L,T *obj,const MemFn &fn,int startParam)
{
    return Caller<ArgTypeList<ParamList...>::arity,ReturnType, ParamList...>::f(L,obj, fn,startParam);
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
struct FuncTraits
{
    enum
    {
        arity = 0
    };
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

    static R call(lua_State*L,const DeclType &fp)
    {
        return doCall<R,DeclType ,ParamList...>(L,fp);
    }
};

/* Non-const member function pointers. */
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

    static R call(lua_State*L,ClassType *obj,const DeclType &fp,int startParam)
    {
        return doCall<R,T,DeclType,ParamList...>(L,obj, fp,startParam);
    }
};

/* Const member function pointers. */

template<class T, class R,class... ParamList>
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

    static R call(lua_State*L,const ClassType *obj,const DeclType &fp,int startParam)
    {
        return doCall<R,T,DeclType,ParamList...>(L,obj, fp,startParam);
    }
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

    static ReturnType call(lua_State*L,DeclType &fn,int startParam)
    {
        return doCall<ReturnType,DeclType,ParamList...>(L,fn,startParam);
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
            LUA_ASSERT(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncTraits<Fn>::call(L,fn,startParam));
            return 1;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        //参数个数:对象指针+成员函数参数个
        if (!(LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity + 1)) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity + 1,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            Stack<ReturnType>::push(L, FuncTraits<MemFn>::call(L,object, fn,startParam));
            return 1;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
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
            LUA_ASSERT(L, false, Msg);
        }
        try {
            FuncTraits<Fn>::call(L,fn,startParam);
            return 0;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        //参数个数:对象指针+成员函数参数个
        if (!LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity + 1) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity + 1,
                     LuaHelper::GetParamCount(L));
            LUA_ASSERT(L, false, Msg);
        }
        try {
            FuncTraits<MemFn>::call(L,object, fn,startParam);
            return 0;
        }
        catch (const std::exception &e) {
            return LUA_ASSERT(L, false, e.what());
        }
    }
};

} // namespace luabridge

#endif