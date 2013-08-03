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
#include "MathValue.h"
#include "../utility/debug.hpp"
#include <cfloat>
#include <cmath>
#include <limits>
#include <sstream>
#include <string.h>

using namespace std;


MathValue::MathValue() :
    mType(UNDEFINED)
{
    // do nothing
}


MathValue::MathValue(bool value) :
    mType(BOOLEAN), mBoolean(value)
{
    // do nothing
}


MathValue::MathValue(int value) :
    mType(NUMBER), mNumber(value)
{
    // do nothing
}


MathValue::MathValue(double value) :
    mType(NUMBER), mNumber(value)
{
    // do nothing
}


MathValue::MathValue(const wstring& value) :
    mType(STRING), mString(value)
{
    // do nothing
}


MathValue::MathValue(const wchar_t *value) :
    mType(STRING), mString(value)
{
    // do nothing
}


MathValue& MathValue::operator=(bool value)
{
    mType = BOOLEAN;
    mBoolean = value;
    
    return *this;
}


MathValue& MathValue::operator=(int value)
{
    mType = NUMBER;
    mNumber = value;
    
    return *this;
}


MathValue& MathValue::operator=(double value)
{
    mType = NUMBER;
    mNumber = value;
    
    return *this;
}


MathValue& MathValue::operator=(const wstring& value)
{
    mType = STRING;
    mString = value;
    
    return *this;
}


MathValue& MathValue::operator=(const wchar_t *value)
{
    mType = STRING;
    mString = value;
    
    return *this;
}


wstring MathValue::GetTypeName() const
{
    switch (mType)
    {
    case UNDEFINED:
        return L"undefined";
        
    case BOOLEAN:
        return L"boolean";
        
    case NUMBER:
        return L"number";
        
    case STRING:
        return L"string";
    }
    
    // Should never happen
    ASSERT(false);
    return wstring();
}


bool MathValue::ToBoolean() const
{
    switch (mType)
    {
    case UNDEFINED:
        return false;
        
    case BOOLEAN:
        return mBoolean;
        
    case NUMBER:
        return (mNumber != 0.0 && !_isnan(mNumber));
        
    case STRING:
        return (!mString.empty() && _wcsicmp(mString.c_str(), L"false") != 0);
    }
    
    // Should never happen
    ASSERT(false);
    return false;
}


int MathValue::ToInteger() const
{
    double number = ToNumber();
    return _finite(number) ? static_cast<int>(floor(number)) : 0;
}


double MathValue::ToNumber() const
{
    switch (mType)
    {
    case UNDEFINED:
        return numeric_limits<double>::quiet_NaN();
        
    case BOOLEAN:
        return (mBoolean ? 1.0 : 0.0);
        
    case NUMBER:
        return mNumber;
        
    case STRING:
        return MathStringToNumber(mString);
    }
    
    // Should never happen
    ASSERT(false);
    return 0.0;
}


wstring MathValue::ToString() const
{
    switch (mType)
    {
    case UNDEFINED:
        return L"undefined";
        
    case BOOLEAN:
        return mBoolean ? L"true" : L"false";
        
    case NUMBER:
        return MathNumberToString(mNumber);
        
    case STRING:
        return mString;
    }
    
    // Should never happen
    ASSERT(false);
    return wstring();
}


wstring MathValue::ToCompatibleString() const
{
    // To keep compatible with 0.24.x math evaluations, we must
    // return an integer formatted string for all number type
    // results.  Thus, convert number value to an Integer prior
    // to returning it as a string.
    if (NUMBER == mType)
    {
        wostringstream stream;
        
        stream << ToInteger();
        
        return stream.str();
    }
    
    // All other values, let the default handler deal with the
    // conversion process.
    return ToString();
}


MathValue operator+(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return (a.ToNumber() + b.ToNumber());
}


MathValue operator+(const MathValue& a)
{
    if (a.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return a.ToNumber();
}


MathValue operator-(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return (a.ToNumber() - b.ToNumber());
}


MathValue operator-(const MathValue& a)
{
    if (a.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return -a.ToNumber();
}


MathValue operator*(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return (a.ToNumber() * b.ToNumber());
}


MathValue operator/(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return (a.ToNumber() / b.ToNumber());
}


MathValue operator%(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    double divisor = b.ToNumber();
    
    if (divisor == 0.0)
    {
        // Modulus by zero generates a NaN
        return numeric_limits<double>::quiet_NaN();
    }
    
    return fmod(a.ToNumber(), divisor);
}


MathValue operator&&(const MathValue& a, const MathValue& b)
{
    // For compatibility reasons, convert undefined values to false for
    // Boolean operators.
    return (a.ToBoolean() && b.ToBoolean());
}


MathValue operator||(const MathValue& a, const MathValue& b)
{
    // For compatibility reasons, convert undefined values to false for
    // Boolean operators.
    return (a.ToBoolean() || b.ToBoolean());
}


MathValue operator!(const MathValue& a)
{
    // For compatibility reasons, convert undefined values to false for
    // Boolean operators.
    return !a.ToBoolean();
}


MathValue operator==(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsBoolean() || b.IsBoolean())
    {
        // If either operand is a Boolean, then do a Boolean comparison
        return (a.ToBoolean() == b.ToBoolean());
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() == b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison.
        return (a.ToNumber() == b.ToNumber());
    }
}


MathValue operator!=(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsBoolean() || b.IsBoolean())
    {
        // If either operand is a Boolean, then do a Boolean comparison
        return (a.ToBoolean() != b.ToBoolean());
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() != b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison.
        return (a.ToNumber() != b.ToNumber());
    }
}


MathValue operator<(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() < b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison
        return (a.ToNumber() < b.ToNumber());
    }
}


MathValue operator<=(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() <= b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison
        return (a.ToNumber() <= b.ToNumber());
    }
}


MathValue operator>(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() > b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison
        return (a.ToNumber() > b.ToNumber());
    }
}


MathValue operator>=(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    else if (a.IsString() && b.IsString())
    {
        // If both operands are strings then do a string comparison
        return (a.ToString() >= b.ToString());
    }
    else
    {
        // In all other cases do a numeric comparison
        return (a.ToNumber() >= b.ToNumber());
    }
}


MathValue MathConcatenate(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    return (a.ToString() + b.ToString());
}


MathValue MathIntDivide(const MathValue& a, const MathValue& b)
{
    if (a.IsUndefined() || b.IsUndefined())
    {
        // Undefined operands always generate an undefined result
        return MathValue();
    }
    
    int divisor = b.ToInteger();
    
    if (divisor == 0)
    {
        // Division by zero results in an Infinity of the appropriate sign
        return _copysign(numeric_limits<double>::infinity(), a.ToNumber());
    }
    
    return (a.ToInteger() / divisor);
}


wstring MathNumberToString(double number)
{
    if (_finite(number))
    {
        // Number
        wostringstream stream;
        
        stream.precision(numeric_limits<double>::digits10 + 1);
        stream << number;
        
        return stream.str();
    }
    else if (number == numeric_limits<double>::infinity())
    {
        // Positive infinity
        return L"Infinity";
    }
    else if (number == -numeric_limits<double>::infinity())
    {
        // Negative infinity
        return L"-Infinity";
    }
    else
    {
        // Not a Number (NaN)
        return L"NaN";
    }
}


double MathStringToNumber(const wstring& str)
{
    wistringstream stream(str);
    double number;
    
    if (stream >> number)
    {
        // Number
        return number;
    }
    else if (_wcsicmp(str.c_str(), L"Infinity") == 0)
    {
        // Positive infinity
        return numeric_limits<double>::infinity();
    }
    else if (_wcsicmp(str.c_str(), L"-Infinity") == 0)
    {
        // Negative infinity
        return -numeric_limits<double>::infinity();
    }
    else
    {
        // Not a Number (NaN)
        return numeric_limits<double>::quiet_NaN();
    }
}
