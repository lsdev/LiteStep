
#ifndef __SETTINGSITERATOR_H
#define __SETTINGSITERATOR_H

#include "../utility/common.h"
#include "settingsdefines.h"

class SettingsManager;

class SettingsIterator
{

public:
	SettingsIterator(SettingsMap* pSettingsMap, string szPath);

	BOOL ReadNextLine(LPSTR pszValue, size_t cchValue);
	BOOL ReadNextConfig(LPCSTR pszConfig, LPSTR pszValue, size_t cchValue);

	string get_Path()
	{
		return m_Path;
	};

private:
	SettingsMap* m_pSettingsMap;
	SettingsMap::iterator m_pFileIterator;
	IteratorMap m_Iterators;
	string m_Path;
};



#endif // __SETTINGSITERATOR_H
