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
struct TokenTypeTable { int type; const char *name; } gTokenTypes[] = \
{
    { TT_INVALID,   "INVALID"   },
    { TT_ID,        "ID"        },
    { TT_FALSE,     "FALSE"     },
    { TT_TRUE,      "TRUE"      },
    { TT_NUMBER,    "NUMBER"    },
    { TT_INFINITY,  "INFINITY"  },
    { TT_NAN,       "NAN"       },
    { TT_STRING,    "STRING"    },
    { TT_LPAREN,    "LPAREN"    },
    { TT_RPAREN,    "RPAREN"    },
    { TT_DEFINED,   "DEFINED"   },
    { TT_COMMA,     "COMMA"     },
    { TT_PLUS,      "PLUS"      },
    { TT_MINUS,     "MINUS"     },
    { TT_STAR,      "STAR"      },
    { TT_SLASH,     "SLASH"     },
    { TT_DIV,       "DIV"       },
    { TT_MOD,       "MOD"       },
    { TT_AMPERSAND, "AMPERSAND" },
    { TT_AND,       "AND"       },
    { TT_OR,        "OR"        },
    { TT_NOT,       "NOT"       },
    { TT_EQUAL,     "EQUAL"     },
    { TT_GREATER,   "GREATER"   },
    { TT_GREATEREQ, "GREATEREQ" },
    { TT_LESS,      "LESS"      },
    { TT_LESSEQ,    "LESSEQ"    },
    { TT_NOTEQUAL,  "NOTEQUAL"  },
    { TT_END,       "END"       }
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


MathToken::MathToken(int type, const string& value) :
    mType(type), mValue(value)
{
    // do nothing
}


string MathToken::GetTypeName() const
{
    for (int i = 0; i < gNumTokenTypes; ++i)
    {
        if (gTokenTypes[i].type == mType)
        {
            return string(gTokenTypes[i].name);
        }
    }
    
    return string();
}


void MathToken::SetType(int type)
{
    mType = type;
}


void MathToken::SetValue(const string& value)
{
    mValue = value;
}
