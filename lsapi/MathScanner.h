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
#if !defined(MATHSCANNER_H)
#define MATHSCANNER_H

#include "MathToken.h"
#include <sstream>
#include <string>


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
    static bool IsDigit(char ch);
    
    /**
     * Returns true if a character can appear as the first character in an
     * identifier (name).
     */
    static bool IsFirstNameChar(char ch);
    
    /**
     * Returns true if a character can appear in an identifier (name).
     */
    static bool IsNameChar(char ch);
    
    /**
     * Returns true if a character is a space character.
     */
    static bool IsSpace(char ch);
    
private:
    /** Number of characters of lookahead */
    enum { LOOKAHEAD = 2 };
    
    /** Character buffer */
    char mLookahead[LOOKAHEAD];
    
    /** Input stream */
    std::istringstream mStream;
};


#endif // MATHSCANNER_H
