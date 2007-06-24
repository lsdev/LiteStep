//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
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
#include "../utility/common.h"
#include "lsapi.h"
#include "SettingsEvalParser.h"


EvalParser::EvalParser()
{
    inputPosition = 0;
    currentChar = 0;
    lookaheadChar = 0;
    currentToken = TT_NONE;
}


// evaluate an expression and pass back the result
bool EvalParser::evaluate(LPCSTR expr, int *result)
{
    input = expr;
    inputPosition = 0;
    
    nextChar();
    nextToken();
    
    int value;
    
    if (!expression(value))
    {
        return false;
    }
    
    if (result)
    {
        *result = value;
    }
    
    return true;
}


// basic-expression:
//      identifier
//      integer
//      ( expression )

bool EvalParser::basicExpression(int &result)
{
    char szValue[MAX_LINE_LENGTH + 1];
    
    if (currentToken == TT_ID)
    {
        if (GetRCString(stringValue.c_str(), szValue, NULL, MAX_LINE_LENGTH))
        {
            char szToken[MAX_LINE_LENGTH + 1];
            
            if (isDigit(szValue[0]))
            {
                result = 0;
                
                if (GetToken(szValue, szToken, NULL, FALSE))
                {
                    result = strtol(szToken, NULL, 0);
                }
            }
            else
            {
                result = TRUE;
                
                if (GetToken(szValue, szToken, NULL, FALSE))
                {
                    if ((stricmp(szToken, "off") == 0) ||
                        (stricmp(szToken, "false") == 0) ||
                        (stricmp(szToken, "no") == 0))
                    {
                        result = FALSE;
                    }
                }
            }
        }
        else
        {
            // if the command is nonexistant, it's false
            result = 0;
        }
        
        nextToken();
        return true;
    }
    else if (currentToken == TT_INTEGER)
    {
        result = intValue;
        
        nextToken();
        return true;
    }
    else if (matchToken(TT_LP))
    {
        if (!expression(result))
        {
            return false;
        }
        
        return matchToken(TT_RP);
    }
    else if (matchToken(TT_FALSE))
    {
        result = 0;
        return true;
    }
    else if (matchToken(TT_TRUE))
    {
        result = 1;
        return true;
    }
    
    return false;
}


// unary-expression
//      "not" unary-expression
//      - unary-expression
//      basic-expression

bool EvalParser::unaryExpression(int &result)
{
    if (matchToken(TT_NOT))
    {
        if (!basicExpression(result))
        {
            return false;
        }
        
        result = !result;
        return true;
    }
    else if (matchToken(TT_MINUS))
    {
        if (!basicExpression(result))
        {
            return false;
        }
        
        result = -result;
        return true;
    }
    
    return basicExpression(result);
}


// relational-expression:
//      unary-expression relational-operator unary-expression
//      unary-expression

bool EvalParser::relationalExpression(int &result)
{
    int subresult;
    int token;
    
    if (!unaryExpression(result))
    {
        return false;
    }
    
    if (!isRelationalOperator(currentToken))
    {
        return true;
    }
    
    token = currentToken;
    nextToken();
    
    if (!unaryExpression(subresult))
    {
        return false;
    }
    
    switch (token)
    {
    case TT_EQ:
        result = (result == subresult);
        break;
        
    case TT_NE:
        result = (result != subresult);
        break;
        
    case TT_GT:
        result = (result > subresult);
        break;
        
    case TT_GE:
        result = (result >= subresult);
        break;
        
    case TT_LT:
        result = (result < subresult);
        break;
        
    case TT_LE:
        result = (result <= subresult);
        break;
    }
    
    return true;
}


// and-expression
//      and-expression "and" relational-expression
//      relational-expression

bool EvalParser::andExpression(int &result)
{
    if (!relationalExpression(result))
    {
        return false;
    }
    
    while (matchToken(TT_AND))
    {
        int subresult;
        
        if (!relationalExpression(subresult))
        {
            return false;
        }
        
        result = result && subresult;
    }
    
    return true;
}


// or-expression:
//      or-expression "or" and-expression
//      and-expression

bool EvalParser::orExpression(int &result)
{
    if (!andExpression(result))
    {
        return false;
    }
    
    while (matchToken(TT_OR))
    {
        int subresult;
        
        if (!andExpression(subresult))
        {
            return false;
        }
        
        result = result || subresult;
    }
    
    return true;
}


// expression:
//      or-expression

bool EvalParser::expression(int &result)
{
    return orExpression(result);
}


// if current token is of type tokenType, consume it
bool EvalParser::matchToken(int tokenType)
{
    if (currentToken == tokenType)
    {
        nextToken();
        return true;
    }
    
    return false;
}


// scan next token
bool EvalParser::nextToken()
{
    // skip whitespace
    while (isWhiteSpace(currentChar))
    {
        nextChar();
    }
    
    // are we at the end of the input?
    if (currentChar == 0)
    {
        return currentToken = TT_END, true;
    }
    
    // predict token type based on first character
    if (isLetter(currentChar))
    {
        // it's an identifier or keyword
        stringValue.assign(1, (CHAR) currentChar);
        nextChar();
        
        while ((isLetter(currentChar) || isDigit(currentChar))
            && !isSymbol(currentChar))
        {
            stringValue.append(1, (CHAR) currentChar);
            nextChar();
        }
        
        currentToken = TT_ID;
        
        // is it a keyword?
        if (!stricmp(stringValue.c_str(), "and"))
        {
            currentToken = TT_AND, true;
        }
        else if (!stricmp(stringValue.c_str(), "or"))
        {
            currentToken = TT_OR, true;
        }
        else if (!stricmp(stringValue.c_str(), "not"))
        {
            currentToken = TT_NOT, true;
        }
        else if (!stricmp(stringValue.c_str(), "false"))
        {
            currentToken = TT_FALSE, true;
        }
        else if (!stricmp(stringValue.c_str(), "true"))
        {
            currentToken = TT_TRUE, true;
        }
        
        return currentToken != FALSE;
    }
    else if (isDigit(currentChar))
    {
        // it's a numeric literal
        stringValue.assign(1, (CHAR) currentChar);
        nextChar();
        
        while (isDigit(currentChar))
        {
            stringValue.append(1, (CHAR) currentChar);
            nextChar();
        }
        
        intValue = atoi(stringValue.c_str());
        return currentToken = TT_INTEGER, true;
    }
    else
    {
        // assume it's a symbol
        if (matchChar('='))
        {
            return currentToken = TT_EQ, true;
        }
        else if (matchChars('<', '>'))
        {
            return currentToken = TT_NE, true;
        }
        else if (matchChars('<', '='))
        {
            return currentToken = TT_LE, true;
        }
        else if (matchChar('<'))
        {
            return currentToken = TT_LT, true;
        }
        else if (matchChars('>', '='))
        {
            return currentToken = TT_GE, true;
        }
        else if (matchChar('>'))
        {
            return currentToken = TT_GT, true;
        }
        else if (matchChar('+'))
        {
            return currentToken = TT_PLUS, true;
        }
        else if (matchChar('-'))
        {
            return currentToken = TT_MINUS, true;
        }
        else if (matchChar('*'))
        {
            return currentToken = TT_STAR, true;
        }
        else if (matchChar('/'))
        {
            return currentToken = TT_SLASH, true;
        }
        else if (matchChar('('))
        {
            return currentToken = TT_LP, true;
        }
        else if (matchChar(')'))
        {
            return currentToken = TT_RP, true;
        }
    }
    
    // if we got this far, it's an error
    return currentToken = TT_ERROR, false;
}


// if the next char is ch, consume it
bool EvalParser::matchChar(int ch)
{
    if (currentChar == ch)
    {
        nextChar();
        return true;
    }
    
    return false;
}


// if the next two chars are ch1 and ch2, consume both
bool EvalParser::matchChars(int ch1, int ch2)
{
    if (currentChar == ch1 && lookaheadChar == ch2)
    {
        nextChar();
        nextChar();
        return true;
    }
    
    return false;
}


// advance to next input char
bool EvalParser::nextChar()
{
    // first time?
    if (inputPosition == 0)
    {
        lookaheadChar = input.c_str()[inputPosition++];
    }
    
    // read in char
    currentChar = lookaheadChar;
    
    if (lookaheadChar != 0)
    {
        lookaheadChar = input.c_str()[inputPosition++];
    }
    
    return true;
}
