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
#ifndef __MRU_H
#define __MRU_H

#ifdef _DEBUG
#define _LSDEBUG
#endif

#include "common.h"

//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINCOMMCTRLAPI
#if !defined(_COMCTL32_) && defined(_WIN32)
#define WINCOMMCTRLAPI DECLSPEC_IMPORT
#else
#define WINCOMMCTRLAPI
#endif
#endif // WINCOMMCTRLAPI

#ifdef __cplusplus
extern "C"
{            /* Assume C declarations for C++ */
#endif // __cplusplus

	//===========================================================================
	//
	// MRU Routines
	//
	//===========================================================================

	// CreateMRUList structure
	typedef struct
	{
		DWORD cbSize;
		DWORD nMaxItems;
		DWORD dwFlags;
		HKEY hKey;
		LPCSTR lpszSubKey;
		PROC lpfnCompare;
	}
	CREATEMRULISTA, * LPCREATEMRULISTA;
	typedef const CREATEMRULISTA * LPCCREATEMRULISTA;

	typedef struct
	{
		DWORD cbSize;
		DWORD nMaxItems;
		DWORD dwFlags;
		HKEY hKey;
		LPCWSTR lpszSubKey;
		PROC lpfnCompare;
	}
	CREATEMRULISTW, * LPCREATEMRULISTW;
	typedef const CREATEMRULISTW * LPCCREATEMRULISTW;

#ifdef UNICODE

	typedef CREATEMRULISTW CREATEMRULIST;
	typedef LPCREATEMRULISTW LPCREATEMRULIST;
	typedef LPCCREATEMRULISTW * LPCCREATEMRULIST;
#else

	typedef CREATEMRULISTA CREATEMRULIST;
	typedef LPCREATEMRULISTA LPCREATEMRULIST;
	typedef LPCCREATEMRULISTA * LPCCREATEMRULIST;
#endif // UNICODE

	// CreateMRUList flags
#define MRUF_STRING_LIST  0
#define MRUF_BINARY_LIST  1
#define MRUF_DELAYED_SAVE 2
	//===========================================================================

#ifdef __cplusplus
}
#endif // __cplusplus


#include "lsapi.h"
#include "lsdebug.h"

#define MAX_ITEMS 29

class MRUList
{
private:
	int m_nSize;
	LPSTR m_pszKey;
	int m_nIndex;


	HANDLE m_hMRUList;

public:
	MRUList(LPCSTR pszKey);
	MRUList();
	~MRUList();

	void First(LPSTR buffer, DWORD size);
	void Next(LPSTR buffer, DWORD size);
	void Previous(LPSTR buffer, DWORD size);
	void Current(LPSTR buffer, DWORD size);

	void Add(LPCSTR pszItem);

private:
	void Load();
};

#endif // __MRU_H
