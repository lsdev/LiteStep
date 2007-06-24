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
#ifndef __SAFESTR_H
#define __SAFESTR_H

#include <strsafe.h>

#ifdef StringCchCopy
#undef StringCchCopy
#endif
#ifdef _UNICODE
#define StringCchCopy(pwzDest, cchDest, pwzSrc) \
        StringCchCopyExW(pwzDest, cchDest, pwzSrc, NULL, NULL, STRSAFE_NULL_ON_FAILURE)
#else
#define StringCchCopy(pszDest, cchDest, pszSrc) \
        StringCchCopyExA(pszDest, cchDest, pszSrc, NULL, NULL, STRSAFE_NULL_ON_FAILURE)
#endif // _UNICODE

#ifdef StringCchCopyN
#undef StringCchCopyN
#endif
#ifdef _UNICODE
#define StringCchCopyN(pwzDest, cchDest, pwzSrc, cchSrc) \
        StringCchCopyNExW(pwzDest, cchDest, pwzSrc, cchSrc, NULL, NULL, STRSAFE_NULL_ON_FAILURE)
#else
#define StringCchCopyN(pszDest, cchDest, pszSrc, cchSrc) \
        StringCchCopyNExA(pszDest, cchDest, pszSrc, cchSrc, NULL, NULL, STRSAFE_NULL_ON_FAILURE)
#endif // _UNICODE

#ifdef StringCchCat
#undef StringCchCat
#endif
#ifdef _UNICODE
#define StringCchCat(pwzDest, cchDest, pwzSrc) \
        StringCchCatExW(pwzDest, cchDest, pwzSrc, NULL, NULL, STRSAFE_NO_TRUNCATION)
#else
#define StringCchCat(pszDest, cchDest, pszSrc) \
        StringCchCatExA(pszDest, cchDest, pszSrc, NULL, NULL, STRSAFE_NO_TRUNCATION)
#endif // _UNICODE


#endif // __SAFESTR_H

