#include <cctype>
#include <sstream>
#include <string>
#include "MathException.h"
#include "MathScanner.h"
#include "MathToken.h"

using namespace std;


// Reserved words
struct { const char *str; int type; } gReservedWords[] =
{
    { "false",    TT_FALSE    },
    { "true",     TT_TRUE     },
    { "infinity", TT_INFINITY },
    { "nan",      TT_NAN      },
    { "defined",  TT_DEFINED  },
    { "div",      TT_DIV      },
    { "mod",      TT_MOD      },
    { "and",      TT_AND      },
    { "or",       TT_OR       },
    { "not",      TT_NOT      }
};

const int gNumReservedWords = sizeof(gReservedWords) / sizeof(gReservedWords[0]);


// Operators and punctuation
// Checked in this order so for example "<=" must precede "<". Must have enough
// lookahead to recognize the longest symbol.
struct { const char *str; int length; int type; } gSymbols[] =
{
    { "(",  1, TT_LPAREN    },
    { ")",  1, TT_RPAREN    },
    { ",",  1, TT_COMMA     },
    { "+",  1, TT_PLUS      },
    { "-",  1, TT_MINUS     },
    { "*",  1, TT_STAR      },
    { "/",  1, TT_SLASH     },
    { "&",  1, TT_AMPERSAND },
    { "=",  1, TT_EQUAL     },
    { ">=", 2, TT_GREATEREQ },
    { ">",  1, TT_GREATER   },
    { "<>", 2, TT_NOTEQUAL  },
    { "<=", 2, TT_LESSEQ    },
    { "<",  1, TT_LESS      },
    { "!=", 2, TT_NOTEQUAL  }
};

const int gNumSymbols = sizeof(gSymbols) / sizeof(gSymbols[0]);


MathScanner::MathScanner(const string& expression) :
    mStream(expression)
{
    // Fill the lookahead buffer
    Next(LOOKAHEAD);
}


MathToken MathScanner::NextToken()
{
    // Skip past whitespace
    SkipSpace();
    
    if (mLookahead[0] < 0)
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
    else if (mLookahead[0] == '\"' || mLookahead[0] == '\'')
    {
        // String literal
        return ScanString();
    }
    
    // Operators and punctuation symbols
    for (int i = 0; i < gNumSymbols; i++)
    {
        bool match = true;
        
        for (int j = 0; j < gSymbols[i].length; j++)
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
    throw MathException("Illegal character");
}


MathToken MathScanner::CheckReservedWord(const string& identifier)
{
    for (int i = 0; i < gNumReservedWords; i++)
    {
        if (stricmp(identifier.c_str(), gReservedWords[i].str) == 0)
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
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < LOOKAHEAD - 1; j++)
            mLookahead[j] = mLookahead[j + 1];
        
        mLookahead[LOOKAHEAD - 1] = mStream.get();
    }
}


MathToken MathScanner::ScanIdentifier()
{
    ostringstream value;
    
    while (IsNameChar(mLookahead[0]))
    {
        value.put(mLookahead[0]);
        Next();
    }
    
    return CheckReservedWord(value.str());
}


MathToken MathScanner::ScanNumber()
{
    ostringstream value;
    
    while (IsDigit(mLookahead[0]))
    {
        value.put(mLookahead[0]);
        Next();
    }
    
    if (mLookahead[0] == '.')
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
    ostringstream value;
    int quote = mLookahead[0];
    Next();
    
    while (mLookahead[0] != EOF && mLookahead[0] != quote)
    {
        if (mLookahead[0] == '\\')
        {
            // Escape sequence
            Next();
            
            switch (mLookahead[0])
            {
            case '\\':
                value.put('\\');
                break;
                
            case '\"':
                value.put('\"');
                break;
                
            case '\'':
                value.put('\'');
                break;
                
            default:
                throw MathException("Illegal string escape sequence");
            }
        }
        else
        {
            // Just a character
            value.put(mLookahead[0]);
        }
        
        Next();
    }
    
    if (mLookahead[0] == EOF)
    {
        throw MathException("Unterminated string literal");
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


bool MathScanner::IsDigit(int ch)
{
    return (ch >= '0' && ch <= '9');
}


bool MathScanner::IsFirstNameChar(int ch)
{
    return !IsDigit(ch) && IsNameChar(ch);
}


bool MathScanner::IsNameChar(int ch)
{
    if (ch < 0 || IsSpace(ch))
    {
        return false;
    }

    switch (ch)
    {
        case '!':
        // case '@':  Will be reserved in 0.25
        // case '#':  Will be reserved in 0.25
        case '$':
        case '&':
        case '*':
        case '(':
        case ')':
        case '-':
        case '+':
        case '=':
        case '[':
        case ']':
        // case '|':  Will be reserved in 0.25
        case ';':
        case '"':
        case '\'':
        case '<':
        case '>':
        case ',':
        case '/':
            return false;
    }

    return true;
}


bool MathScanner::IsSpace(int ch)
{
    return (ch == ' ' || ch == '\t'); // More than this?
}
