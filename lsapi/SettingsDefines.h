
#ifndef __SETTINGSDEFINES_H
#define __SETTINGSDEFINES_H

#include "../utility/common.h"
#include "../utility/stringutility.h"
#include <string>
#include <map>
#include <set>


typedef std::multimap<std::string, std::string, stringicmp> SettingsMap;
typedef std::map<std::string, SettingsMap::iterator> IteratorMap;


#endif // __SETTINGSDEFINES_H_
