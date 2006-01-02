#include <sstream>
#include <string>
#include "lsapi.h"
#include "MathEvaluate.h"
#include "MathException.h"
#include "MathParser.h"
#include "MathValue.h"
#include "SettingsDefines.h"

using namespace std;


bool MathEvaluateBool(const SettingsMap& context, const string& expression,
    bool& result, unsigned int flags)
{
    try
    {
        StringSet recursiveVarSet;
        MathParser mathParser(context, expression, recursiveVarSet, flags);
        result = mathParser.Evaluate().ToBoolean();
    }
    catch (const MathException& e)
    {
        // FIXME: Localize this
        ostringstream message;
        
        message << "Error in Expression:\n"
                << expression << "\n"
                << "\n"
                << "Description:\n"
                << e.what();
        
        MessageBox(GetLitestepWnd(), message.str().c_str(), NULL, MB_SETFOREGROUND);
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
        result = mathParser.Evaluate().ToString();
    }
    catch (const MathException& e)
    {
        // FIXME: Localize this
        ostringstream message;
        
        message << "Error in Expression:\n"
                << expression << "\n"
                << "\n"
                << "Description:\n"
                << e.what();
        
        MessageBox(GetLitestepWnd(), message.str().c_str(), NULL, MB_SETFOREGROUND);
        return false;
    }
    
    return true;
}
