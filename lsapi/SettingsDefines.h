
#ifndef __SETTINGSDEFINES_H
#define __SETTINGSDEFINES_H

#include "../utility/common.h"
#include "../utility/stringutility.h"
#include <string>
#include <map>
#include <set>

using namespace std;

typedef multimap<string, string, stringicmp> SettingsMap;
typedef SettingsMap::value_type value_type;
typedef map<string, SettingsMap::iterator> IteratorMap;

#include "settingsiterator.h"




#endif // __SETTINGSDEFINES_H_
