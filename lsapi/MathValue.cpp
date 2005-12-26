#include <cassert>
#include <cfloat>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include "MathValue.h"

using namespace std;


MathValue::MathValue() :
    mType(UNDEFINED)
{

}


MathValue::MathValue(bool value) :
    mType(BOOLEAN), mBoolean(value)
{

}


MathValue::MathValue(int value) :
    mType(NUMBER), mNumber(value)
{

}


MathValue::MathValue(double value) :
    mType(NUMBER), mNumber(value)
{

}


MathValue::MathValue(const string& value) :
    mType(STRING), mString(value)
{

}


MathValue::MathValue(const char *value) :
    mType(STRING), mString(value)
{

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


MathValue& MathValue::operator=(const string& value)
{
    mType = STRING;
    mString = value;
    
    return *this;
}


MathValue& MathValue::operator=(const char *value)
{
    mType = STRING;
    mString = value;
    
    return *this;
}


string MathValue::GetTypeName() const
{
    switch (mType)
    {
    case UNDEFINED:
        return "undefined";
    
    case BOOLEAN:
        return "boolean";
    
    case NUMBER:
        return "number";
    
    case STRING:
        return "string";
    }
    
    return string();
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
        return (!mString.empty() && stricmp(mString.c_str(), "false") != 0);
    }
    
    // Should never happen
    assert(false);
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
    assert(false);
    return 0.0;
}


string MathValue::ToString() const
{
    switch (mType)
    {
    case UNDEFINED:
        return "undefined";
        
    case BOOLEAN:
        return mBoolean ? "true" : "false";
        
    case NUMBER:
        return MathNumberToString(mNumber);
        
    case STRING:
        return mString;
    }
    
    // Should never happen
    assert(false);
    return string();
}


MathValue operator+(const MathValue& a, const MathValue& b)
{
    return (a.ToNumber() + b.ToNumber());
}


MathValue operator+(const MathValue& a)
{
    return a.ToNumber();
}


MathValue operator-(const MathValue& a, const MathValue& b)
{
    return (a.ToNumber() - b.ToNumber());
}


MathValue operator-(const MathValue& a)
{
    return -a.ToNumber();
}


MathValue operator*(const MathValue& a, const MathValue& b)
{
    return (a.ToNumber() * b.ToNumber());
}


MathValue operator/(const MathValue& a, const MathValue& b)
{
    return (a.ToNumber() / b.ToNumber());
}


MathValue operator%(const MathValue& a, const MathValue& b)
{
    double divisor = b.ToNumber();
    
    if (divisor != 0.0)
        return fmod(a.ToNumber(), divisor);
    
    return numeric_limits<double>::quiet_NaN();
}


MathValue operator&&(const MathValue& a, const MathValue& b)
{
    return (a.ToBoolean() && b.ToBoolean());
}


MathValue operator||(const MathValue& a, const MathValue& b)
{
    return (a.ToBoolean() || b.ToBoolean());
}


MathValue operator!(const MathValue& a)
{
    return !a.ToBoolean();
}


MathValue operator==(const MathValue& a, const MathValue& b)
{
    if (a.IsBoolean() || b.IsBoolean())
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
    if (a.IsBoolean() || b.IsBoolean())
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
    if (a.IsString() && b.IsString())
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
    if (a.IsString() && b.IsString())
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
    if (a.IsString() && b.IsString())
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
    if (a.IsString() && b.IsString())
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
    return (a.ToString() + b.ToString());
}


MathValue MathIntDivide(const MathValue& a, const MathValue& b)
{
    int divisor = b.ToInteger();
    
    if (divisor != 0)
        return (a.ToInteger() / divisor);
    
    return numeric_limits<double>::infinity();
}


string MathNumberToString(double number)
{
    if (_finite(number))
    {
        // Number
        ostringstream stream;
        
        stream.precision(numeric_limits<double>::digits10 + 1);
        stream << number;
        
        return stream.str();
    }
    else if (number == numeric_limits<double>::infinity())
    {
        // Positive infinity
        return "Infinity";
    }
    else if (number == -numeric_limits<double>::infinity())
    {
        // Negative infinity
        return "-Infinity";
    }
    else
    {
        // Not a Number (NaN)
        return "NaN";
    }
}


double MathStringToNumber(const string& str)
{
    istringstream stream(str);
    double number;
    
    if (stream >> number)
    {
        // Number
        return number;
    }
    else if (stricmp(str.c_str(), "Infinity") == 0)
    {
        // Positive infinity
        return numeric_limits<double>::infinity();
    }
    else if (stricmp(str.c_str(), "-Infinity") == 0)
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
