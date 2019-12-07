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

#pragma once

#include "lua_stack.h"
#include <functional>
#include <type_traits>
#include "../myvm/lua_vmclass.h"

template<int unique_no, typename BOOST_FUNCTION>
struct lua_function
{
	static BOOST_FUNCTION fn;

	static int Call(lua_State* L)
	{_P_
		lua_stack l(L);

		l.Push( CLuaCFunctionWrapN<BOOST_FUNCTION::arity, BOOST_FUNCTION >()(l, fn) );
		return 1;
	}

	static int Call0(lua_State* L)
	{_P_
		lua_stack l(L);

		CLuaCFunctionWrapN<BOOST_FUNCTION::arity, BOOST_FUNCTION >()(l, fn);
		return 0;
	}
};

template<int unique_no, typename BOOST_FUNCTION>
BOOST_FUNCTION lua_function<unique_no, BOOST_FUNCTION>::fn;

// -----------------------
// Function Wrapp
// -----------------------

template<int unique_no, typename FT, typename F>
inline lua_CFunction LuaCFunctionWrap(F f)
{
	return LuaCFunctionWrapI<unique_no, boost::function<FT>, typename boost::function<FT>::result_type>()(boost::function<FT>(f));
}

template<int unique_no, typename BOOST_FUNCTION, typename R>
struct LuaCFunctionWrapI
{
	inline lua_CFunction operator() (BOOST_FUNCTION f)
	{
 		lua_function<unique_no, BOOST_FUNCTION>::fn = f;
		return & lua_function<unique_no, BOOST_FUNCTION>::Call;
	//	return 0;
	}
};

template<int unique_no, typename BOOST_FUNCTION>
struct LuaCFunctionWrapI<unique_no, BOOST_FUNCTION, void>
{
	inline lua_CFunction operator() (BOOST_FUNCTION f)
	{
		lua_function<unique_no, BOOST_FUNCTION >::fn = f;
		return & lua_function<unique_no, BOOST_FUNCTION>::Call0;
	}
};

// ------------------------
// Functin Marshal
// ------------------------

#define LUA_PARAM(n)	\
	typename boost::remove_const<boost::remove_reference<BOOST_FUNCTION::arg##n##_type>::type>::type

template<int N, typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		!!! ParametersTooMany !!!;
		l.LuaAssert(false, "Parameters number too many.");
		return 0;
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<0, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		return f();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<1, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 1, "Request 1 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<2, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{

		l.LuaAssert(l.GetParamCount() == 2, "Request 2 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<3, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 3, "Request 3 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<4, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 4, "Request 4 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<5, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 5, "Request 5 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<6, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 6, "Request 6 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5),
			l.GetParam<LUA_PARAM(6)>(6)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<7, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 7, "Request 7 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5),
			l.GetParam<LUA_PARAM(6)>(6),
			l.GetParam<LUA_PARAM(7)>(7)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<8, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 8, "Request 8 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5),
			l.GetParam<LUA_PARAM(6)>(6),
			l.GetParam<LUA_PARAM(7)>(7),
			l.GetParam<LUA_PARAM(8)>(8)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<9, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 9, "Request 9 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5),
			l.GetParam<LUA_PARAM(6)>(6),
			l.GetParam<LUA_PARAM(7)>(7),
			l.GetParam<LUA_PARAM(8)>(8),
			l.GetParam<LUA_PARAM(9)>(9)
			)
			();
	}
};

template<typename BOOST_FUNCTION>
struct CLuaCFunctionWrapN<10, BOOST_FUNCTION>
{
	inline typename BOOST_FUNCTION::result_type operator () (lua_stack& l, BOOST_FUNCTION f)
	{
		l.LuaAssert(l.GetParamCount() == 10, "Request 10 param");

		return boost::bind(
			f, 
			l.GetParam<LUA_PARAM(1)>(1),
			l.GetParam<LUA_PARAM(2)>(2),
			l.GetParam<LUA_PARAM(3)>(3),
			l.GetParam<LUA_PARAM(4)>(4),
			l.GetParam<LUA_PARAM(5)>(5),
			l.GetParam<LUA_PARAM(6)>(6),
			l.GetParam<LUA_PARAM(7)>(7),
			l.GetParam<LUA_PARAM(8)>(8),
			l.GetParam<LUA_PARAM(9)>(9),
			l.GetParam<LUA_PARAM(10)>(10)
			)
			();
	}
};
