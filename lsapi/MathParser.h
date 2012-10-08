//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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
#if !defined(MATHPARSER_H)
#define MATHPARSER_H

#include "MathScanner.h"
#include "MathToken.h"
#include "MathValue.h"
#include "SettingsDefines.h"
#include <string>
#include <vector>


/** Vector of {@link MathValue} */
typedef std::vector<MathValue> MathValueList;


/**
 * Parser and evaluator for math expressions.
 */
class MathParser
{
public:
    /**
     * Constructor.
     */
    MathParser(const SettingsMap& context, const std::string& expression,
        const StringSet& recursiveVarSet, unsigned int flags = 0);
    
    /**
     * Parses and evaluates a math expression.
     */
    MathValue Evaluate();
    
protected:
    /**
     * Calls a function with the specified arguments and returns the result.
     */
    MathValue CallFunction(const std::string& name, const MathValueList& argList) const;
    
    /**
     * Returns the value of a variable.
     */
    MathValue GetVariable(const std::string& name) const;
    
private:
    /**
     * Parses and evaluates a primary expression.
     */
    MathValue ParsePrimaryExpression();
    
    /**
     * Parses and evaluates a unary expression.
     */
    MathValue ParseUnaryExpression();
    
    /**
     * Parses and evaluates a multiplicative expression.
     */
    MathValue ParseMultiplicativeExpression();
    
    /**
     * Parses and evaluates an additive expression.
     */
    MathValue ParseAdditiveExpression();
    
    /**
     * Parses and evaluates a concatenation expression.
     */
    MathValue ParseConcatenationExpression();
    
    /**
     * Parses and evaluates a relational expression.
     */
    MathValue ParseRelationalExpression();
    
    /**
     * Parses and evaluates a logical AND expression.
     */
    MathValue ParseLogicalANDExpression();
    
    /**
     * Parses and evaluates a logical OR expression.
     */
    MathValue ParseLogicalORExpression();
    
    /**
     * Parses and evaluates an expression.
     */
    MathValue ParseExpression();
    
    /**
     * Parses and evaluates an expression list.
     */
    void ParseExpressionList(MathValueList& valueList);
    
    /**
     * Consumes the current token if its type is <code>type</code>. Throws an
     * exception if the token does not match.
     */
    void Match(int type);
    
    /**
     * Consumes the next <code>count</code> tokens.
     */
    void Next(int count = 1);
    
private:
    /** Number of tokens of lookahead */
    enum { LOOKAHEAD = 2 };
    
    /** Token buffer */
    MathToken mLookahead[LOOKAHEAD];
    
    /** Variable bindings */
    const SettingsMap& mContext;
    
    /** Lexical analyzer */
    MathScanner mScanner;
    
    /** Set of variables to check for recursive definition */
    const StringSet& mRecursiveVarSet;
    
    /** Flags */
    unsigned int mFlags;
};


#endif // MATHPARSER_H
