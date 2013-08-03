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
#if !defined(MATHEVALUATE_H)
#define MATHEVALUATE_H

#include "SettingsDefines.h"
#include <string>


/**
 * Flags for {@link MathEvaluateBool} and {@link MathEvaluateString}.
 */
enum
{
    /** Throws an exception if an undefined variable is referenced. */
     MATH_EXCEPTION_ON_UNDEFINED      = (1 << 0)
    /** Evaluates all numbers as integers when converting to a String. */
    ,MATH_VALUE_TO_COMPATIBLE_STRING  = (1 << 2)
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
bool MathEvaluateBool(const SettingsMap& context,
    const std::wstring& expression,
    bool& result,
    unsigned int flags = 0);


/**
 * Evaluates a math expression and converts the result to a string.
 *
 * @param  context         map with variable bindings
 * @param  expression      string with expression to evaluate
 * @param  result          variable to hold expression result
 * @param  recursiveVarSet set of variables to check for recursive definitions
 * @param  flags           flags that control parsing and evaluation
 * @return <code>true</code>  if successful or
 *         <code>false</code> if an error occured
 */
bool MathEvaluateString(const SettingsMap& context,
    const std::wstring& expression,
    std::wstring& result,
    const StringSet& recursiveVarSet,
    unsigned int flags = 0);


#endif // MATHEVALUATE_H
