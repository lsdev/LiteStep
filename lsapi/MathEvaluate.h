#if !defined(MATHEVALUATE_H)
#define MATHEVALUATE_H

#include <string>
#include "SettingsDefines.h"


/**
 * Flags for {@link MathEvaluateBool}, {@MathEvaluateFloat}, {@MathEvaluateInt},
 * and {@link MathEvaluateString}.
 */
enum
{
    /** Throws an exception if an undefined variable is referenced. */
    MATH_EXCEPTION_ON_UNDEFINED = (1 << 0)
};


/**
 * Evaluates a math expression and converts the result to a Boolean.
 *
 * @param  context    map with variable bindings
 * @param  expression string with expression to evaluate
 * @param  result     variable to hold expression result
 * @param  flags      flags that control parsing and evaluation
 * @return <code>true</code>  if successful or
 *         <code>false</code> if an error occured
 */
bool MathEvaluateBool(SettingsMap& context,
    const std::string& expression,
    bool& result,
    unsigned int flags = 0);

/**
 * Evaluates a math expression and converts the result to a floating-point number.
 *
 * @param  context    map with variable bindings
 * @param  expression string with expression to evaluate
 * @param  result     variable to hold expression result
 * @param  flags      flags that control parsing and evaluation
 * @return <code>true</code>  if successful or
 *         <code>false</code> if an error occured
 */
bool MathEvaluateFloat(SettingsMap& context,
    const std::string& expression,
    double& result,
    unsigned int flags = 0);

/**
 * Evaluates a math expression and converts the result to an integer.
 *
 * @param  context    map with variable bindings
 * @param  expression string with expression to evaluate
 * @param  result     variable to hold expression result
 * @param  flags      flags that control parsing and evaluation
 * @return <code>true</code>  if successful or
 *         <code>false</code> if an error occured
 */
bool MathEvaluateInt(SettingsMap& context,
    const std::string& expression,
    int& result,
    unsigned int flags = 0);

/**
 * Evaluates a math expression and converts the result to a string.
 *
 * @param  context    map with variable bindings
 * @param  expression string with expression to evaluate
 * @param  result     variable to hold expression result
 * @param  flags      flags that control parsing and evaluation
 * @return <code>true</code>  if successful or
 *         <code>false</code> if an error occured
 */
bool MathEvaluateString(SettingsMap& context,
    const std::string& expression,
    std::string& result,
    unsigned int flags = 0);


#endif
