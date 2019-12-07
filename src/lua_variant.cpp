#include "lua_variant.h"

std::string TableToString(lua_variant* var)
{
	std::string Result = "";

	char Buff[64];

	if (var != NULL)
	{
		Result.append("{");

		LuaTable& Table = var->GetValueAsRefTable();

		for (std::map<lua_variant, lua_variant>::iterator it = Table.begin(); it != Table.end(); it++)
		{
			const lua_variant* pKey   = &(it->first);
			lua_variant*		  pValue = &(it->second);

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
				Result.append(TableToString((lua_variant*)pKey));
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

			std::map<lua_variant, lua_variant>::iterator itTail = it;
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

lua_variant StringToTable(const char* str)
{
	lua_variant Result;

	std::string Content = str;
	std::size_t HeadPos = Content.find_first_of("{");
	std::size_t TailPos = Content.find_last_of("}");

	if ((HeadPos != std::string::npos) && (TailPos != std::string::npos) && (TailPos > HeadPos))
	{
		HeadPos++;
		Content = Content.substr(HeadPos, (TailPos-HeadPos));

		std::map<lua_variant, lua_variant> Table;

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

					lua_variant KeyVar   = StringToVariant(Key.c_str());
					lua_variant ValueVar = StringToVariant(Value.c_str());

					Table[KeyVar] = ValueVar;

					Key.clear();
					Value.clear();
				}
			}

			if ((Status == 1) && (!Token.empty()))
			{
				lua_variant KeyVar   = StringToVariant(Key.c_str());
				lua_variant ValueVar = StringToVariant(Token.c_str());

				Table[KeyVar] = ValueVar;
			}
		}

		Result.Set(Table);
	}

	return Result;
}

lua_variant StringToVariant(const char* str)
{
	lua_variant Result;

	std::string Temp = str;

	std::size_t HeadPos = std::string::npos;
	std::size_t TailPos = std::string::npos;

	if (_stricmp(Temp.c_str(), "nil")==0)
	{
		//nil
		Result.Clear();
	}
	else if (_stricmp(Temp.c_str(), "true")==0)
	{
		//boolean
		Result.Set(true);
	}
	else if (_stricmp(Temp.c_str(), "false")==0)
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