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


HFONT CreateSimpleFont(LPCTSTR pszName, int nHeight, BOOL fBold, BOOL fItalic, BOOL fUnderline)
{
    LOGFONT lf;
    
    lf.lfHeight = nHeight;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = fBold ? FW_BOLD : FW_NORMAL;
    lf.lfItalic = fItalic;
    lf.lfUnderline = fUnderline;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    StrCopy(lf.lfFaceName, LF_FACESIZE, pszName);
    
    return CreateFontIndirect(&lf);
}


HWND GetLitestepDesktopWindow()
{
    return FindWindow(_T("DesktopBackgroundClass"), NULL);
}


int GetRCEnum(LPCTSTR pszKeyName, LPENUM pEnum)
{
    TCHAR szName[MAX_ENUM_NAME];
    
    // Get the name from the configuration file
    GetRCString(pszKeyName, szName, NULL, MAX_ENUM_NAME);
    
    // Find the value mapping for the name
    while (pEnum->pszName)
    {
        if (StrCompareIgnoreCase(szName, pEnum->pszName) == 0)
        {
            // Return the value
            return pEnum->nValue;
        }
        
        pEnum++;
    }
    
    // Value attached to the NULL entry is the default
    return pEnum->nValue;
}


void HorizontalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP)
{
    for (int x = 0; x < cxDest; x += cxSrc)
    {
        StretchBlt(hdcDest,
            xDest + x, yDest,
            min(cxDest - x, cxSrc), cyDest,
            hdcSrc,
            xSrc, ySrc,
            min(cxDest - x, cxSrc), cySrc,
            dwROP);
    }
}


void ModifyWindowStyle(HWND hWnd, UINT uRemove, UINT uAdd)
{
    UINT uStyle = (UINT) GetWindowLong(hWnd, GWL_STYLE);
    SetWindowLong(hWnd, GWL_STYLE, (long) ((uStyle & ~uRemove) | uAdd));
}


void ScaleEdgeBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, int cxLeft, int cyTop, int cxRight, int cyBottom, int nMode, DWORD dwROP)
{
    if (cyTop > 0)
    {
        // Top Left Corner
        if (cxLeft > 0)
        {
            BitBlt(hdcDest,
                xDest, yDest,
                cxLeft, cyTop,
                hdcSrc,
                xSrc, ySrc,
                dwROP);
        }
        
        // Top
        if (nMode == SCALE_STRETCH || nMode == SCALE_VERTICALTILE)
        {
            StretchBlt(hdcDest,
                xDest + cxLeft, yDest,
                cxDest - cxLeft - cxRight, cyTop,
                hdcSrc,
                xSrc + cxLeft, ySrc,
                cxSrc - cxLeft - cxRight, cyTop,
                dwROP);
        }
        else
        {
            HorizontalTileBlt(hdcDest,
                xDest + cxLeft, yDest,
                cxDest - cxLeft - cxRight, cyTop,
                hdcSrc,
                xSrc + cxLeft, ySrc,
                cxSrc - cxLeft - cxRight, cyTop,
                dwROP);
        }
        
        // Top Right Corner
        if (cxRight > 0)
        {
            BitBlt(hdcDest,
                xDest + cxDest - cxRight, yDest,
                cxRight, cyTop,
                hdcSrc,
                xSrc + cxSrc - cxRight, ySrc,
                dwROP);
        }
    }
    
    if (cxLeft > 0)
    {
        // Left
        if (nMode == SCALE_STRETCH || nMode == SCALE_HORIZONTALTILE)
        {
            StretchBlt(hdcDest,
                xDest, yDest + cyTop,
                cxLeft, cyDest - cyTop - cyBottom,
                hdcSrc,
                xSrc, ySrc + cyTop,
                cxLeft, cySrc - cyTop - cyBottom,
                dwROP);
        }
        else
        {
            VerticalTileBlt(hdcDest,
                xDest, yDest + cyTop,
                cxLeft, cyDest - cyTop - cyBottom,
                hdcSrc,
                xSrc, ySrc + cyTop,
                cxLeft, cySrc - cyTop - cyBottom,
                dwROP);
        }
    }
    
    // Center
    if (nMode == SCALE_STRETCH)
    {
        StretchBlt(hdcDest,
            xDest + cxLeft, yDest + cyTop,
            cxDest - cxLeft - cxRight, cyDest - cyTop - cyBottom,
            hdcSrc,
            xSrc + cxLeft, ySrc + cyTop,
            cxSrc - cxLeft - cxRight, cySrc - cyTop - cyBottom,
            dwROP);
    }
    else if (nMode == SCALE_HORIZONTALTILE)
    {
        HorizontalTileBlt(hdcDest,
            xDest + cxLeft, yDest + cyTop,
            cxDest - cxLeft - cxRight, cyDest - cyTop - cyBottom,
            hdcSrc,
            xSrc + cxLeft, ySrc + cyTop,
            cxSrc - cxLeft - cxRight, cySrc - cyTop - cyBottom,
            dwROP);
    }
    else if (nMode == SCALE_VERTICALTILE)
    {
        VerticalTileBlt(hdcDest,
            xDest + cxLeft, yDest + cyTop,
            cxDest - cxLeft - cxRight, cyDest - cyTop - cyBottom,
            hdcSrc,
            xSrc + cxLeft, ySrc + cyTop,
            cxSrc - cxLeft - cxRight, cySrc - cyTop - cyBottom,
            dwROP);
    }
    else if (nMode == SCALE_TILE)
    {
        TileBlt(hdcDest,
            xDest + cxLeft, yDest + cyTop,
            cxDest - cxLeft - cxRight, cyDest - cyTop - cyBottom,
            hdcSrc,
            xSrc + cxLeft, ySrc + cyTop,
            cxSrc - cxLeft - cxRight, cySrc - cyTop - cyBottom,
            dwROP);
    }
    
    if (cxRight > 0)
    {
        // Right
        if (nMode == SCALE_STRETCH || nMode == SCALE_HORIZONTALTILE)
        {
            StretchBlt(hdcDest,
                xDest + cxDest - cxRight, yDest + cyTop,
                cxRight, cyDest - cyTop - cyBottom,
                hdcSrc,
                xSrc + cxSrc - cxRight, ySrc + cyTop,
                cxRight, cySrc - cyTop - cyBottom,
                dwROP);
        }
        else
        {
            VerticalTileBlt(hdcDest,
                xDest + cxDest - cxRight, yDest + cyTop,
                cxRight, cyDest - cyTop - cyBottom,
                hdcSrc,
                xSrc + cxSrc - cxRight, ySrc + cyTop,
                cxRight, cySrc - cyTop - cyBottom,
                dwROP);
        }
    }
    
    if (cyBottom > 0)
    {
        // Bottom Left Corner
        if (cxLeft > 0)
        {
            BitBlt(hdcDest,
                xDest, yDest + cyDest - cyBottom,
                cxLeft, cyBottom,
                hdcSrc,
                xSrc, ySrc + cySrc - cyBottom,
                dwROP);
        }
        
        // Bottom
        if (nMode == SCALE_STRETCH || nMode == SCALE_VERTICALTILE)
        {
            StretchBlt(hdcDest,
                xDest + cxLeft, yDest + cyDest - cyBottom,
                cxDest - cxLeft - cxRight, cyBottom,
                hdcSrc,
                xSrc + cxLeft, ySrc + cySrc - cyBottom,
                cxSrc - cxLeft - cxRight, cyBottom,
                dwROP);
        }
        else
        {
            HorizontalTileBlt(hdcDest,
                xDest + cxLeft, yDest + cyDest - cyBottom,
                cxDest - cxLeft - cxRight, cyBottom,
                hdcSrc,
                xSrc + cxLeft, ySrc + cySrc - cyBottom,
                cxSrc - cxLeft - cxRight, cyBottom,
                dwROP);
        }
        
        // Bottom Right Corner
        if (cxRight > 0)
        {
            BitBlt(hdcDest,
                xDest + cxDest - cxRight, yDest + cyDest - cyBottom,
                cxRight, cyBottom,
                hdcSrc,
                xSrc + cxSrc - cxRight, ySrc + cySrc - cyBottom,
                dwROP);
        }
    }
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


void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP)
{
    for (int y = 0; y < cyDest; y += cySrc)
    {
        for (int x = 0; x < cxDest; x += cxSrc)
        {
            BitBlt(hdcDest,
                xDest + x, yDest + y,
                min(cxDest - x, cxSrc), min(cyDest - y, cySrc),
                hdcSrc,
                xSrc, ySrc,
                dwROP);
        }
    }
}


void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP)
{
    for (int y = 0; y < cyDest; y += cySrc)
    {
        StretchBlt(hdcDest,
            xDest, yDest + y,
            cxDest, min(cyDest - y, cySrc),
            hdcSrc,
            xSrc, ySrc,
            cxSrc, min(cyDest - y, cySrc),
            dwROP);
    }
}
