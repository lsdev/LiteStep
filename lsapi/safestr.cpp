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
/****************************************************************************
****************************************************************************/
#include "safestr.h" // Always include last in cpp file


bool CharInStr(const char chr, LPCSTR szString)
{
	if (!(chr && szString))
		return false;

	return strchr(szString, chr) != NULL;
}


LPCSTR StrIPos(LPCSTR s1, LPCSTR s2)
{
	if (!(StrLen(s1) && StrLen(s2)))
		return NULL;

	LPCSTR result;
	LPSTR t1 = new char[StrLen(s1) + 1];
	LPSTR t2 = new char[StrLen(s2) + 1];

	strlwr(StrCopy(t1, s1));
	strlwr(StrCopy(t2, s2));
	result = strstr(t1, t2);
	if (result)
		result = s1 + (result - t1);

	delete[] t1;
	delete[] t2;
	return result;
}


LPSTR StrLCopy(LPSTR szDest, LPCSTR szSource, size_t dwMaxLen)
{
	size_t maxLen;

	if (!(szDest && szSource))
		return NULL;

	maxLen = StrLen(szSource) < dwMaxLen ? StrLen(szSource) : dwMaxLen;

	strncpy(szDest, szSource, maxLen);
	szDest[maxLen] = '\0';

	return szDest;
}

bool CharInStrW(const wchar_t chr, LPCWSTR szString)
{
	if (!(chr && szString))
		return false;

	return wcschr(szString, chr) != NULL;
}


LPCWSTR StrIPosW(LPCWSTR s1, LPCWSTR s2)
{
	if (!(StrLenW(s1) && StrLenW(s2)))
		return NULL;

	LPCWSTR result;
	LPWSTR t1 = new wchar_t[StrLenW(s1) + 1];
	LPWSTR t2 = new wchar_t[StrLenW(s2) + 1];

	wcslwr(StrCopyW(t1, s1));
	wcslwr(StrCopyW(t2, s2));
	result = wcsstr(t1, t2);
	if (result)
		result = s1 + (result - t1);

	delete[] t1;
	delete[] t2;
	return result;
}


LPWSTR StrLCopyW(LPWSTR szDest, LPCWSTR szSource, size_t dwMaxLen)
{
	size_t maxLen;

	if (!(szDest && szSource))
		return NULL;

	maxLen = StrLenW(szSource) < dwMaxLen ? StrLenW(szSource) : dwMaxLen;

	wcsncpy(szDest, szSource, maxLen);
	szDest[maxLen] = L'\0';

	return szDest;
}

