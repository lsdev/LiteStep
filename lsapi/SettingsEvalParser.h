/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

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

class EvalParser
{
public:

	EvalParser();

	bool evaluate(LPCSTR expr, int *result);

private:

	// input source
    std::string input;

	// scanner state
	int inputPosition;
	int currentChar;
	int lookaheadChar;

	// EvalParser state
	int currentToken;

	// current token value
	bool boolValue;
	int intValue;
    std::string stringValue;

private:

	// RDP functions
	bool basicExpression(int &result);
	bool unaryExpression(int &result);
	bool relationalExpression(int &result);
	bool andExpression(int &result);
	bool orExpression(int &result);
	bool expression(int &result);

	inline static bool isRelationalOperator(int tokenType);

	// token functions
	bool matchToken(int tokenType);
	bool nextToken();

	// character functions
	bool matchChar(int ch);
	bool matchChars(int ch1, int ch2);
	bool nextChar();

	// character class identification functions
	inline static bool isLetter(int ch);
	inline static bool isDigit(int ch);
	inline static bool isWhiteSpace(int ch);
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
	return (ch >= '0' && ch <= '9');
}

inline bool EvalParser::isWhiteSpace(int ch)
{
	return (ch == ' ') || (ch == '\t');
}

BOOL Evaluate(LPCSTR expr, int *result);

#endif
