//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "MathEvaluate.h"
#include "MathException.h"
#include "MathParser.h"
#include "../utility/macros.h"

using namespace std;


bool MathEvaluateBool(const SettingsMap& context, const wstring& expression,
    bool& result, unsigned int flags)
{
    try
    {
        const StringSet recursiveVarSet; // dummy set
        MathParser mathParser(context, expression, recursiveVarSet, flags);
        result = mathParser.Evaluate().ToBoolean();
    }
    catch (const MathException& e)
    {
        RESOURCE_STREX(
            GetModuleHandle(NULL), IDS_MATHEXCEPTION,
            resourceTextBuffer, MAX_LINE_LENGTH,
            L"Error in Expression:\n  %ls\n\nDescription:\n  %ls",
            expression.c_str(), e.GetException());
        
        RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);
        
        return false;
    }
    
    return true;
}


bool MathEvaluateString(const SettingsMap& context, const wstring& expression,
    wstring& result, const StringSet& recursiveVarSet, unsigned int flags)
{
    try
    {
        MathParser mathParser(context, expression, recursiveVarSet, flags);
        
        if (MATH_VALUE_TO_COMPATIBLE_STRING & flags)
        {
            result = mathParser.Evaluate().ToCompatibleString();
        }
        else
        {
            result = mathParser.Evaluate().ToString();
        }
    }
    catch (const MathException& e)
    {
        RESOURCE_STREX(
            GetModuleHandle(NULL), IDS_MATHEXCEPTION,
            resourceTextBuffer, MAX_LINE_LENGTH,
            L"Error in Expression:\n  %s\n\nDescription:\n  %s",
            expression.c_str(), e.GetException());
        
        RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);
        
        return false;
    }
    
    return true;
}
