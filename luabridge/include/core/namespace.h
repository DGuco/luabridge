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

#include "lua_class.h"

using namespace std;

namespace luabridge
{

/** Provides C++ to Lua registration capabilities.

    This class is not instantiated directly, call `getGlobalNamespace` to start
    the registration process.
*/
class Namespace
{
private:
    //----------------------------------------------------------------------------
    /**
        Open the global namespace for registrations.
    */
    explicit Namespace(LuaVm *luaVm)
        : m_pLuaVm(luaVm)
    {
        lua_State * L = m_pLuaVm->LuaState();
        lua_getglobal(L, "_G");
        m_pLuaVm->AddStackSize(1);
    }

    //----------------------------------------------------------------------------
    /**
        Open a namespace for registrations.

        The namespace is created if it doesn't already exist.
        The parent namespace is at the top of the Lua stack.
    */
    Namespace(char const *name, LuaVm *luaVm)
        : m_pLuaVm(luaVm)
    {
        lua_State * L = m_pLuaVm->LuaState();

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
            lua_rawsetp(L, -2, GetPropgetKey()); // ns [propgetKey] = pg. Stack: pns, ns

            lua_newtable(L); // Stack: pns, ns, propset table (ps)
            lua_rawsetp(L, -2, GetPropsetKey()); // ns [propsetKey] = ps. Stack: pns, ns

            // pns [name] = ns
            lua_pushvalue(L, -1); // Stack: pns, ns, ns
            LuaHelper::RawSetField(L, -3, name); // Stack: pns, ns
#if 0
            lua_pushcfunction (L, &tostringMetaMethod);
            rawsetfield (L, -2, "__tostring");
#endif
        }
        m_pLuaVm->AddStackSize(1);
    }

public:
    //----------------------------------------------------------------------------
    /**
        Open the global namespace.
    */
    static Namespace GetGlobalNamespace(LuaVm *luaVm)
    {
        return Namespace(luaVm);
    }

    //----------------------------------------------------------------------------
    /**
        Open a new or existing namespace for registrations.
    */
    Namespace BeginNamespace(char const *name,LuaVm *luaVm)
    {
        m_pLuaVm->AssertIsActive();
        return Namespace(name, m_pLuaVm);
    }

    //----------------------------------------------------------------------------
    /**
        Continue namespace registration in the parent.

        Do not use this on the global namespace.
    */
    Namespace endNamespace()
    {
        if (m_pLuaVm->GetStackSize()  == 1) {
            throw std::logic_error("endNamespace () called on global namespace");
        }

        assert (m_pLuaVm->GetStackSize()  > 1);
        m_pLuaVm->AddStackSize(-1);
        lua_State *L = m_pLuaVm->LuaState();
        lua_pop(L, 1);
        return Namespace(m_pLuaVm);
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

        if (m_pLuaVm->GetStackSize()  == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, pt); // Stack: ns, pointer
        lua_pushcclosure(L, &CFunc::getVariable<T>, 1); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2); // Stack: ns

        if (isWritable) {
            lua_pushlightuserdata(L, pt); // Stack: ns, pointer
            lua_pushcclosure(L, &CFunc::setVariable<T>, 1); // Stack: ns, setter
        }
        else {
            lua_pushstring(L, name); // Stack: ns, ps, name
            lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: ns, error_fn
        }
        CFunc::AddSetter(L, name, -2); // Stack: ns

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
        if (m_pLuaVm->GetStackSize()  == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)

        lua_pushlightuserdata(L, reinterpret_cast <void *> (get)); // Stack: ns, function ptr
        lua_pushcclosure(L, &CFunc::Call<TG (*)()>::f, 1); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2);

        if (set != 0) {
            lua_pushlightuserdata(L, reinterpret_cast <void *> (set)); // Stack: ns, function ptr
            lua_pushcclosure(L, &CFunc::Call<void (*)(TS)>::f, 1);
        }
        else {
            lua_pushstring(L, name);
            lua_pushcclosure(L, &CFunc::readOnlyError, 1);
        }
        CFunc::AddSetter(L, name, -2);

        return *this;
    }

    //----------------------------------------------------------------------------
    /**
        Add or replace a property.
        If the set function is omitted or null, the property is read-only.
    */
    Namespace &addProperty(char const *name, int (*get)(lua_State *), int (*set)(lua_State *) = 0)
    {
        if (m_pLuaVm->GetStackSize() == 1) {
            throw std::logic_error("addProperty () called on global namespace");
        }
        lua_State *L = m_pLuaVm->LuaState();

        assert (lua_istable(L, -1)); // Stack: namespace table (ns)
        lua_pushcfunction(L, get); // Stack: ns, getter
        CFunc::AddGetter(L, name, -2); // Stack: ns
        if (set != 0) {
            lua_pushcfunction(L, set); // Stack: ns, setter
            CFunc::AddSetter(L, name, -2); // Stack: ns
        }
        else {
            lua_pushstring(L, name); // Stack: ns, name
            lua_pushcclosure(L, &CFunc::readOnlyError, 1); // Stack: ns, name, readOnlyError
            CFunc::AddSetter(L, name, -2); // Stack: ns
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
        lua_State *L = m_pLuaVm->LuaState();

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
        lua_State *L = m_pLuaVm->LuaState();

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
        m_pLuaVm->AssertIsActive();
        return Class<T>(name, m_pLuaVm);
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
        m_pLuaVm->AssertIsActive();
        return Class<Derived>(name, m_pLuaVm, ClassInfo<Base>::GetStaticKey());
    }
private:
    LuaVm *m_pLuaVm;
};

//------------------------------------------------------------------------------
/**
    Retrieve the global namespace.

    It is recommended to put your namespace inside the global namespace, and
    then add your classes and functions to it, rather than adding many classes
    and functions directly to the global namespace.
*/
inline Namespace getGlobalNamespace(LuaVm *L)
{
    return Namespace::GetGlobalNamespace(L);
}

} // namespace luabridge

#endif
