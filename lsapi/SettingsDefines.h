
#ifndef __SETTINGSDEFINES_H
#define __SETTINGSDEFINES_H

#include "../utility/common.h"
#include <string>
#include <map>
#include <set>

using namespace std;

struct stringcmp
{
	bool operator()(const string &s1, const string &s2) const
	{
		return (strcmp(s1.c_str(), s2.c_str()) < 0);
	}
};

typedef multimap<string, string, stringcmp > SettingsMap;
typedef SettingsMap::value_type value_type;
typedef map<string, SettingsMap::iterator> IteratorMap;

#include "settingsiterator.h"




#endif // __SETTINGSDEFINES_H_
