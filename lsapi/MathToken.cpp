//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2015  LiteStep Development Team
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
#include "MathToken.h"

using std::wstring;


MathToken::MathToken() :
    mType(TT_INVALID)
{
    // do nothing
}


MathToken::MathToken(int type) :
    mType(type)
{
    // do nothing
}


MathToken::MathToken(int type, const wstring& value) :
    mType(type), mValue(value)
{
    // do nothing
}


wstring MathToken::GetTypeName() const
{
    switch (mType)
    {
    case TT_INVALID: return L"INVALID";
    case TT_ID: return L"ID";
    case TT_FALSE: return L"FALSE";
    case TT_TRUE: return L"TRUE";
    case TT_NUMBER: return L"NUMBER";
    case TT_INFINITY: return L"INFINITY";
    case TT_NAN: return L"NAN";
    case TT_STRING: return L"STRING";
    case TT_LPAREN: return L"LPAREN";
    case TT_RPAREN: return L"RPAREN";
    case TT_DEFINED: return L"DEFINED";
    case TT_COMMA: return L"COMMA";
    case TT_PLUS: return L"PLUS";
    case TT_MINUS: return L"MINUS";
    case TT_STAR: return L"STAR";
    case TT_SLASH: return L"SLASH";
    case TT_DIV: return L"DIV";
    case TT_MOD: return L"MOD";
    case TT_AMPERSAND: return L"AMPERSAND";
    case TT_AND: return L"AND";
    case TT_OR: return L"OR";
    case TT_NOT: return L"NOT";
    case TT_EQUAL: return L"EQUAL";
    case TT_GREATER: return L"GREATER";
    case TT_GREATEREQ: return L"GREATEREQ";
    case TT_LESS: return L"LESS";
    case TT_LESSEQ: return L"LESSEQ";
    case TT_NOTEQUAL: return L"NOTEQUAL";
    case TT_END: return L"END";
    default: return wstring();
    }
}


void MathToken::SetType(int type)
{
    mType = type;
}


void MathToken::SetValue(const wstring& value)
{
    mValue = value;
}
