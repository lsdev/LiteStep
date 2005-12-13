/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2006 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __EvalParser_H
#define __EvalParser_H

#include "../utility/common.h"
#include "lsapidefines.h"
#include <string>


class SettingsManager;

/**
 * Token types for {@link EvalParser}.
 */
enum
{
    TT_NONE,
    TT_ERROR,
    TT_END,
    TT_ID,
    TT_INTEGER,
    TT_LP,
    TT_RP,
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_EQ,
    TT_NE,
    TT_GT,
    TT_GE,
    TT_LT,
    TT_LE,
    TT_AND,
    TT_OR,
    TT_NOT,
    TT_FALSE,
    TT_TRUE
};


/**
 * Conditional expression parser and evaluator.
 */
class EvalParser
{
public:

    /**
     * Constructor.
     */
    EvalParser();
    
    /**
     * Evaluates an expression.
     *
     * @param   expr    an string containing an expression to evaluate
     * @param   result  a pointer to an integer that will receive the expression's value
     * @return  <code>true</code> if the operation succeeded or <code>false</code> otherwise
     */
    bool evaluate(LPCSTR expr, int *result);
    
private:

    /** Input string */
    std::string input;
    
    /** Current character position in the input string */
    int inputPosition;
    
    /** Current character */
    int currentChar;
    
    /** Next character */
    int lookaheadChar;
    
    /** Type of the current token */
    int currentToken;
    
    /** Boolean value of the current token */
    bool boolValue;
    
    /** Integer value of the current token */
    int intValue;
    
    /** String value of the current token */
    std::string stringValue;
    
private:

    /**
     * Parses and evaluates a basic expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool basicExpression(int &result);
    
    /**
     * Parses and evaluates a unary expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool unaryExpression(int &result);
    
    /**
     * Parses and evaluates a relational expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool relationalExpression(int &result);
    
    /**
     * Parses and evaluates a logical AND expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool andExpression(int &result);
    
    /**
     * Parses and evaluates a logical OR expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool orExpression(int &result);
    
    /**
     * Parses and evaluates an expression.
     *
     * @param   result  an integer that receives the evaluation result
     * @return  <code>true</code> if the operation succeeded
     */
    bool expression(int &result);
    
    /**
     * Returns <code>true</code> if the specified token type is a relational operator.
     */
    inline static bool isRelationalOperator(int tokenType);
    
    /**
     * Advances to the next token if the current token's type matches
     * <code>tokenType</code>.
     *
     * @param  tokenType  token type to match
     * @return <code>true</code> if the token type matched
     */
    bool matchToken(int tokenType);
    
    /**
     * Extracts the next token from the input.
     *
     * @return <code>true</code> if successful or
     *         <code>false</code> if there was a lexical error
     */
    bool nextToken();
    
    /**
     * Advances to the next character if the current character matches <code>ch</code>.
     *
     * @param  ch  character to match
     * @return <code>true</code> if the character matched
     */
    bool matchChar(int ch);
    
    /**
     * If the current character matches <code>ch1</code> and the next character
     * matches <code>ch2</code> then advances two characters.
     *
     * @param  ch1  first character to match
     * @param  ch2  second character to match
     * @return <code>true</code> if both characters matched
     */
    bool matchChars(int ch1, int ch2);
    
    /**
     * Advances to the next character.
     *
     * @return <code>true</code> if successful or
     *         <code>false</code> if at the end of the input.
     */
    bool nextChar();
    
    /**
     * Returns <code>true</code> if <code>ch</code> is an alphabetic character.
     */
    inline static bool isLetter(int ch);
    
    /**
     * Returns <code>true</code> if <code>ch</code> is a digit.
     */
    inline static bool isDigit(int ch);
    
    /**
     * Returns <code>true</code> if <code>ch</code> is a whitespace character.
     */
    inline static bool isWhiteSpace(int ch);
    
    /**
     * Returns <code>true</code> if <code>ch</code> is a symbol.
     */
    inline static bool isSymbol(int ch);
};


inline bool EvalParser::isRelationalOperator(int tokenType)
{
    return (tokenType == TT_EQ)
        || (tokenType == TT_NE)
        || (tokenType == TT_GT)
        || (tokenType == TT_GE)
        || (tokenType == TT_LT)
        || (tokenType == TT_LE);
}


inline bool EvalParser::isLetter(int ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_');
}


inline bool EvalParser::isDigit(int ch)
{
    return ((ch >= '0' && ch <= '9') || (ch == '-'));
}


inline bool EvalParser::isWhiteSpace(int ch)
{
    return (ch == ' ') || (ch == '\t');
}


inline bool EvalParser::isSymbol(int ch)
{
    switch (ch)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '=':
    case '<':
    case '>':
    case '(':
    case ')':
        return true;
    
    default:
        return false;
    }
}


#endif
