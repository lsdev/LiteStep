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
#include "safeptr.h"

BOOL IsValidStringPtrW(LPCWSTR lpwz)
{
	return ((lpwz != NULL) && (IsBadStringPtrW(lpwz, (UINT) - 1) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPCWSTR lpwz, UINT_PTR ucchMax)
#else
BOOL IsValidStringPtrCchW(LPCWSTR lpwz, UINT_PTR ucchMax)
#endif
{
	return ((lpwz != NULL) && (IsBadStringPtrW(lpwz, ucchMax) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPWSTR lpwz)
#else
BOOL IsValidStringWritePtrW(LPWSTR lpwz)
#endif
{
	return ((lpwz != NULL) && (IsBadWritePtr(lpwz, (wcslen(lpwz) * sizeof(WCHAR))) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPWSTR lpwz, UINT_PTR ucchMax)
#else
BOOL IsValidStringWritePtrCchW(LPWSTR lpwz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (lpwz != NULL) && (IsBadWritePtr(lpwz, (ucchMax * sizeof(WCHAR))) == 0));
}

BOOL IsValidStringPtrA(LPCSTR lpsz)
{
	return ((lpsz != NULL) && (IsBadStringPtrA(lpsz, (UINT) - 1) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPCSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringPtrCchA(LPCSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((lpsz != NULL) && (IsBadStringPtrA(lpsz, ucchMax) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPSTR lpsz)
#else
BOOL IsValidStringWritePtrA(LPSTR lpsz)
#endif
{
	return ((lpsz != NULL) && (IsBadWritePtr(lpsz, (strlen(lpsz) * sizeof(char))) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringWritePtrCchA(LPSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (lpsz != NULL) && (IsBadWritePtr(lpsz, (ucchMax * sizeof(char))) == 0));
}

BOOL IsValidReadPtr(CONST VOID *lp)
{
	return ((lp != NULL) && (IsBadReadPtr(lp, sizeof(lp)) == 0));
}
#ifdef __cplusplus
BOOL IsValidReadPtr(CONST VOID *lp, UINT_PTR ucb)
#else
BOOL IsValidReadPtrUcb(CONST VOID *lp, UINT_PTR ucb)
#endif
{
	return ((lp != NULL) && (IsBadReadPtr(lp, ucb) == 0));
}
BOOL IsValidWritePtr(LPVOID lp)
{
	return ((lp != NULL) && (IsBadWritePtr(lp, sizeof(lp)) == 0));
}
#ifdef __cplusplus
BOOL IsValidWritePtr(LPVOID lp, UINT_PTR ucb)
#else
BOOL IsValidWritePtrUcb(LPVOID lp, UINT_PTR ucb)
#endif
{
	return ((lp != NULL) && (IsBadWritePtr(lp, ucb) == 0));
}
BOOL IsValidCodePtr(FARPROC lfpn)
{
	return ((lfpn != NULL) && (IsBadCodePtr(lfpn) == 0));
}
