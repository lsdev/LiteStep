/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef __SETTINGSMANAGER_H
#define __SETTINGSMANAGER_H

#include "../utility/common.h"
#include "../utility/stringutility.h"
#include <string>
#include <map>
#include <set>
#include "parser.h"
#include "settingsdefines.h"
#include "settingsiterator.h"

struct FileInfo
{
	SettingsMap* m_pSettingsMap;
	UINT m_Count;
};

typedef set<SettingsIterator*> IteratorSet;
typedef map<string, FileInfo*, stringicmp> FileMap;

class SettingsManager
{
	IteratorSet m_Iterators;
	SettingsMap m_SettingsMap;
	FileMap m_FileMap;


protected:
	BOOL _FindLine(LPCSTR pszName, SettingsMap::iterator &it);

	void _SetupVars(LPCSTR szLSPath);

#ifdef LS_COMPAT_MATH
    double _MathEvaluate(LPTSTR ptzInput);
#endif //LS_COMPAT_MATH

public:

	SettingsManager(LPCSTR pszLiteStepPath);
	~SettingsManager();

	void ParseFile(LPCSTR pszFileName);

	BOOL GetRCString(LPCSTR pszKeyName, LPSTR pszValue, LPCSTR pszDefStr, int nMaxLen);
	BOOL GetRCLine(LPCSTR pszKeyName, LPSTR pszValue, int nMaxLen, LPCSTR pszDefStr);
	BOOL GetRCBool(LPCSTR pszKeyName, BOOL bIfFound);
	BOOL GetRCBoolDef(LPCSTR pszKeyName, BOOL bDefault);
	int GetRCInt(LPCSTR pszKeyName, int nDefault);
	COLORREF GetRCColor(LPCSTR pszKeyName, COLORREF crDefault);

	BOOL GetVariable(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength);
	void SetVariable(LPCSTR pszKeyName, LPCSTR pszValue);

	FILE* LCOpen (LPCSTR pszPath);
	BOOL LCClose (FILE *pFile);
	BOOL LCReadNextConfig(FILE *pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue);
	BOOL LCReadNextLineOrCommand(FILE *pFile, LPSTR pszValue, size_t cchValue);

	void VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, DWORD dwLength);
};

#endif // __SETTINGSMANAGER_H
