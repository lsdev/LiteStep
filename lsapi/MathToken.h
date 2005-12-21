#if !defined(MATHTOKEN_H)
#define MATHTOKEN_H

#include <string>


/**
 * Token types for {@link MathToken}.
 */
enum
{
    TT_INVALID,
    TT_ID,
    TT_FALSE,
    TT_TRUE,
    TT_NUMBER,
    TT_INFINITY,
    TT_NAN,
    TT_STRING,
    TT_LPAREN,
    TT_RPAREN,
    TT_DEFINED,
    TT_COMMA,
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_DIV,
    TT_MOD,
    TT_AMPERSAND,
    TT_AND,
    TT_OR,
    TT_NOT,
    TT_EQUAL,
    TT_GREATER,
    TT_GREATEREQ,
    TT_LESS,
    TT_LESSEQ,
    TT_NOTEQUAL,
    TT_END
};


/**
 * Token in a math expression.
 */
class MathToken
{
public:

    /**
     * Constructs a token with type <code>TT_INVALID</code>.
     */
    MathToken();
    
    /**
     * Constructs a token with the specified type.
     */
    MathToken(int type);
    
    /**
     * Constructs a token with the specified type and lexical value.
     */
    MathToken(int type, const std::string& value);
    
    /**
     * Returns the type of this token.
     */
    int GetType() const { return mType; }
    
    /**
     * Returns a string description of this token's type.
     */
    std::string GetTypeName() const;
    
    /**
     * Sets the type of this token.
     */
    void SetType(int type);
    
    /**
     * Returns the lexical value of this token.
     */
    std::string GetValue() const { return mValue; }
    
    /**
     * Sets the lexical value of this token.
     */
    void SetValue(const std::string& value);
    
private:

    /** Token type */
    int mType;
    
    /** Lexical value */
    std::string mValue;
};


#endif
