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
#include "MathToken.h"

using namespace std;


// Mapping of token types to names
struct TokenTypeTable { int type; const wchar_t *name; } gTokenTypes[] = \
{
    { TT_INVALID,   L"INVALID"   },
    { TT_ID,        L"ID"        },
    { TT_FALSE,     L"FALSE"     },
    { TT_TRUE,      L"TRUE"      },
    { TT_NUMBER,    L"NUMBER"    },
    { TT_INFINITY,  L"INFINITY"  },
    { TT_NAN,       L"NAN"       },
    { TT_STRING,    L"STRING"    },
    { TT_LPAREN,    L"LPAREN"    },
    { TT_RPAREN,    L"RPAREN"    },
    { TT_DEFINED,   L"DEFINED"   },
    { TT_COMMA,     L"COMMA"     },
    { TT_PLUS,      L"PLUS"      },
    { TT_MINUS,     L"MINUS"     },
    { TT_STAR,      L"STAR"      },
    { TT_SLASH,     L"SLASH"     },
    { TT_DIV,       L"DIV"       },
    { TT_MOD,       L"MOD"       },
    { TT_AMPERSAND, L"AMPERSAND" },
    { TT_AND,       L"AND"       },
    { TT_OR,        L"OR"        },
    { TT_NOT,       L"NOT"       },
    { TT_EQUAL,     L"EQUAL"     },
    { TT_GREATER,   L"GREATER"   },
    { TT_GREATEREQ, L"GREATEREQ" },
    { TT_LESS,      L"LESS"      },
    { TT_LESSEQ,    L"LESSEQ"    },
    { TT_NOTEQUAL,  L"NOTEQUAL"  },
    { TT_END,       L"END"       }
};

const int gNumTokenTypes = sizeof(gTokenTypes) / sizeof(gTokenTypes[0]);


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
    for (int i = 0; i < gNumTokenTypes; ++i)
    {
        if (gTokenTypes[i].type == mType)
        {
            return wstring(gTokenTypes[i].name);
        }
    }
    
    return wstring();
}


void MathToken::SetType(int type)
{
    mType = type;
}


void MathToken::SetValue(const wstring& value)
{
    mValue = value;
}
