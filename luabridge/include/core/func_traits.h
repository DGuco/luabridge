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
 * Traits class for unrolling the type list values into function arguments.
 */
template<class ReturnType, size_t NUM_PARAMS>
struct Caller;

template<class ReturnType>
struct Caller<ReturnType, 0>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &params)
    {
        return fn();
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &)
    {
        return (obj->*fn)();
    }
};

template<class ReturnType>
struct Caller<ReturnType, 1>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 2>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 3>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 4>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 5>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 6>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 7>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 8>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType>
struct Caller<ReturnType, 9>
{
    template<class Fn, class Params>
    static ReturnType f(Fn &fn, TypeListValues<Params> &tvl)
    {
        return fn(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd,
                  tvl.tl.tl.tl.tl.tl.tl.tl.tl.hd);
    }

    template<class T, class MemFn, class Params>
    static ReturnType f(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
    {
        return (obj->*fn)(tvl.hd, tvl.tl.hd, tvl.tl.tl.hd, tvl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.hd, tvl.tl.tl.tl.tl.tl.tl.tl.hd,
                          tvl.tl.tl.tl.tl.tl.tl.tl.tl.hd);
    }
};

template<class ReturnType, class Fn, class Params>
ReturnType doCall(Fn &fn, TypeListValues<Params> &tvl)
{
    return Caller<ReturnType, TypeListSize<Params>::value>::f(fn, tvl);
}

template<class ReturnType, class T, class MemFn, class Params>
static ReturnType doCall(T *obj, MemFn &fn, TypeListValues<Params> &tvl)
{
    return Caller<ReturnType, TypeListSize<Params>::value>::f(obj, fn, tvl);
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

    static R call(const DeclType &fp, TypeListValues<Params> &tvl)
    {
        return doCall<R>(fp, tvl);
    }
};

/* Windows: WINAPI (a.k.a. __stdcall) function pointers. */

#ifdef _M_IX86 // Windows 32bit only

template <class R, class... ParamList>
struct FuncTraits <R (__stdcall *) (ParamList...)>
{
  enum
  {
    arity = sizeof...(ParamList)
  };
  static bool const isMemberFunction = false;
  using DeclType = R (__stdcall *) (ParamList...);
  using ReturnType = R;
  using Params = typename MakeTypeList <ParamList...>::Result;

  static R call (const DeclType& fp, TypeListValues <Params>& tvl)
  {
    return doCall <R> (fp, tvl);
  }
};

#endif // _M_IX86

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

    static R call(ClassType *obj, const DeclType &fp, TypeListValues<Params> tvl)
    {
        return doCall<R>(obj, fp, tvl);
    }
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

    static R call(const ClassType *obj, const DeclType &fp, TypeListValues<Params> tvl)
    {
        return doCall<R>(obj, fp, tvl);
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

    static ReturnType call(DeclType &fn, TypeListValues<Params> &tvl)
    {
        return doCall<ReturnType>(fn, tvl);
    }
};

template<class ReturnType, class Params, int startParam>
struct Invoke
{
    template<class Fn>
    static int run(lua_State *L, Fn &fn)
    {
        if (!LuaHelper::GetParamCount(L) == FuncTraits<Fn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        ArgList<Params, startParam> args(L);
        try {
            Stack<ReturnType>::push(L, FuncTraits<Fn>::call(fn, args));
            return 1;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        if (!LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        ArgList<Params, startParam> args(L);
        try {
            Stack<ReturnType>::push(L, FuncTraits<MemFn>::call(object, fn, args));
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
        if (!LuaHelper::GetParamCount(L) == FuncTraits<Fn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<Fn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        ArgList<Params, startParam> args(L);
        try {
            FuncTraits<Fn>::call(fn, args);
            return 0;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }

    template<class T, class MemFn>
    static int run(lua_State *L, T *object, const MemFn &fn)
    {
        if (!LuaHelper::GetParamCount(L) == FuncTraits<MemFn>::arity) {
            char Msg[128] = {0};
            snprintf(Msg,
                     128,
                     "Param count error need = %d,in fact num = %d",
                     FuncTraits<MemFn>::arity,
                     LuaHelper::GetParamCount(L));
            LuaHelper::LuaAssert(L, false, Msg);
        }
        ArgList<Params, startParam> args(L);
        try {
            FuncTraits<MemFn>::call(object, fn, args);
            return 0;
        }
        catch (const std::exception &e) {
            return LuaHelper::LuaAssert(L, false, e.what());
        }
    }
};

} // namespace luabridge

#endif