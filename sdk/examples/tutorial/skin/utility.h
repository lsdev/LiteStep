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

#if !defined(UTILITY_H)
#define UTILITY_H

#include "common.h"
#include "utility.h"


// Name-value mapping for use with GetRCEnum
typedef struct ENUM {
    LPCTSTR pszName;
    int nValue;
} ENUM, *LPENUM;

// ScaleEdgeBlt modes
#define SCALE_STRETCH 0
#define SCALE_HORIZONTALTILE 1
#define SCALE_VERTICALTILE 2
#define SCALE_TILE 3


/**
 * Creates a font using defaults for most parameters.
 */
HFONT CreateSimpleFont(LPCTSTR pszName, int nHeight, BOOL fBold, BOOL fItalic, BOOL fUnderline);

/**
 * Returns a handle to Litestep's desktop window.
 */
HWND GetLitestepDesktopWindow();

/**
 * Gets an enumerated value from the configuration file.
 */
int GetRCEnum(LPCTSTR pszKeyName, LPENUM pEnum);

/**
 * Fills the destination with the source by tiling it horizontally and
 * stretching it vertically.
 */
void HorizontalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP);

/**
 * Simultaneously removes one set of styles from a window while adding another.
 */
void ModifyWindowStyle(HWND hWnd, UINT uRemove, UINT uAdd);

/**
 * Fills the destination with the source by tiling or stretching but leaves the
 * edges intact.
 */
void ScaleEdgeBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, int cxLeft, int cyTop, int cxRight, int cyBottom, int nMode, DWORD dwROP);

/**
 * Concatenates a string to the contents of a string buffer.
 */
int StrCat(LPTSTR pszDest, int cchDest, LPCTSTR pszSrc);

/**
 * Compare two strings lexicographically.
 */
int StrCompare(LPCTSTR pszLeft, LPCTSTR pszRight);

/**
 * Compare two strings lexicographically, ignoring differences in case.
 */
int StrCompareIgnoreCase(LPCTSTR pszLeft, LPCTSTR pszRight);

/**
 * Copies a string into a string buffer.
 */
int StrCopy(LPTSTR pszDest, int cchDest, LPCTSTR pszSrc);

/**
 * Returns the length of a string.
 */
int StrLen(LPCTSTR pszSrc);

/**
 * Formats a string.
 */
int StrPrintf(LPTSTR pszDest, int cchDest, LPCTSTR pszFormat, ...);

/**
 * Fills the destination with the source by tiling it horizontally and
 * vertically.
 */
void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP);

/**
 * Fills the destination with the source by tiling it vertically and stretching
 * it horizontally.
 */
void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD dwROP);


#endif // UTILITY_H
