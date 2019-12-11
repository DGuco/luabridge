//
// Created by dguco on 19-12-11.
//

#ifndef LUABRIDGE_LUA_VALUE_H
#define LUABRIDGE_LUA_VALUE_H

#include "lua_file.h"
#include <iostream>

using namespace std;

#define int64 long long

bool CheckLuaArg_Num(lua_State *L, int Index);
bool CheckLuaArg_Str(lua_State *L, int Index);
void LuaDebugOutput(lua_State *L);

#define _Lua_Check_ArgNum_(l, index)   { if( CheckLuaArg_Num( l , index ) == false ) return 0; }
#define _Lua_Check_ArgStr_(l, index)   { if( CheckLuaArg_Str( l , index ) == false ) return ""; }

int64 StrToInt64(const char *str);

class CLuaStack;

template<typename R>
struct GetValue
{
    R operator()(lua_State *L, int index)
    {
        throw std::runtime_error("Bad lua type");
    }
};

template<>
struct GetValue<float>
{
    float operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return static_cast<float>(lua_tonumber(L, index));
    }
};

template<>
struct GetValue<double>
{
    double operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return lua_tonumber(L, index);
    }
};

template<>
struct GetValue<long double>
{
    long double operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return lua_tonumber(L, index);
    }
};

template<>
struct GetValue<int>
{
    int operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return static_cast<int>(lua_tonumber(L, index));
    }
};

template<>
struct GetValue<long>
{
    long operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return static_cast<long>(lua_tonumber(L, index));
    }
};

template<>
struct GetValue<int64>
{
    int64 operator()(lua_State *L, int index)
    {
        if (!CheckLuaArg_Str(L, index))
            return 0;

        return StrToInt64(lua_tostring(L, index));
    }
};

template<>
struct GetValue<const char *>
{
    const char *operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgStr_(L, index);
        return lua_tostring(L, index);
    }
};

template<>
struct GetValue<std::string>
{
    std::string operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgStr_(L, index);
        return std::string(lua_tostring(L, index), lua_strlen(L, index));
    }
};

template<>
struct GetValue<bool>
{
    bool operator()(lua_State *L, int index)
    {
        _Lua_Check_ArgNum_(L, index);
        return lua_toboolean(L, index) != 0;
    }
};

//template<typename R>
//struct GetValue<R *>
//{
//    R *operator()(lua_State *L, int index)
//    {
//        R **pobj = static_cast<R **>( luaL_checkudata(L, index, R::GetLuaTypeName()));
//        luaL_argcheck(L, pobj != NULL && *pobj != NULL, index, "Invalid object");
//        L.Pop();
//        return *pobj;
//    }
//};

//template<>
//struct GetValue<LuaTable>
//{
//    LuaTable operator()(lua_State *L, int index)
//    {
//        return L.GetTable(index);
//    }
//};
//
//template<>
//struct GetValue<CLuaVariant>
//{
//    CLuaVariant operator()(CLuaStack &l, int index)
//    {
//        CLuaVariant value;
//        if (lua_istable(l, index))
//            value.Set(l.GetTable(index));
//        else {
//            if (lua_isboolean(l, index))
//                value.Set(l.getValue<bool>(index));
//            else {
//                if (lua_isnumber(l, index))
//                    value.Set(l.getValue<double>(index));
//                else if (lua_isstring(l, index))
//                    value.Set(l.getValue<std::string>(index));
//            }
//        }
//        return value;
//    }
//};


void Empty_SendLuaErrorInfo(const char *, const char *)
{}

void LuaDebugOutput(lua_State *L)
{
    char szMsg[1024];
    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    int debug_StackNum = lua_gettop(L);

    for (int i = 0;; i++) {
        if (lua_getstack(L, i, &trouble_info) == 0)
            break;
        lua_getinfo(L, "Snl", &trouble_info);

        /*sprintf( szMsg, "name:(%s) what:(%s) short:(%s) linedefined:(%d) currentline:(%d)",
            trouble_info.name,
            trouble_info.what,
            trouble_info.short_src,
            trouble_info.linedefined,
            trouble_info.currentline
            );*/
        sprintf(szMsg, "%s(%d): /t%s",
                trouble_info.short_src,
                trouble_info.currentline,
                trouble_info.name
        );

        //g_pfunLuaDebugOut( "",szMsg );
    }

}

bool CheckLuaArg_Num(lua_State *L, int Index)
{
    if (lua_isnumber(L, Index))
        return true;

//	printf("%s\n",lua_tostring(L,-1));

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 0, &trouble_info) || lua_getstack(L, 1, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    char szMsg[1024];

    if (lua_isnil(L, Index)) {
        sprintf(szMsg, "Lua function(%s), %d arg is null", trouble_info.name, Index);
        //g_pfunLuaDebugOut(trouble_info.name, szMsg);
    }
    else if (lua_isstring(L, Index)) {
        sprintf(szMsg,
                "Lua function(%s) %d arg type error('%s') it's not number",
                trouble_info.name,
                Index,
                lua_tostring(L, Index));
        //g_pfunLuaDebugOut(trouble_info.name, szMsg);
    }
    else {
        sprintf(szMsg, "Lua function(%s), %d arg type error", trouble_info.name, Index);
        //g_pfunLuaDebugOut(trouble_info.name, szMsg);
    }


    LuaDebugOutput(L);

    return false;
}

int64 StrToInt64(const char *str)
{
    int64 ret = atoi(str);
    if (ret != 0)
        return ret;
    return  0;
//    return _strtoi64(str, NULL, 16);
//
//    __int64 ret = 0;
//    for( int i = 0 ; str[i] != 0 ; i++ )
//    {
//        ret *= 16;
//        if( str[i] <= '9' )
//            ret += str[i] - '0';
//        else if( str[i] <= 'F' )
//            ret += (str[i] - 'A' + 10);
//        else if( str[i] <= 'f' )
//            ret += (str[i] - 'a' + 10);
//    }
//
//    return ret;

}

bool CheckLuaArg_Str(lua_State *L, int Index)
{
    if (lua_isstring(L, Index))
        return true;

    char szMsg[1024];

    lua_Debug trouble_info;
    memset(&trouble_info, 0, sizeof(lua_Debug));
    int debug_StackNum = lua_gettop(L);
    if (lua_getstack(L, 1, &trouble_info) || lua_getstack(L, 0, &trouble_info))
        lua_getinfo(L, "Snl", &trouble_info);

    if (lua_isnil(L, Index)) {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg dosent exist", trouble_info.name, Index);
        //g_pfunLuaDebugOut(trouble_info.name, szMsg);
    }
    else {
        sprintf(szMsg, "Lua func(%s) arg-[%d] arg type error", trouble_info.name, Index);
        //g_pfunLuaDebugOut(trouble_info.name, szMsg);
    }
/*
    sprintf( szMsg, "name:(%s) namewhat:(%s) what:(%s) source:(%s) short:(%s) linedefined:(%d) currentline:(%d)\n",
        trouble_info.name,
        trouble_info.namewhat,
        trouble_info.what,
        trouble_info.source,
        trouble_info.short_src,
        trouble_info.linedefined,
        trouble_info.currentline
        );

	g_pfunLuaDebugOut( trouble_info.name,szMsg );
*/
    LuaDebugOutput(L);
    return false;
}
#endif //LUABRIDGE_LUA_VALUE_H
