#if !defined(MATHPARSER_H)
#define MATHPARSER_H

#include <string>
#include <vector>
#include "MathScanner.h"
#include "MathToken.h"
#include "MathValue.h"
#include "SettingsDefines.h"


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


#endif
