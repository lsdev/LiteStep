/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2003,2005 The LiteStep Development Team

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
	return (NULL != lpwz);
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPCWSTR lpwz, UINT_PTR ucchMax)
#else
BOOL IsValidStringPtrCchW(LPCWSTR lpwz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (NULL != lpwz));
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPWSTR lpwz)
#else
BOOL IsValidStringWritePtrW(LPWSTR lpwz)
#endif
{
	return (NULL != lpwz);
}
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPWSTR lpwz, UINT_PTR ucchMax)
#else
BOOL IsValidStringWritePtrCchW(LPWSTR lpwz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (NULL != lpwz));
}

BOOL IsValidStringPtrA(LPCSTR lpsz)
{
	return (NULL != lpsz);
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPCSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringPtrCchA(LPCSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (NULL != lpsz));
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPSTR lpsz)
#else
BOOL IsValidStringWritePtrA(LPSTR lpsz)
#endif
{
	return (NULL != lpsz);
}
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringWritePtrCchA(LPSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (NULL != lpsz));
}

BOOL IsValidReadPtr(CONST VOID *lp)
{
	return (NULL != lp);
}
#ifdef __cplusplus
BOOL IsValidReadPtr(CONST VOID *lp, UINT_PTR ucb)
#else
BOOL IsValidReadPtrUcb(CONST VOID *lp, UINT_PTR ucb)
#endif
{
	return ((ucb > 0) && (NULL != lp));
}

BOOL IsValidWritePtr(LPVOID lp)
{
	return (NULL != lp);
}
#ifdef __cplusplus
BOOL IsValidWritePtr(LPVOID lp, UINT_PTR ucb)
#else
BOOL IsValidWritePtrUcb(LPVOID lp, UINT_PTR ucb)
#endif
{
	return ((ucb > 0) && (NULL != lp));
}

BOOL IsValidCodePtr(FARPROC lfpn)
{
	return (NULL != lfpn);
}
