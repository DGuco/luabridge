//------------------------------------------------------------------------------
/*
  https://github.com/DGuco/luabridge

  Copyright (C) 2021 DGuco(杜国超)<1139140929@qq.com>.  All rights reserved.
  Copyright 2019, Dmitry Tarakanov
  Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
  Copyright 2007, Nathan Reed

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

#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__

#include <functional>
#include <stdexcept>
#include <string>
#include <assert.h>
#include "lua_library.h"
#include "classinfo.h"
#include "lua_exception.h"
#include "security.h"
#include "type_traits.h"
#include "lua_helpers.h"
#include "lua_functions.h"
#include "constructor.h"

using namespace std;

namespace luabridge
{

namespace detail
{

/**
 * Base for class and namespace registration.
 * Maintains Lua stack in the proper state.
 * Once beginNamespace, beginClass or deriveClass is called the parent
 * object upon its destruction may no longer clear the Lua stack.
 * Then endNamespace or endClass is called, a new parent is created
 * and the child transfers the responsibility for clearing stack to it.
 * So there can be maximum one "active" registrar object.
 */
class Registrar
{
protected:
    lua_State *const L;
    int mutable m_stackSize;

    Registrar()
        : L(NULL)
    {
    }

    Registrar(lua_State *L)
        : L(L), m_stackSize(0)
    {
    }

    Registrar(const Registrar &rhs)
        : L(rhs.L), m_stackSize(rhs.m_stackSize)
    {
        rhs.m_stackSize = 0;
    }

    Registrar &operator=(const Registrar &rhs)
    {
        Registrar tmp(rhs);
        std::swap(m_stackSize, tmp.m_stackSize);
        return *this;
    }

    virtual ~Registrar()
    {
        if (m_stackSize > 0) {
            assert (m_stackSize <= lua_gettop(L));
            lua_pop(L, m_stackSize);
        }
    }

    void assertIsActive() const
    {
        if (m_stackSize == 0) {
            throw std::logic_error("Unable to continue registration");
        }
    }
};

} // namespace detail

/** Provides C++ to Lua registration capabilities.

    This class is not instantiated directly, call `getGlobalNamespace` to start
    the registration process.
*/
class Namespace: public detail::Registrar
{
public:
    //============================================================================
    /**
      Factored base to reduce template instantiations.
    */
    class ClassBase: public detail::Registrar
    {
    public:
        explicit ClassBase(Namespace &parent)
            : Registrar(parent)
        {
        }

    protected:
        //--------------------------------------------------------------------------
        /**
          Create the const table.
        */
        void createConstTable(const char *name, bool trueConst = true)
        {
            std::string type_name = std::string(trueConst ? "const " : "") + name;

            // Stack: namespace table (ns) //栈状态lua_gettop(L) == n + 1:栈状态:ns
            lua_newtable(L); // Stack: ns, const table (co)   栈状态lua_gettop(L) == n + 2:ns=>co
            lua_pushvalue(L, -1); // Stack: ns, co, co 栈状态lua_gettop(L) == n + 3:ns=>co=>co
            lua_setmetatable(L, -2); // co.__metatable = co. 栈状态lua_gettop(L) == n + 2:ns=>co
            lua_pushstring(L, type_name.c_str()); // const table name 栈状态lua_gettop(L)== n + 3:ns=>co=>type_name
            lua_rawsetp(L, -2, getTypeKey()); // co [typeKey] = type_name. 栈状态lua_gettop(L)== n + 2:ns=>co

            /**
             *https://zilongshanren.com/post/bind-a-simple-cpp-class-in-lua/
             *https://blog.csdn.net/qiuwen_521/article/details/107855867
             *用Lua里面的面向对象的方式来访问。
             *local s = cc.create()
             *s:setName("zilongshanren")
             *s:setAge(20)
             *s:print()
             *s:setName(xx)就等价于s.setName(s,xx)，此时我们只需要给s提供一个metatable,并且给这个metatable设置一个key为"__index"，
             *value等于它本身的metatable。最后，只需要把之前Student类的一些方法添加到这个metatable里面就可以了,或者key为"__index"，
             *value位一个function(t,k)类型的函数，函数中可以根据k获取对应的类的方法,这里的实现是后者
            **/
            lua_pushcfunction(L, &CFunc::IndexMetaMethod); //栈状态lua_gettop(L)== n + 3:ns=>co=>IndexMetaMethod
            //co.__index = &CFunc::IndexMetaMethod 栈状态lua_gettop(L)== n + 2:ns=>co
            LuaHelper::RawSetField(L, -2, "__index");

            lua_pushcfunction(L,
                              &CFunc::newindexObjectMetaMethod); //栈状态lua_gettop(L)== n + 3:ns=>co=>newindexObjectMetaMethod
            //co.__newindex = &CFunc::newindexObjectMetaMethod 栈状态lua_gettop(L)== n + 2:ns=>co
            LuaHelper::RawSetField(L, -2, "__newindex");

            lua_newtable(L); //propget table(gt) 栈状态lua_gettop(L)== n + 3:ns=>co=>gt
            lua_rawsetp(L, -2, getPropgetKey());//co[progetkey] = gt 栈状态lua_gettop(L)== n + 2:ns=>co

            if (Security::hideMetatables()) {
                lua_pushnil(L);  //栈状态lua_gettop(L)== n + 3:ns=>co=>nil
                LuaHelper::RawSetField(L, -2, "__metatable"); //co.__metatable = nil 栈状态lua_gettop(L)== n + 2:ns=>co
            }
            //now 栈状态lua_gettop(L)== n + 2:ns=>co
        }

        //--------------------------------------------------------------------------
        /**
          Create the class table.

          The Lua stack should have the const table on top.
        */
        void createClassTable(char const *name)
        {
            // Stack: namespace table (ns), const table (co)
            // Stack 栈状态lua_gettop(L) == n + 2:ns=>co

            // Class table is the same as const table except the propset table
            createConstTable(name, false); // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl

            lua_newtable(L); // propset table (ps)  Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>ps
            lua_rawsetp(L, -2, getPropsetKey()); // cl [propsetKey] = ps. Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl

            lua_pushvalue(L, -2); // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl=>co
            lua_rawsetp(L, -2, getConstKey()); // cl [constKey] = co. Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl=>co

            lua_pushvalue(L, -1); // Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>cl
            lua_rawsetp(L, -3, getClassKey()); // co [classKey] = cl.Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
            //now 栈状态lua_gettop(L)== n + 3:ns=>co=>cl
        }

        //--------------------------------------------------------------------------
        /**
          Create the static table.
        */
        void createStaticTable(char const *name)
        {
            // Stack: namespace table (ns), const table (co), class table (cl)
            // Stack 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
            lua_newtable(L); //visible static table (vst) Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>vst
            lua_newtable(L); //static metatable (st) Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>vst=>st
            lua_pushvalue(L, -1); // Stack 栈状态lua_gettop(L) == n + 6:ns=>co=>cl=>vst=>st=>st
            lua_setmetatable(L, -3); // vst.__metatable = st. Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>vst=>st
            lua_insert(L, -2); // Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>vst
            LuaHelper::RawSetField(L, -5, name); // ns [name] = vst. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st


            lua_pushcfunction(L,
                              &CFunc::IndexMetaMethod); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>IndexMetaMethod
            //st.__index = IndexMetaMethod,Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            LuaHelper::RawSetField(L, -2, "__index");

            lua_pushcfunction(L,
                              &CFunc::newindexStaticMetaMethod); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>newindexStaticMetaMethod
            //st.__newindex = newindexStaticMetaMethod,Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            LuaHelper::RawSetField(L, -2, "__newindex");

            lua_newtable(L); // proget table (pg) Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>pg
            lua_rawsetp(L, -2, getPropgetKey()); // st [propgetKey] = pg. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            lua_newtable(L); // Stack: ns, co, cl, st, propset table (ps) Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>ps
            lua_rawsetp(L, -2, getPropsetKey()); // st [propsetKey] = pg. Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            lua_pushvalue(L, -2); //Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
            lua_rawsetp(L, -2, getClassKey()); // st [classKey] = cl.  Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

            if (Security::hideMetatables()) {
                lua_pushnil(L); // Stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>nil
                LuaHelper::RawSetField(L,
                                       -2,
                                       "__metatable"); //st.__metatable = nil   Stack 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            }
            //now 栈状态lua_gettop(L)== n + 4:ns=>co=>cl=>st
        }

        //==========================================================================
        /**
          lua_CFunction to construct a class object wrapped in a container.
        */
        template<class MemFn,class C>
        static int ctorContainerProxy(lua_State *L)
        {
            typedef typename ContainerTraits<C>::Type T;
            T *const p = FuncTraits<MemFn>::template callnew<T>(L,2);
            UserdataSharedHelper<C, false>::push(L, p);
            return 1;
        }

        //--------------------------------------------------------------------------
        /**
          lua_CFunction to construct a class object in-place in the userdata.
        */
        template<class MemFn,class T>
        static int ctorPlacementProxy(lua_State *L)
        {
            UserdataValue<T> *value = UserdataValue<T>::place(L);
            FuncTraits<MemFn>::template callnew<T>(L,value->getObject(),2);
            value->commit();
            return 1;
        }

        void assertStackState() const
        {
            // Stack: const table (co), class table (cl), static table (st)
            assert (lua_istable(L, -3));
            assert (lua_istable(L, -2));
            assert (lua_istable(L, -1));
        }
    };

    //============================================================================
    //
    // Class
    //
    //============================================================================
    /**
      Provides a class registration in a lua_State.

      After construction the Lua stack holds these objects:
        -1 static table
        -2 class table
        -3 const table
        -4 enclosing namespace table
    */
    template<class T>
    class Class: public ClassBase
    {
    public:
        //==========================================================================
        /**
          Register a new class or add to an existing class registration.
          构造函数完成后
          vst(visible static table) = {
              __metatable =  {
                  __index = &CFunc::IndexMetaMethod,
                  __newindex = &CFunc::newindexStaticMetaMethod,
              }
          }
          co(const table) = {
              __metatable = co,
              typekey = const_name,
              __index = &CFunc::IndexMetaMethod,
              __newindex = &CFunc::newindexStaticMetaMethod,
              __gc = &CFunc::gcMetaMethod<T>,
              __call = class Constructor(class Constructor 会被注册在这个表里),
              propgetKey = {table}(通过addProperty注册普通成员变量的get方法会注册在这里),
              classKey = cl,
              func1_name = func1,(普通成员函数1 会被注册在这个表里),
              func2_name = func12,(普通成员函数2 会被注册在这个表里),
          }

          cl(class table) = {
              __metatable = cl,
              typekey = name,
              __index = &CFunc::IndexMetaMethod,
              __newindex = &CFunc::newindexStaticMetaMethod,
              __gc = &CFunc::gcMetaMethod<T>,
              propgetKey = {}(table)(通过addProperty注册普通成员变量的get方法也会注册在这里),
              propsetKey = {}(table)(通过addProperty注册普通成员变量的set方法也会注册在这里),,
              constKey = co,
              func1_name = func1,(普通成员函数1 也会被注册在这个表里),
              func2_name = func12,(普通成员函数2 也会被注册在这个表里),
          }

          st(static table) = {
              __index = &CFunc::IndexMetaMethod,
              __newindex = &CFunc::newindexStaticMetaMethod,
              propgetKey = {}(table)(通过addStaticProperty注册静态成员变量的get方法会注册在这里),
              propsetKey = {}(table)(通过addStaticProperty注册静态成员变量的set方法会注册在这里),
              classKey = cl,
              static_func1_name = func1,(static成员函数1 也会被注册在这个表里),
              static_ffunc2_name = func12,(static成员函数2 也会被注册在这个表里),
          }

          _G = {
              name = vst;
          }

          registry = {
              ClassInfo<T>_static_key = st,
              ClassInfo<T>_class_key = cl,
              ClassInfo<T>_const_key = co,
          }
         **/
        Class(char const *name, Namespace &parent)
            : ClassBase(parent)
        {
            //栈顶是否是表(_G)
            LUA_ASSERT_EX(L, lua_istable(L, -1), "lua_istable(L, -1)", false); //栈状态lua_gettop(L)== n + 1:ns
            //尝试find类的metadata表_G[name]
            LuaHelper::RawGetField(L, -1, name); // st = _G[name] 栈状态ua_gettop(L)== n + 2:ns=>st|nil

            //表不存在create it
            if (lua_isnil(L, -1)) // Stack: ns, nil 栈状态:ns=>nil
            {
                //弹出nil值
                lua_pop(L, 1); // Stack: ns 栈状态ua_gettop(L)== n + 1:ns
                //create类的const metadata表(co),and set co.__metatable = co
                createConstTable(name);
                //now 栈状态lua_gettop(L)== n + 2:ns=>co
                //注册gc元方法
                lua_pushcfunction(L, &CFunc::gcMetaMethod<T>); // 栈状态lua_gettop(L)== n + 3:ns=>co=>gcfun
                //co.__gc = gcfun 即co.__metatable.__gc = gcfun 栈状态lua_gettop(L)== n + 2:ns=>co
                LuaHelper::RawSetField(L, -2, "__gc");
                ++m_stackSize;

                //create类的非const metadata表 and set cl.__metatable = cl
                createClassTable(name); //class table (cl) stack栈状态lua_gettop(L)== n + 3:ns=>co=>cl
                //now 栈状态lua_gettop(L) == n + 3:ns=>co=>cl
                //注册gc元方法
                lua_pushcfunction(L, &CFunc::gcMetaMethod<T>); //gcfun stack栈状态lua_gettop(L)== n + 4:ns=>co=>cl=>gcfun
                //cl.__gc = gcfun 即 cl.__metatable.__gc = gcfun stack栈状态lua_gettop(L)== n + 3:ns=>co=>cl
                LuaHelper::RawSetField(L, -2, "__gc");
                ++m_stackSize;

                //create类的static metadata表 and set st.__metatable = st
                createStaticTable(name); // Stack: ns, co, cl, st 栈状态:ns=>co=>cl=>st
                //now 栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
                ++m_stackSize;

                //Map T back to its tables.
                //把st元表作一个副本压栈。
                lua_pushvalue(L, -1); // 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>st
                //把static metadata表插入registry表
                lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getStaticKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
                //把cl元表作一个副本压栈。
                lua_pushvalue(L, -2); // stack 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>cl
                //把metadata表插入registry表
                lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getClassKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
                //把co元表作一个副本压栈。
                lua_pushvalue(L, -3); // 栈状态lua_gettop(L) == n + 5:ns=>co=>cl=>st=>co
                //把const metadata表插入registry表
                lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getConstKey()); //stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st

                //now stack栈状态lua_gettop(L) == n + 4:ns=>co=>cl=>st
            }
            else { //表存在
                LUA_ASSERT_EX (L, lua_istable(L, -1), "lua_istable(L, -1)", false); // Stack: 栈状态ua_gettop(L)== n + 2:ns=>st
                ++m_stackSize;

                // Map T back from its stored tables
                lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getConstKey()); // Stack 栈状态ua_gettop(L)== n + 3:ns=>st=>co
                //调整co的位置
                lua_insert(L, -2); // Stack 栈状态ua_gettop(L)== n + 3:ns=>co=>st
                ++m_stackSize;

                lua_rawgetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getClassKey()); // Stack 栈状态ua_gettop(L)== n + 4:ns=>co=>st=>cl
                //调整cl的位置
                lua_insert(L, -2); // Stack 栈状态ua_gettop(L)== n + 4:ns=>co=>cl=>st
                ++m_stackSize;
            }
        }

        //==========================================================================
        /**
          Derive a new class.
        */
        Class(char const *name, Namespace &parent, void const *const staticKey)
            : ClassBase(parent)
        {
            assert (lua_istable(L, -1)); // Stack: namespace table (ns)

            createConstTable(name); // Stack: ns, const table (co)
            lua_pushcfunction(L, &CFunc::gcMetaMethod<T>); // Stack: ns, co, function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: ns, co
            ++m_stackSize;

            createClassTable(name); // Stack: ns, co, class table (cl)
            lua_pushcfunction(L, &CFunc::gcMetaMethod<T>); // Stack: ns, co, cl, function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: ns, co, cl
            ++m_stackSize;

            createStaticTable(name); // Stack: ns, co, cl, st
            ++m_stackSize;

            lua_rawgetp(L, LUA_REGISTRYINDEX, staticKey); // Stack: ns, co, cl, st, parent st (pst) | nil
            if (lua_isnil(L, -1)) // Stack: ns, co, cl, st, nil
            {
                ++m_stackSize;
                throw std::runtime_error("Base class is not registered");
            }

            assert (lua_istable(L, -1)); // Stack: ns, co, cl, st, pst

            lua_rawgetp(L, -1, getClassKey()); // Stack: ns, co, cl, st, pst, parent cl (pcl)
            assert (lua_istable(L, -1));

            lua_rawgetp(L, -1, getConstKey()); // Stack: ns, co, cl, st, pst, pcl, parent co (pco)
            assert (lua_istable(L, -1));

            lua_rawsetp(L, -6, getParentKey()); // co [parentKey] = pco. Stack: ns, co, cl, st, pst, pcl
            lua_rawsetp(L, -4, getParentKey()); // cl [parentKey] = pcl. Stack: ns, co, cl, st, pst
            lua_rawsetp(L, -2, getParentKey()); // st [parentKey] = pst. Stack: ns, co, cl, st

            lua_pushvalue(L, -1); // Stack: ns, co, cl, st, st
            lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getStaticKey()); // Stack: ns, co, cl, st
            lua_pushvalue(L, -2); // Stack: ns, co, cl, st, cl
            lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getClassKey()); // Stack: ns, co, cl, st
            lua_pushvalue(L, -3); // Stack: ns, co, cl, st, co
            lua_rawsetp(L, LUA_REGISTRYINDEX, ClassInfo<T>::getConstKey()); // Stack: ns, co, cl, st
        }

        //--------------------------------------------------------------------------
        /**
          Continue registration in the enclosing namespace.
        */
        Namespace endClass()
        {
            assert (m_stackSize > 3);
            m_stackSize -= 3;
            lua_pop(L, 3);
            return Namespace(*this);
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a static data member.
        */
        template<class U>
        Class<T> &addStaticProperty(char const *name, U *pu, bool isWritable = true)
        {
            return addStaticData(name, pu, isWritable);
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a static data member.
        */
        template<class U>
        Class<T> &addStaticData(char const *name, U *pu, bool isWritable = true)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, pu); // Stack: co, cl, st, pointer
            lua_pushcclosure(L, &CFunc::getVariable<U>, 1); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -2); // Stack: co, cl, st

            if (isWritable) {
                lua_pushlightuserdata(L, pu); // Stack: co, cl, st, ps, pointer
                lua_pushcclosure(L, &CFunc::setVariable<U>, 1); // Stack: co, cl, st, ps, setter
            }
            else {
                lua_pushstring(L, name); // Stack: co, cl, st, name
                lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: co, cl, st, error_fn
            }
            CFunc::addSetter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a static property member.

          If the set function is null, the property is read-only.
        */
        template<class U>
        Class<T> &addStaticProperty(char const *name, U (*get)(), void (*set)(U) = 0)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::Call<U (*)()>::f, 1); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -2); // Stack: co, cl, st

            if (set != 0) {
                lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: co, cl, st, function ptr
                lua_pushcclosure(L, &CFunc::Call<void (*)(U)>::f, 1); // Stack: co, cl, st, setter
            }
            else {
                lua_pushstring(L, name); // Stack: co, cl, st, ps, name
                lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: co, cl, st, error_fn
            }
            CFunc::addSetter(L, name, -2); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a static member function.
        */
        template<class FP>
        Class<T> &addStaticFunction(char const *name, FP const fp)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast <void *> (fp)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::Call<FP>::f, 1); // co, cl, st, function
            LuaHelper::RawSetField(L, -2, name); // co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a lua_CFunction.
        */
        Class<T> &addStaticFunction(char const *name, int (*const fp)(lua_State *))
        {
            return addStaticCFunction(name, fp);
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a lua_CFunction.
        */
        Class<T> &addStaticCFunction(char const *name, int (*const fp)(lua_State *))
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcfunction(L, fp); // co, cl, st, function
            LuaHelper::RawSetField(L, -2, name); // co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a data member.
        */
        template<class U>
        Class<T> &addProperty(char const *name, U T::* mp, bool isWritable = true)
        {
            return addData(name, mp, isWritable);
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a data member.
        */
        template<class U>
        Class<T> &addData(char const *name, U T::* mp, bool isWritable = true)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            typedef const U T::*mp_t;
            new(lua_newuserdata(L, sizeof(mp_t))) mp_t(mp); // Stack: co, cl, st, field ptr
            lua_pushcclosure(L, &CFunc::getProperty<T, U>, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            CFunc::addGetter(L, name, -5); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -3); // Stack: co, cl, st

            if (isWritable) {
                new(lua_newuserdata(L, sizeof(mp_t))) mp_t(mp); // Stack: co, cl, st, field ptr
                lua_pushcclosure(L, &CFunc::setProperty<T, U>, 1); // Stack: co, cl, st, setter
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a property member.
        */
        template<class TG, class TS = TG>
        Class<T> &addProperty(char const *name, TG (T::* get)() const, void (T::* set)(TS) = 0)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            typedef TG (T::*get_t)() const;
            new(lua_newuserdata(L, sizeof(get_t))) get_t(get); // Stack: co, cl, st, funcion ptr
            lua_pushcclosure(L, &CFunc::CallConstMember<get_t>::f, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            CFunc::addGetter(L, name, -5); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -3); // Stack: co, cl, st

            if (set != 0) {
                typedef void (T::* set_t)(TS);
                new(lua_newuserdata(L, sizeof(set_t))) set_t(set); // Stack: co, cl, st, function ptr
                lua_pushcclosure(L, &CFunc::CallMember<set_t>::f, 1); // Stack: co, cl, st, setter
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a property member.
        */
        template<class TG, class TS = TG>
        Class<T> &addProperty(char const *name, TG (T::* get)(lua_State *) const, void (T::* set)(TS, lua_State *) = 0)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            typedef TG (T::*get_t)(lua_State *) const;
            new(lua_newuserdata(L, sizeof(get_t))) get_t(get); // Stack: co, cl, st, funcion ptr
            lua_pushcclosure(L, &CFunc::CallConstMember<get_t>::f, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            CFunc::addGetter(L, name, -5); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -3); // Stack: co, cl, st

            if (set != 0) {
                typedef void (T::* set_t)(TS, lua_State *);
                new(lua_newuserdata(L, sizeof(set_t))) set_t(set); // Stack: co, cl, st, function ptr
                lua_pushcclosure(L, &CFunc::CallMember<set_t>::f, 1); // Stack: co, cl, st, setter
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a property member, by proxy.

          When a class is closed for modification and does not provide (or cannot
          provide) the function signatures necessary to implement get or set for
          a property, this will allow non-member functions act as proxies.

          Both the get and the set functions require a T const* and T* in the first
          argument respectively.
        */
        template<class TG, class TS = TG>
        Class<T> &addProperty(char const *name, TG (*get)(T const *), void (*set)(T *, TS) = 0)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::Call<TG (*)(const T *)>::f, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
            CFunc::addGetter(L, name, -5); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -3); // Stack: co, cl, st

            if (set != 0) {
                lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: co, cl, st, function ptr
                lua_pushcclosure(L, &CFunc::Call<void (*)(T *, TS)>::f, 1); // Stack: co, cl, st, setter
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a property member, by proxy C-function.

          When a class is closed for modification and does not provide (or cannot
          provide) the function signatures necessary to implement get or set for
          a property, this will allow non-member functions act as proxies.

          The object userdata ('this') value is at the index 1.
          The new value for set function is at the index 2.
        */
        Class<T> &addProperty(char const *name, int (*get)(lua_State *), int (*set)(lua_State *) = 0)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcfunction(L, get);
            lua_pushvalue(L, -1); // Stack: co, cl, st,, getter, getter
            CFunc::addGetter(L, name, -5); // Stack: co, cl, st,, getter
            CFunc::addGetter(L, name, -3); // Stack: co, cl, st,

            if (set != 0) {
                lua_pushcfunction(L, set);
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st,
            }

            return *this;
        }

#ifdef LUABRIDGE_CXX11
        template<class TG, class TS = TG>
        Class<T> &addProperty(char const *name, std::function<TG(const T *)> get, std::function<void(T *, TS)> set = nullptr)
        {
            using GetType = decltype(get);
            new(lua_newuserdata(L, sizeof(get))) GetType(std::move(get)); // Stack: co, cl, st, function userdata (ud)
            lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
            lua_pushcfunction (L, &CFunc::gcMetaMethodAny<GetType>); // Stack: co, cl, st, ud, mt, gc function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
            lua_setmetatable(L, -2); // Stack: co, cl, st, ud
            lua_pushcclosure(L, &CFunc::CallProxyFunctor<GetType>::f, 1); // Stack: co, cl, st, getter
            lua_pushvalue(L, -1); // Stack: co, cl, st, getter, getter
            CFunc::addGetter(L, name, -4); // Stack: co, cl, st, getter
            CFunc::addGetter(L, name, -4); // Stack: co, cl, st

            if (set != nullptr) {
                using SetType = decltype(set);
                new(lua_newuserdata(L, sizeof(set))) SetType(std::move(set)); // Stack: co, cl, st, function userdata (ud)
                lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
                lua_pushcfunction (L, &CFunc::gcMetaMethodAny<SetType>); // Stack: co, cl, st, ud, mt, gc function
                LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
                lua_setmetatable(L, -2); // Stack: co, cl, st, ud
                lua_pushcclosure(L, &CFunc::CallProxyFunctor<SetType>::f, 1); // Stack: co, cl, st, setter
                CFunc::addSetter(L, name, -3); // Stack: co, cl, st
            }

            return *this;
        }

#endif // LUABRIDGE_CXX11

#ifndef LUABRIDGE_CXX11

        //--------------------------------------------------------------------------
        /**
            Add or replace a member function.
        */
        template<class MemFn>
        Class<T> &addFunction(char const *name, MemFn mf)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            static const std::string GC = "__gc";
            if (name == GC) {
                throw std::logic_error(GC + " metamethod registration is forbidden");
            }
            CFunc::CallMemberFunctionHelper<MemFn, FuncTraits<MemFn>::isConstMemberFunction>::add(L, name, mf);
            return *this;
        }

#else // ifndef LUABRIDGE_CXX11

        //--------------------------------------------------------------------------
        /**
            Add or replace a member function by std::function.
        */
        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, std::function<ReturnType(T *, Params...)> function)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using FnType = decltype(function);
            new(lua_newuserdata(L,
                                sizeof(function))) FnType(std::move(function)); // Stack: co, cl, st, function userdata (ud)
            lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
            lua_pushcfunction (L, &CFunc::gcMetaMethodAny<FnType>); // Stack: co, cl, st, ud, mt, gc function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
            lua_setmetatable(L, -2); // Stack: co, cl, st, ud
            lua_pushcclosure(L, &CFunc::CallProxyFunctor<FnType>::f, 1); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a const member function by std::function.
        */
        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, std::function<ReturnType(const T *, Params...)> function)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            using FnType = decltype(function);
            new(lua_newuserdata(L,
                                sizeof(function))) FnType(std::move(function)); // Stack: co, cl, st, function userdata (ud)
            lua_newtable (L); // Stack: co, cl, st, ud, ud metatable (mt)
            lua_pushcfunction (L, &CFunc::gcMetaMethodAny<FnType>); // Stack: co, cl, st, ud, mt, gc function
            LuaHelper::RawSetField(L, -2, "__gc"); // Stack: co, cl, st, ud, mt
            lua_setmetatable(L, -2); // Stack: co, cl, st, ud
            lua_pushcclosure(L, &CFunc::CallProxyFunctor<FnType>::f, 1); // Stack: co, cl, st, function
            lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a member function.
        */
        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, ReturnType (T::* mf)(Params...))
        {
            using MemFn = ReturnType (T::*)(Params...);

            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            static const std::string GC = "__gc";
            if (name == GC) {
                throw std::logic_error(GC + " metamethod registration is forbidden");
            }
            CFunc::CallMemberFunctionHelper<MemFn, false>::add(L, name, mf);
            return *this;
        }

        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, ReturnType (T::* mf)(Params...) const)
        {
            using MemFn = ReturnType (T::*)(Params...) const;

            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            static const std::string GC = "__gc";
            if (name == GC) {
                throw std::logic_error(GC + " metamethod registration is forbidden");
            }
            CFunc::CallMemberFunctionHelper<MemFn, true>::add(L, name, mf);
            return *this;
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a proxy function.
        */
        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, ReturnType (*proxyFn)(T *object, Params...))
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            static const std::string GC = "__gc";
            if (name == GC) {
                throw std::logic_error(GC + " metamethod registration is forbidden");
            }
            using FnType = decltype(proxyFn);
            lua_pushlightuserdata(L, reinterpret_cast <void *> (proxyFn)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::CallProxyFunction<FnType>::f, 1); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st
            return *this;
        }

        template<class ReturnType, class... Params>
        Class<T> &addFunction(char const *name, ReturnType (*proxyFn)(const T *object, Params...))
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            static const std::string GC = "__gc";
            if (name == GC) {
                throw std::logic_error(GC + " metamethod registration is forbidden");
            }
            using FnType = decltype(proxyFn);
            lua_pushlightuserdata(L, reinterpret_cast <void *> (proxyFn)); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::CallProxyFunction<FnType>::f, 1); // Stack: co, cl, st, function
            lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st
            return *this;
        }

#endif

        //--------------------------------------------------------------------------
        /**
            Add or replace a member lua_CFunction.
        */
        Class<T> &addFunction(char const *name, int (T::*mfp)(lua_State *))
        {
            return addCFunction(name, mfp);
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a member lua_CFunction.
        */
        Class<T> &addCFunction(char const *name, int (T::*mfp)(lua_State *))
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            typedef int (T::*MFP)(lua_State *);
            new(lua_newuserdata(L, sizeof(mfp))) MFP(mfp); // Stack: co, cl, st, function ptr
            lua_pushcclosure(L, &CFunc::CallMemberCFunction<T>::f, 1); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -3, name); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a const member lua_CFunction.
        */
        Class<T> &addFunction(char const *name, int (T::*mfp)(lua_State *) const)
        {
            return addCFunction(name, mfp);
        }

        //--------------------------------------------------------------------------
        /**
            Add or replace a const member lua_CFunction.
        */
        Class<T> &addCFunction(char const *name, int (T::*mfp)(lua_State *) const)
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            typedef int (T::*MFP)(lua_State *) const;
            new(lua_newuserdata(L, sizeof(mfp))) MFP(mfp);
            lua_pushcclosure(L, &CFunc::CallConstMemberCFunction<T>::f, 1);
            lua_pushvalue(L, -1); // Stack: co, cl, st, function, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st, function
            LuaHelper::RawSetField(L, -4, name); // Stack: co, cl, st

            return *this;
        }

        //--------------------------------------------------------------------------
        /**
          Add or replace a primary Constructor.

          The primary Constructor is invoked when calling the class type table
          like a function.

          The template parameter should be a function pointer type that matches
          the desired Constructor (since you can't take the address of a Constructor
          and pass it as an argument).
        */
        template<class MemFn, class C>
        Class<T> &addConstructor()
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcclosure(L, &ctorContainerProxy<MemFn,C> , 0);
            LuaHelper::RawSetField(L, -2, "__call");

            return *this;
        }

        template<class MemFn>
        Class<T> &addConstructor()
        {
            assertStackState(); // Stack: const table (co), class table (cl), static table (st)

            lua_pushcclosure(L, &ctorPlacementProxy<MemFn,T>, 0);
            LuaHelper::RawSetField(L, -2, "__call");

            return *this;
        }
    };
private:
    //----------------------------------------------------------------------------
    /**
        Open the global namespace for registrations.
    */
    explicit Namespace(lua_State *L)
        : Registrar(L)
    {
        lua_getglobal(L, "_G");
        ++m_stackSize;
    }

    //----------------------------------------------------------------------------
    /**
        Open a namespace for registrations.

        The namespace is created if it doesn't already exist.
        The parent namespace is at the top of the Lua stack.
    */
    Namespace(char const *name, Namespace &parent)
        : Registrar(parent)
    {
        assert (lua_istable(L, -1)); // Stack: parent namespace (pns)

        LuaHelper::RawGetField(L, -1, name); // Stack: pns, namespace (ns) | nil

        if (lua_isnil(L, -1)) // Stack: pns, nil
        {
            lua_pop(L, 1); // Stack: pns

            lua_newtable(L); // Stack: pns, ns
            lua_pushvalue(L, -1); // Stack: pns, ns, ns

            // na.__metatable = ns
            lua_setmetatable(L, -2); // Stack: pns, ns

            // ns.__index = IndexMetaMethod
            lua_pushcfunction(L, &CFunc::IndexMetaMethod);
            LuaHelper::RawSetField(L, -2, "__index"); // Stack: pns, ns

            // ns.__newindex = newindexMetaMethod
            lua_pushcfunction(L, &CFunc::newindexStaticMetaMethod);
            LuaHelper::RawSetField(L, -2, "__newindex"); // Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propget table (pg)
            lua_rawsetp(L, -2, getPropgetKey()); // ns [propgetKey] = pg. Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propset table (ps)
            lua_rawsetp(L, -2, getPropsetKey()); // ns [propsetKey] = ps. Stack: pns, ns

            // pns [name] = ns
            lua_pushvalue(L, -1); // Stack: pns, ns, ns
            LuaHelper::RawSetField(L, -3, name); // Stack: pns, ns
#if 0
            lua_pushcfunction (L, &tostringMetaMethod);
            rawsetfield (L, -2, "__tostring");
#endif
        }

        ++m_stackSize;
    }

    //----------------------------------------------------------------------------
    /**
        Close the class and continue the namespace registrations.
    */
    explicit Namespace(ClassBase &child)
        : Registrar(child)
    {
    }

public:
    //----------------------------------------------------------------------------
    /**
        Open the global namespace.
    */
    static Namespace GetGlobalNamespace(lua_State *L)
    {
        enableExceptions(L);
        return Namespace(L);
    }

    //----------------------------------------------------------------------------
    /**
        Open a new or existing namespace for registrations.
    */
    Namespace BeginNamespace(char const *name)
    {
        assertIsActive();
        return Namespace(name, *this);
    }

    //----------------------------------------------------------------------------
    /**
        Continue namespace registration in the parent.

        Do not use this on the global namespace.
    */
    Namespace endNamespace()
    {
        if (m_stackSize == 1) {
            throw std::logic_error("endNamespace () called on global namespace");
        }

        assert (m_stackSize > 1);
        --m_stackSize;
        lua_pop(L, 1);
        return Namespace(*this);
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a variable.
    */
    template<class T>
    Namespace &addProperty(char const *name, T *pt, bool isWritable = true)
    {
        return addVariable(name, pt, isWritable);
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a variable.
    */
    template<class T>
    Namespace &addVariable(char const *name, T *pt, bool isWritable = true)
    {
        if (m_stackSize == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, pt); // Stack: ns, pointer
        lua_pushcclosure(L, &CFunc::getVariable<T>, 1); // Stack: ns, getter
        CFunc::addGetter(L, name, -2); // Stack: ns

        if (isWritable) {
            lua_pushlightuserdata(L, pt); // Stack: ns, pointer
            lua_pushcclosure(L, &CFunc::setVariable<T>, 1); // Stack: ns, setter
        }
        else {
            lua_pushstring(L, name); // Stack: ns, ps, name
            lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: ns, error_fn
        }
        CFunc::addSetter(L, name, -2); // Stack: ns

        return *this;
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a property.

        If the set function is omitted or null, the property is read-only.
    */
    template<class TG, class TS = TG>
    Namespace &addProperty(char const *name, TG (*get)(), void (*set)(TS) = 0)
    {
        if (m_stackSize == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: ns, function ptr
        lua_pushcclosure(L, &CFunc::Call<TG (*)()>::f, 1); // Stack: ns, getter
        CFunc::addGetter(L, name, -2);

        if (set != 0) {
            lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: ns, function ptr
            lua_pushcclosure(L, &CFunc::Call<void (*)(TS)>::f, 1);
        }
        else {
            lua_pushstring(L, name);
            lua_pushcclosure(L, &CFunc::readOnlyError, 1);
        }
        CFunc::addSetter(L, name, -2);

        return *this;
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a property.
        If the set function is omitted or null, the property is read-only.
    */
    Namespace &addProperty(char const *name, int (*get)(lua_State *), int (*set)(lua_State *) = 0)
    {
        if (m_stackSize == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)
        lua_pushcfunction(L, get); // Stack: ns, getter
        CFunc::addGetter(L, name, -2); // Stack: ns
        if (set != 0) {
            lua_pushcfunction(L, set); // Stack: ns, setter
            CFunc::addSetter(L, name, -2); // Stack: ns
        }
        else {
            lua_pushstring(L, name); // Stack: ns, name
            lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: ns, name, readOnlyError
            CFunc::addSetter(L, name, -2); // Stack: ns
        }

        return *this;
    }

//----------------------------------------------------------------------------
    /**
        Add or replace a free function.
    */
    template<class FP>
    Namespace &addFunction(char const *name, FP const fp)
    {
        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast <void *> (fp)); // Stack: ns, function ptr
        lua_pushcclosure(L, &CFunc::Call<FP>::f, 1); // Stack: ns, function
        LuaHelper::RawSetField(L, -2, name); // Stack: ns
        return *this;
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a lua_CFunction.
    */
    Namespace &addFunction(char const *name, int (*const fp)(lua_State *))
    {
        return addCFunction(name, fp);
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a lua_CFunction.
    */
    Namespace &addCFunction(char const *name, int (*const fp)(lua_State *))
    {
        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushcfunction(L, fp); // Stack: ns, function
        LuaHelper::RawSetField(L, -2, name); // Stack: ns

        return *this;
    }

    //----------------------------------------------------------------------------
    /**
        Open a new or existing class for registrations.
    */
    template<class T>
    Class<T> beginClass(char const *name)
    {
        assertIsActive();
        return Class<T>(name, *this);
    }

    //----------------------------------------------------------------------------
    /**
        Derive a new class for registrations.

        To continue registrations for the class later, use beginClass ().
        Do not call deriveClass () again.
    */
    template<class Derived, class Base>
    Class<Derived> deriveClass(char const *name)
    {
        assertIsActive();
        return Class<Derived>(name, *this, ClassInfo<Base>::getStaticKey());
    }
};

//------------------------------------------------------------------------------
/**
    Retrieve the global namespace.

    It is recommended to put your namespace inside the global namespace, and
    then add your classes and functions to it, rather than adding many classes
    and functions directly to the global namespace.
*/
inline Namespace getGlobalNamespace(lua_State *L)
{
    return Namespace::GetGlobalNamespace(L);
}

} // namespace luabridge

#endif
