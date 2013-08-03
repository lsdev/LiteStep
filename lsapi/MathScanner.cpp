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
#include "MathScanner.h"
#include "MathException.h"
#include <string.h> // needed for _stricmp
#include <stdio.h> // needed for EOF

using namespace std;


// Reserved words
struct ReservedWordTable { const wchar_t *str; int type; } gReservedWords[] = \
{
    { L"false",    TT_FALSE    },
    { L"true",     TT_TRUE     },
    { L"infinity", TT_INFINITY },
    { L"nan",      TT_NAN      },
    { L"defined",  TT_DEFINED  },
    { L"div",      TT_DIV      },
    { L"mod",      TT_MOD      },
    { L"and",      TT_AND      },
    { L"or",       TT_OR       },
    { L"not",      TT_NOT      }
};

const int gNumReservedWords = sizeof(gReservedWords) / sizeof(gReservedWords[0]);


// Operators and punctuation
// Checked in this order so for example "<=" must precede "<". Must have enough
// lookahead to recognize the longest symbol.
struct SymbolTable { const wchar_t *str; int length; int type; } gSymbols[] = \
{
    { L"(",  1, TT_LPAREN    },
    { L")",  1, TT_RPAREN    },
    { L",",  1, TT_COMMA     },
    { L"+",  1, TT_PLUS      },
    { L"-",  1, TT_MINUS     },
    { L"*",  1, TT_STAR      },
    { L"/",  1, TT_SLASH     },
    { L"&",  1, TT_AMPERSAND },
    { L"=",  1, TT_EQUAL     },
    { L">=", 2, TT_GREATEREQ },
    { L">",  1, TT_GREATER   },
    { L"<>", 2, TT_NOTEQUAL  },
    { L"<=", 2, TT_LESSEQ    },
    { L"<",  1, TT_LESS      },
    { L"!=", 2, TT_NOTEQUAL  }
};

const int gNumSymbols = sizeof(gSymbols) / sizeof(gSymbols[0]);


MathScanner::MathScanner(const wstring& expression) :
    mStream(expression)
{
    // Fill the lookahead buffer
    Next(LOOKAHEAD);
}


MathToken MathScanner::NextToken()
{
    // Skip past whitespace
    SkipSpace();
    
    if (mLookahead[0] == WEOF)
    {
        // End of input
        return MathToken(TT_END);
    }
    else if (IsFirstNameChar(mLookahead[0]))
    {
        // Identifier or reserved word
        return ScanIdentifier();
    }
    else if (IsDigit(mLookahead[0]))
    {
        // Numeric literal
        return ScanNumber();
    }
    else if (mLookahead[0] == L'\"' || mLookahead[0] == L'\'')
    {
        // String literal
        return ScanString();
    }
    
    // Operators and punctuation symbols
    for (int i = 0; i < gNumSymbols; ++i)
    {
        bool match = true;
        
        for (int j = 0; j < gSymbols[i].length; ++j)
        {
            if (mLookahead[j] != gSymbols[i].str[j])
            {
                match = false;
                break;
            }
        }
        
        if (match)
        {
            Next(gSymbols[i].length);
            return MathToken(gSymbols[i].type);
        }
    }
    
    // Error
    throw MathException(L"Illegal character");
}


MathToken MathScanner::CheckReservedWord(const wstring& identifier)
{
    for (int i = 0; i < gNumReservedWords; ++i)
    {
        if (_wcsicmp(identifier.c_str(), gReservedWords[i].str) == 0)
        {
            // It's a reserved word
            return MathToken(gReservedWords[i].type);
        }
    }
    
    // It's just an identifier
    return MathToken(TT_ID, identifier);
}


void MathScanner::Next(int count)
{
    for (int i = 0; i < count; ++i)
    {
        for (int j = 0; j < LOOKAHEAD - 1; ++j)
        {
            mLookahead[j] = mLookahead[j + 1];
        }
        
        if (!mStream.get(mLookahead[LOOKAHEAD - 1]))
        {
            mLookahead[LOOKAHEAD - 1] = WEOF;
        }
    }
}


MathToken MathScanner::ScanIdentifier()
{
    wostringstream value;
    
    while (IsNameChar(mLookahead[0]))
    {
        value.put(mLookahead[0]);
        Next();
    }
    
    return CheckReservedWord(value.str());
}


MathToken MathScanner::ScanNumber()
{
    wostringstream value;
    
    while (IsDigit(mLookahead[0]))
    {
        value.put(mLookahead[0]);
        Next();
    }
    
    if (mLookahead[0] == L'.')
    {
        value.put(mLookahead[0]);
        Next();
        
        while (IsDigit(mLookahead[0]))
        {
            value.put(mLookahead[0]);
            Next();
        }
    }
    
    return MathToken(TT_NUMBER, value.str());
}


MathToken MathScanner::ScanString()
{
    wostringstream value;
    wchar_t quote = mLookahead[0];
    Next();
    
    while (mLookahead[0] != WEOF && mLookahead[0] != quote)
    {
        if (mLookahead[0] == L'\\')
        {
            // Escape sequence
            Next();
            
            switch (mLookahead[0])
            {
            case L'\\':
                value.put(L'\\');
                break;
                
            case L'\"':
                value.put(L'\"');
                break;
                
            case L'\'':
                value.put(L'\'');
                break;
                
            default:
                throw MathException(L"Illegal string escape sequence");
            }
        }
        else
        {
            // Just a character
            value.put(mLookahead[0]);
        }
        
        Next();
    }
    
    if (mLookahead[0] == WEOF)
    {
        throw MathException(L"Unterminated string literal");
    }
    
    Next();
    return MathToken(TT_STRING, value.str());
}


void MathScanner::SkipSpace()
{
    while (IsSpace(mLookahead[0]))
    {
        Next();
    }
}


bool MathScanner::IsDigit(wchar_t ch)
{
    return (ch >= L'0' && ch <= L'9');
}


bool MathScanner::IsFirstNameChar(wchar_t ch)
{
    return !IsDigit(ch) && IsNameChar(ch);
}


bool MathScanner::IsNameChar(wchar_t ch)
{
    if (ch == WEOF || IsSpace(ch))
    {
        return false;
    }
    
    switch (ch)
    {
    case L'!':
    // case '@':  Will be reserved in 0.25
    // case '#':  Will be reserved in 0.25
    case L'$':
    case L'&':
    case L'*':
    case L'(':
    case L')':
    case L'-':
    case L'+':
    case L'=':
    case L'[':
    case L']':
    // case '|':  Will be reserved in 0.25
    case L';':
    case L'"':
    case L'\'':
    case L'<':
    case L'>':
    case L',':
    case L'/':
        return false;
    }
    
    return true;
}


bool MathScanner::IsSpace(wchar_t ch)
{
    return (ch == L' ' || ch == L'\t'); // More than this?
}
