#if !defined(MATHSCANNER_H)
#define MATHSCANNER_H

#include <sstream>
#include <string>
#include "MathToken.h"


/**
 * Lexical analyzer for math expressions.
 */
class MathScanner
{
public:

    /**
     * Constructs a MathScanner that reads from the specified string.
     */
    MathScanner(const std::string& expression);
    
    /**
     * Extracts the next token from the input and returns it.
     */
    MathToken NextToken();
    
private:

    /**
     * Returns a token for the specified identifier, first checking to see if
     * its a reserved word.
     */
    MathToken CheckReservedWord(const std::string& identifier);
    
    /**
     * Read the next <code>count</code> characters from the input.
     */
    void Next(int count = 1);
    
    /**
     * Scans an identifier.
     */
    MathToken ScanIdentifier();
    
    /**
     * Scans a numeric literal.
     */
    MathToken ScanNumber();
    
    /**
     * Scans a string literal.
     */
    MathToken ScanString();

private:
    
    /**
     * Skips past white space in the input.
     */
    void SkipSpace();

    /**
     * Returns true if a character is a digit.
     */
    static bool IsDigit(int ch);
    
    /**
     * Returns true if a character can appear as the first character in an
     * identifier (name).
     */
    static bool IsFirstNameChar(int ch);
    
    /**
     * Returns true if a character can appear in an identifier (name).
     */
    static bool IsNameChar(int ch);
    
    /**
     * Returns true if a character is a space character.
     */
    static bool IsSpace(int ch);
    
private:

    /** Number of characters of lookahead */
    enum { LOOKAHEAD = 2 };
    
    /** Character buffer */
    int mLookahead[LOOKAHEAD];
    
    /** Input stream */
    std::istringstream mStream;
};


#endif
