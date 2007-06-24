//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#if !defined(SAFEPTR_H)
#define SAFEPTR_H

#include "common.h"

//
// Valid String Pointer Functions
//

//
// Unicode
//
BOOL IsValidStringPtrW(LPCWSTR lpwz);
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPCWSTR lpwz, UINT_PTR ucchMax);
#else // __cplusplus
BOOL IsValidStringPtrCchW(LPCWSTR lpwz, UINT_PTR ucchMax);
#endif // __cplusplus
#ifdef __cplusplus
BOOL IsValidStringPtrW(LPWSTR lpwz);
BOOL IsValidStringPtrW(LPWSTR lpwz, UINT_PTR ucchMax);
#else
BOOL IsValidStringReadPtrW(LPWSTR lpwz);
BOOL IsValidStringReadPtrCchW(LPWSTR lpwz, UINT_PTR ucchMax);
#endif // __cplusplus

//
// Ansi
//
BOOL IsValidStringPtrA(LPCSTR lpsz);
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPCSTR lpsz, UINT_PTR ucchMax);
#else // __cplusplus
BOOL IsValidStringPtrCchA(LPCSTR lpsz, UINT_PTR ucchMax);
#endif // __cplusplus
#ifdef __cplusplus
BOOL IsValidStringPtrA(LPSTR lpsz);
BOOL IsValidStringPtrA(LPSTR lpsz, UINT_PTR ucchMax);
#else // __cplusplus
BOOL IsValidStringReadPtrA(LPSTR lpsz);
BOOL IsValidStringReadPtrCchA(LPSTR lpsz, UINT_PTR ucchMax);
#endif // __cplusplus

//
// TCHAR defines
//
#ifdef _UNICODE
#ifdef __cplusplus
#define IsValidStringPtr IsValidStringPtrW
#else // __cplusplus
#define IsValidStringPtr(lpwz) IsValidStringPtrW(lpwz)
#define IsValidStringPtrCch(lpwz, ucchMax) IsValidStringPtrCchW(lpwz, ucchMax)
#define IsValidStringReadPtr(lpwz) IsValidStringReadPtrW(lpwz)
#define IsValidStringReadPtrCch(lpwz, ucchMax) IsValidStringReadPtrCchW(lpwz, ucchMax)
#endif // _cplusplus
#else // _UNICODE
#ifdef __cplusplus
#define IsValidStringPtr IsValidStringPtrA
#else // __cplusplus
#define IsValidStringPtr(lpsz) IsValidStringPtrA(lpsz)
#define IsValidStringPtrCch(lpsz, ucchMax) IsValidStringPtrCchA(lpsz, ucchMax)
#define IsValidStringReadPtr(lpsz) IsValidStringReadPtrA(lpsz)
#define IsValidStringReadPtrCch(lpsz, ucchMax) IsValidStringReadPtrCchA(lpsz, ucchMax)
#endif // _cplusplus
#endif // _UNICODE

//
// Valid Pointer Functions
//
BOOL IsValidReadPtr(CONST VOID *lp);
#ifdef __cplusplus
BOOL IsValidReadPtr(CONST VOID *lp, UINT_PTR ucb);
#else // __cplusplus
BOOL IsValidReadPtrUcb(CONST VOID *lp, UINT_PTR ucb);
#endif // __cplusplus
BOOL IsValidWritePtr(LPVOID lp);
#ifdef __cplusplus
BOOL IsValidWritePtr(LPVOID lp, UINT_PTR ucb);
#else // __cplusplus
BOOL IsValidWritePtrUcb(LPVOID lp, UINT_PTR ucb);
#endif // __cplusplus
BOOL IsValidCodePtr(FARPROC lfpn);

#endif // SAFEPTR_H
