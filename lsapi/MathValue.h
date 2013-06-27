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
#if !defined(MATHVALUE_H)
#define MATHVALUE_H

#include <string>


/**
 * Result of evaluating a math expression.
 */
class MathValue
{
public:
    /**
     * Types
     */
    enum
    {
        UNDEFINED,
        BOOLEAN,
        NUMBER,
        STRING
    };
    
public:
    /**
     * Constructs an undefined value.
     */
    MathValue();
    
    /**
     * Constructs a Boolean value.
     */
    MathValue(bool value);
    
    /**
     * Constructs a numeric value from an integer.
     */
    MathValue(int value);
    
    /**
     * Constructs a numeric value from a floating point number.
     */
    MathValue(double value);
    
    /**
     * Constructs a string value.
     */
    MathValue(const std::string& value);
    
    /**
     * Constructs a string value.
     */
    MathValue(const char *value);
    
    /**
     * Assigns a Boolean to this value.
     */
    MathValue& operator=(bool value);
    
    /**
     * Assigns an integer to this value.
     */
    MathValue& operator=(int value);
    
    /**
     * Assigns a floating point number to this value.
     */
    MathValue& operator=(double value);
    
    /**
     * Assigns a string to this value.
     */
    MathValue& operator=(const std::string& value);
    
    /**
     * Assigns a string to this value.
     */
    MathValue& operator=(const char *value);
    
    /**
     * Returns a string description of this value's type.
     */
    std::string GetTypeName() const;
    
    /**
     * Returns <code>true</code> if this value is undefined.
     */
    bool IsUndefined() const
    {
        return (mType == UNDEFINED);
    }
    
    /**
     * Returns <code>true</code> if this value is a Boolean.
     */
    bool IsBoolean() const
    {
        return (mType == BOOLEAN);
    }
    
    /**
     * Returns <code>true</code> if this value is a number.
     */
    bool IsNumber() const
    {
        return (mType == NUMBER);
    }
    
    /**
     * Returns <code>true</code> if this value is a string.
     */
    bool IsString() const
    {
        return (mType == STRING);
    }
    
    /**
     * Converts this value to a Boolean.
     */
    bool ToBoolean() const;
    
    /**
     * Converts this value to an integer.
     */
    int ToInteger() const;
    
    /**
     * Converts this value to a number.
     */
    double ToNumber() const;
    
    /**
     * Converts this value to a string.
     */
    std::string ToString() const;
    
    /**
     * Converts this value to a string using integer representation
     * for any NUMBER type.
     */
    std::string ToCompatibleString() const;
    
    /** Operators */
    friend MathValue operator+ (const MathValue& a, const MathValue& b);
    friend MathValue operator+ (const MathValue& a);
    friend MathValue operator- (const MathValue& a, const MathValue& b);
    friend MathValue operator- (const MathValue& a);
    friend MathValue operator* (const MathValue& a, const MathValue& b);
    friend MathValue operator/ (const MathValue& a, const MathValue& b);
    friend MathValue operator% (const MathValue& a, const MathValue& b);
    friend MathValue operator&&(const MathValue& a, const MathValue& b);
    friend MathValue operator||(const MathValue& a, const MathValue& b);
    friend MathValue operator! (const MathValue& a);
    friend MathValue operator==(const MathValue& a, const MathValue& b);
    friend MathValue operator!=(const MathValue& a, const MathValue& b);
    friend MathValue operator< (const MathValue& a, const MathValue& b);
    friend MathValue operator<=(const MathValue& a, const MathValue& b);
    friend MathValue operator> (const MathValue& a, const MathValue& b);
    friend MathValue operator>=(const MathValue& a, const MathValue& b);
    
private:
    /** Type */
    int mType;
    
    /** Boolean value */
    bool mBoolean;
    
    /** Numeric value */
    double mNumber;
    
    /** String value */
    std::string mString;
};


/** Convert values to strings and concatenate them. */
MathValue MathConcatenate(const MathValue& a, const MathValue& b);

/** Convert values to integers and divide them. */
MathValue MathIntDivide(const MathValue& a, const MathValue& b);

/** Convert a number to a string. */
std::string MathNumberToString(double number);

/** Convert a string to a number. */
double MathStringToNumber(const std::string& str);


#endif // MATHVALUE_H
