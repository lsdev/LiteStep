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
#include "mru.h"
#include <time.h>

#define MRUKEY "Software\\LiteStep\\RunMRU"

HANDLE (WINAPI *CreateMRUList)(LPCCREATEMRULISTA lpCreateInfo);

void (WINAPI *FreeMRUList)(HANDLE hList);

int (WINAPI *AddMRUString)(HANDLE hList, LPCSTR lpszString);

int (WINAPI *EnumMRUList)(HANDLE hList, int nItemPos,
                          LPVOID lpBuffer, DWORD nBufferSize);


int CALLBACK MRUCompareString(LPCSTR lpszString1, LPCSTR lpszString2);

MRUList::MRUList(LPCSTR pszKey)
{
	m_pszKey = new char[sizeof(pszKey) + 1];
	strcpy(m_pszKey, pszKey);
	m_nIndex = 0;

	Load();
}

MRUList::MRUList()
{
	m_pszKey = new char[sizeof(MRUKEY) + 1];
	strcpy(m_pszKey, MRUKEY);
	m_nIndex = 0;

	Load();
}

MRUList::~MRUList()
{
	delete[] m_pszKey;
	FreeMRUList(m_hMRUList);
}

void MRUList::First(LPSTR buffer, DWORD size)
{
	m_nIndex = 0;
	EnumMRUList(m_hMRUList, 0, buffer, size);
}

void MRUList::Next(LPSTR buffer, DWORD size)
{
	m_nSize = EnumMRUList(m_hMRUList, -1, NULL, NULL);
	if (m_nSize == 0)
		return ;
	if (m_nIndex >= (m_nSize - 1))
		m_nIndex = 0;
	else
		m_nIndex++;
	_LSDEBUG1(m_nIndex)
	EnumMRUList(m_hMRUList, m_nIndex, buffer, size);
	_LSDEBUG1(buffer)
}

void MRUList::Previous(LPSTR buffer, DWORD size)
{
	m_nSize = EnumMRUList(m_hMRUList, -1, NULL, NULL);
	if (m_nSize == 0)
		return ;
	if (m_nIndex == 0)
	{
		m_nIndex = m_nSize - 1;
	}
	else
		m_nIndex--;
	_LSDEBUG1(m_nIndex)
	EnumMRUList(m_hMRUList, m_nIndex, buffer, size);
	_LSDEBUG1(buffer)
}

void MRUList::Current(LPSTR buffer, DWORD size)
{
	m_nSize = EnumMRUList(m_hMRUList, -1, NULL, NULL);
	if (m_nSize == 0)
		return ;
	EnumMRUList(m_hMRUList, m_nIndex, buffer, size);
}

void MRUList::Add(LPCSTR pszItem)
{
	_LSDEBUG1(pszItem)
	AddMRUString(m_hMRUList, pszItem);
	_LSDEBUG1(pszItem)
	m_nSize = EnumMRUList(m_hMRUList, -1, NULL, NULL);
	_LSDEBUG0
}

void MRUList::Load()
{

	CreateMRUList = (HANDLE (WINAPI *)(LPCCREATEMRULISTA)) GetProcAddress(
	                    GetModuleHandle("COMCTL32.DLL"),
	                    MAKEINTRESOURCE(151));

	FreeMRUList = (void (WINAPI *)(HANDLE)) GetProcAddress(
	                  GetModuleHandle("COMCTL32.DLL"),
	                  MAKEINTRESOURCE(152));

	AddMRUString = (int (WINAPI *)(HANDLE, LPCSTR)) GetProcAddress(
	                   GetModuleHandle("COMCTL32.DLL"),
	                   MAKEINTRESOURCE(153));

	EnumMRUList = (int (WINAPI *)(HANDLE, int, LPVOID, DWORD))
	              GetProcAddress(GetModuleHandle("COMCTL32.DLL"),
	                             MAKEINTRESOURCE(154));

	CREATEMRULIST m_mruListCreate;
	LPCREATEMRULIST pmruList = &m_mruListCreate;
	m_hMRUList = NULL;
	m_mruListCreate.cbSize = sizeof(CREATEMRULIST);
	m_mruListCreate.nMaxItems = 29;
	m_mruListCreate.hKey = (HKEY)HKEY_CURRENT_USER;
	m_mruListCreate.lpszSubKey = m_pszKey;
	m_mruListCreate.dwFlags = MRUF_STRING_LIST;
	m_mruListCreate.lpfnCompare = (PROC)MRUCompareString;

	m_hMRUList = CreateMRUList(pmruList);

	m_nSize = EnumMRUList(m_hMRUList, -1, NULL, NULL);
	_LSDEBUG2("MRUList Size", m_nSize)
}


int CALLBACK MRUCompareString(
    LPCSTR lpszString1,
    LPCSTR lpszString2)
{
	return strcmp(lpszString1, lpszString2);
}
