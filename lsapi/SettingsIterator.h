
#ifndef __SETTINGSITERATOR_H
#define __SETTINGSITERATOR_H

#include "../utility/common.h"
#include "settingsdefines.h"


class SettingsIterator
{

public:
    SettingsIterator(SettingsMap* pSettingsMap, std::string szPath);

	BOOL ReadNextLine(LPSTR pszValue, size_t cchValue);
	BOOL ReadNextConfig(LPCSTR pszConfig, LPSTR pszValue, size_t cchValue);

    std::string get_Path()
	{
		return m_Path;
	};

private:
	SettingsMap* m_pSettingsMap;
	SettingsMap::iterator m_pFileIterator;
	IteratorMap m_Iterators;
    std::string m_Path;
};



#endif // __SETTINGSITERATOR_H
