#include <sstream>
#include <string>
#include "../utility/localization.h"
#include "MathEvaluate.h"
#include "MathException.h"
#include "MathParser.h"
#include "SettingsDefines.h"

using namespace std;


bool MathEvaluateBool(const SettingsMap& context, const string& expression,
    bool& result, unsigned int flags)
{
    try
    {
        const StringSet recursiveVarSet; // dummy set
        MathParser mathParser(context, expression, recursiveVarSet, flags);
        result = mathParser.Evaluate().ToBoolean();
    }
    catch (const MathException& e)
    {
        // FIXME: Localize this
        ostringstream message;
        
        message << "Error in Expression:\n  "
                << expression << "\n"
                << "\n"
                << "Description:\n  "
                << e.what();
        
        Error(LOCALIZE_THIS, message.str().c_str());
        return false;
    }
    
    return true;
}


bool MathEvaluateString(const SettingsMap& context, const string& expression,
    string& result, const StringSet& recursiveVarSet, unsigned int flags)
{
    try
    {
        MathParser mathParser(context, expression, recursiveVarSet, flags);
        
        if (MATH_VALUE_TO_COMPATIBLE_STRING & flags)
        {
            result = mathParser.Evaluate().ToCompatibleString();
        }
        else
        {
            result = mathParser.Evaluate().ToString();
        }
    }
    catch (const MathException& e)
    {
        // FIXME: Localize this
        ostringstream message;
        
        message << "Error in Expression:\n  "
                << expression << "\n"
                << "\n"
                << "Description:\n  "
                << e.what();
        
        Error(LOCALIZE_THIS, message.str().c_str());
        return false;
    }
    
    return true;
}
