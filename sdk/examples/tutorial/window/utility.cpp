// Tutorial
// Copyright (C) 2006 Litestep Development Team
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <stdio.h>
#include "common.h"
#include "utility.h"


HWND GetLitestepDesktopWindow()
{
    return FindWindow(_T("DesktopBackgroundClass"), NULL);
}


void ModifyWindowStyle(HWND hWnd, UINT uRemove, UINT uAdd)
{
    UINT uStyle = (UINT) GetWindowLong(hWnd, GWL_STYLE);
    SetWindowLong(hWnd, GWL_STYLE, (long) ((uStyle & ~uRemove) | uAdd));
}


int StrCat(LPTSTR pszDest, int cchDest, LPCTSTR pszSrc)
{
    int cch = 0;
    
    // Find the end of the string in the buffer
    while (pszDest && *pszDest != '\0')
    {
        pszDest++;
        cch++;
        cchDest--;
    }
    
    // Copy characters into the buffer from the source string
    while (pszSrc && *pszSrc != '\0')
    {
        if (pszDest && cchDest > 1)
        {
            *pszDest++ = *pszSrc;
            cchDest--;
        }
        
        pszSrc++;
        cch++;
    }
    
    // Make sure buffer is always null-terminated
    if (pszDest && cchDest > 0)
    {
        *pszDest = '\0';
    }
    
    return cch;
}


int StrCompare(LPCTSTR pszLeft, LPCTSTR pszRight)
{
    if (pszLeft)
    {
        if (pszRight)
        {
            // Both non-NULL, compare them
            return _tcscmp(pszLeft, pszRight);
        }
        else
        {
            // Non-NULL > NULL
            return 1;
        }
    }
    else
    {
        if (pszRight)
        {
            // NULL < Non-NULL
            return -1;
        }
        else
        {
            // NULL == NULL
            return 0;
        }
    }
}


int StrCompareIgnoreCase(LPCTSTR pszLeft, LPCTSTR pszRight)
{
    if (pszLeft)
    {
        if (pszRight)
        {
            // Both non-NULL, compare them
            return _tcsicmp(pszLeft, pszRight);
        }
        else
        {
            // Non-NULL > NULL
            return 1;
        }
    }
    else
    {
        if (pszRight)
        {
            // NULL < Non-NULL
            return -1;
        }
        else
        {
            // NULL == NULL
            return 0;
        }
    }
}


int StrCopy(LPTSTR pszDest, int cchDest, LPCTSTR pszSrc)
{
    int cch = 0;
    
    // Copy characters into the buffer from the source string
    while (pszSrc && *pszSrc != '\0')
    {
        if (pszDest && cchDest > 1)
        {
            *pszDest++ = *pszSrc;
            cchDest--;
        }
        
        pszSrc++;
        cch++;
    }
    
    // Make sure buffer is always null-terminated
    if (pszDest && cchDest > 0)
    {
        *pszDest = '\0';
    }
    
    return cch;
}


int StrLen(LPCTSTR pszSrc)
{
    int cch = 0;
    
    // Find the end of the string
    while (pszSrc && *pszSrc != '\0')
    {
        pszSrc++;
        cch++;
    }
    
    return cch;
}


int StrPrintf(LPTSTR pszDest, int cchDest, LPCTSTR pszFormat, ...)
{
    va_list vaList;
    int cch;
    
    va_start(vaList, pszFormat);
    cch = _vsntprintf(pszDest, cchDest, pszFormat, vaList);
    va_end(vaList);
    
    return cch;
}
