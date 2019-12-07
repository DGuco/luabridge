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

#ifndef  __LUA_VARIANG_H__
#define  __LUA_VARIANG_H__

#include "lua_file.h"
#include <string>
#include <cstring>
#include <map>

enum LuaVariantType
{
	LUAVARIANTTYPE_NIL = 0,
	LUAVARIANTTYPE_STR = 1,
	LUAVARIANTTYPE_NUM = 2,
	LUAVARIANTTYPE_BOOL = 3,
	LUAVARIANTTYPE_TABLE = 100
};

std::string TableToString(CLuaVariant* var);

CLuaVariant StringToTable(const char* str);

CLuaVariant StringToVariant(const char* str);

class CLuaVariant;
typedef std::map<CLuaVariant, CLuaVariant> LuaTable;

class CLuaVariant
{
	LuaVariantType type;
	std::string strvalue;
	double numvalue;
	bool boolvalue;
	LuaTable tablevalue;
public:
	CLuaVariant()
	{
		Clear();
	}
	CLuaVariant(bool value)
	{
		Set(value);
	}
	CLuaVariant(int value)
	{
		Set(value);
	}
	CLuaVariant(double value)
	{
		Set(value);
	}
	CLuaVariant(const std::string& value)
	{
		Set(value);
	}
	CLuaVariant(const char* value)
	{
		Set(value);
	}
	CLuaVariant(const LuaTable& value)
	{
		Set(value);
	}

	void Clear()
	{
		type = LUAVARIANTTYPE_NIL;
		numvalue = 0;
		boolvalue = false;
		strvalue = "(null)";
		tablevalue.clear();
	}
	void Set(bool value)
	{
		type = LUAVARIANTTYPE_BOOL;
		numvalue = value;
		boolvalue = value;
		if(value)
			strvalue = "(true)";
		else
			strvalue = "(false)";
		tablevalue.clear();
	}

    inline void Set(int value)
	{
		Set(static_cast<double>(value));
	}

	void Set(double value)
	{
		boolvalue = true;
		numvalue = value;
		type = LUAVARIANTTYPE_NUM;
		strvalue = std::to_string(value);
		tablevalue.clear();
	}

	void Set(const std::string& value)
	{
		boolvalue = true;
		strvalue = value;
		type = LUAVARIANTTYPE_STR;
		char* pos;
		numvalue = strtod(strvalue.c_str(), &pos);
		tablevalue.clear();
	}
	void Set(const char* value)
	{
		boolvalue = true;
		strvalue = value;
		type = LUAVARIANTTYPE_STR;
		char* pos;
		numvalue = strtod(value, &pos);
		tablevalue.clear();
	}
	void Set(const LuaTable& value)
	{
		boolvalue = true;
		tablevalue = value;
		type = LUAVARIANTTYPE_TABLE;
		numvalue = 0;
		strvalue = "(table)";
	}

	LuaVariantType GetType() const
	{
		return type;
	}

	bool GetValueAsBool() const
	{
		return boolvalue;
	}

	const char* GetValueAsCStr() const
	{
		return strvalue.c_str();
	}

	const std::string & GetValueAsStdStr() const
	{
		return strvalue;
	}

	double GetValueAsNum() const
	{
		return numvalue;
	}

	int GetValueAsInt() const
	{
		return static_cast<int>(numvalue);
	}

	const LuaTable& GetValueAsTable() const
	{
		return tablevalue;
	}

	LuaTable GetValueAsTable()
	{
		return tablevalue;
	}

	LuaTable& GetValueAsRefTable()
	{
		return tablevalue;
	}
};

inline bool operator < (const CLuaVariant& l, const CLuaVariant& r)
{
	return l.GetValueAsCStr() < r.GetValueAsCStr();
}

std::string TableToString(CLuaVariant* var)
{
    std::string Result = "";

    char Buff[64];

    if (var != NULL)
    {
        Result.append("{");

        LuaTable& Table = var->GetValueAsRefTable();

        for (std::map<CLuaVariant, CLuaVariant>::iterator it = Table.begin(); it != Table.end(); it++)
        {
            const CLuaVariant* pKey   = &(it->first);
            CLuaVariant*		  pValue = &(it->second);

            switch(pKey->GetType())
            {
                case LUAVARIANTTYPE_NIL:
                    Result.append("nil");
                    break;
                case LUAVARIANTTYPE_STR:
                {
                    Result.append("\"");
                    Result.append(pKey->GetValueAsStdStr());
                    Result.append("\"");
                }
                    break;
                case LUAVARIANTTYPE_NUM:
                {
                    sprintf(Buff, "%.4f", pKey->GetValueAsNum());
                    Result.append(Buff);
                }
                    break;
                case LUAVARIANTTYPE_BOOL:
                {
                    bool BoolValue = pKey->GetValueAsBool();

                    if (BoolValue == true)
                    {
                        Result.append("true");
                    }
                    else
                    {
                        Result.append("false");
                    }
                }
                    break;
                case LUAVARIANTTYPE_TABLE:
                    Result.append(TableToString((CLuaVariant*)pKey));
                    break;
            }

            Result.append("=");

            switch(pValue->GetType())
            {
                case LUAVARIANTTYPE_NIL:
                    Result.append("nil");
                    break;
                case LUAVARIANTTYPE_STR:
                {
                    Result.append("\"");
                    Result.append(pValue->GetValueAsStdStr());
                    Result.append("\"");
                }
                    break;
                case LUAVARIANTTYPE_NUM:
                {
                    sprintf(Buff, "%.4f", pValue->GetValueAsNum());
                    Result.append(Buff);
                }
                    break;
                case LUAVARIANTTYPE_BOOL:
                {
                    bool BoolValue = pValue->GetValueAsBool();

                    if (BoolValue == true)
                    {
                        Result.append("true");
                    }
                    else
                    {
                        Result.append("false");
                    }
                }
                    break;
                case LUAVARIANTTYPE_TABLE:
                    Result.append(TableToString(pValue));
                    break;
            }

            std::map<CLuaVariant, CLuaVariant>::iterator itTail = it;
            itTail++;

            if (itTail != Table.end())
            {
                Result.append(",");
            }
        }

        Result.append("}");
    }

    return Result;
}

CLuaVariant StringToTable(const char* str)
{
    CLuaVariant Result;

    std::string Content = str;
    std::size_t HeadPos = Content.find_first_of("{");
    std::size_t TailPos = Content.find_last_of("}");

    if ((HeadPos != std::string::npos) && (TailPos != std::string::npos) && (TailPos > HeadPos))
    {
        HeadPos++;
        Content = Content.substr(HeadPos, (TailPos-HeadPos));

        std::map<CLuaVariant, CLuaVariant> Table;

        if (!Content.empty())
        {
            std::string Key;
            std::string Value;
            std::string Token;
            int TableDepth = 0;
            bool Quotes = false;
            int Status = 0;

            for (std::string::iterator it = Content.begin(); it != Content.end(); it++)
            {
                char ch = *it;

                switch(ch)
                {
                    case '{':
                    {
                        TableDepth++;
                        Token.push_back(ch);
                    }
                        break;
                    case '}':
                    {
                        TableDepth--;

                        if (TableDepth < 0)
                            TableDepth = 0;

                        Token.push_back(ch);
                    }
                        break;
                    case '=':
                    {
                        if (TableDepth > 0)
                        {
                            Token.push_back(ch);
                        }
                        else
                        {
                            Key = Token;
                            Token.clear();
                            Status = 1;
                        }
                    }
                        break;
                    case ',':
                    {
                        if (TableDepth > 0)
                        {
                            Token.push_back(ch);
                        }
                        else
                        {
                            Value = Token;
                            Token.clear();
                            Status = 2;
                        }
                    }
                        break;
                    case '"':
                    {
                        if (TableDepth > 0)
                        {

                        }
                        else
                        {
                            Quotes = !Quotes;
                        }

                        Token.push_back(ch);
                    }
                        break;
                    case ' ':
                    {
                        if (TableDepth > 0)
                        {
                            Token.push_back(ch);
                        }
                        else
                        {
                            if (Quotes == true)
                            {
                                Token.push_back(ch);
                            }
                        }
                    }
                        break;
                    default:
                    {
                        Token.push_back(ch);
                    }
                }

                if (Status == 2)
                {
                    Status = 0;

                    CLuaVariant KeyVar   = StringToVariant(Key.c_str());
                    CLuaVariant ValueVar = StringToVariant(Value.c_str());

                    Table[KeyVar] = ValueVar;

                    Key.clear();
                    Value.clear();
                }
            }

            if ((Status == 1) && (!Token.empty()))
            {
                CLuaVariant KeyVar   = StringToVariant(Key.c_str());
                CLuaVariant ValueVar = StringToVariant(Token.c_str());

                Table[KeyVar] = ValueVar;
            }
        }

        Result.Set(Table);
    }

    return Result;
}

CLuaVariant StringToVariant(const char* str)
{
    CLuaVariant Result;

    std::string Temp = str;

    std::size_t HeadPos = std::string::npos;
    std::size_t TailPos = std::string::npos;

    if (strcmp(Temp.c_str(), "nil")==0)
    {
        //nil
        Result.Clear();
    }
    else if (strcmp(Temp.c_str(), "true")==0)
    {
        //boolean
        Result.Set(true);
    }
    else if (strcmp(Temp.c_str(), "false")==0)
    {
        //boolean
        Result.Set(false);
    }
    else
    {
        HeadPos = Temp.find_first_of("{");
        TailPos = Temp.find_last_of("}");

        if ((HeadPos != std::string::npos) && (TailPos != std::string::npos) && (TailPos > HeadPos))
        {
            //Table
            Result = StringToTable(Temp.c_str());
        }
        else
        {
            HeadPos = Temp.find_first_of("\"");
            TailPos = Temp.find_last_of("\"");

            if ((HeadPos != std::string::npos) && (TailPos != std::string::npos) && (TailPos > HeadPos))
            {
                //string
                Temp = Temp.substr(HeadPos+1, (TailPos-HeadPos-1));
                Result.Set(Temp);
            }
            else
            {
                //Number
                Result.Set(atof(Temp.c_str()));
            }
        }
    }

    return Result;
}

#endif  //__LUA_VARIANG_H__