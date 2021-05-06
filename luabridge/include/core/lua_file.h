/******************************************************************************
* Name: LuaBridge for C++
*
* Author: DGuco(杜国超)
* Date: 2019-12-07 17:15
* E-Mail: 1139140929@qq.com
*
* Copyright (C) 2019 DGuco(杜国超).  All rights reserved.
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

#pragma once

#include "lua_helpers.h"
#include "type_traits.h"
#include "type_list.h"
#include "func_traits.h"
#include "constructor.h"
#include "classinfo.h"
#include "lua_exception.h"
#include "lua_ref.h"
#include "user_data.h"
#include "cfunctions.h"
#include "security.h"
#include "stack.h"
#include "namespace.h"