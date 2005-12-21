#include <sstream>
#include <string>
#include "MathToken.h"

using namespace std;


// Mapping of token types to names
struct { int type; const char *name; } gTokenTypes[] =
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

}


MathToken::MathToken(int type) :
    mType(type)
{

}


MathToken::MathToken(int type, const string& value) :
    mType(type), mValue(value)
{

}


string MathToken::GetTypeName() const
{
    for (int i = 0; i < gNumTokenTypes; i++)
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
