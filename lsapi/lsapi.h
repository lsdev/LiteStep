//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2013  LiteStep Development Team
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
#if !defined(LSAPI_H)
#define LSAPI_H

/*
 * IMPORTANT:
 *   The exports and definitions included
 *   with LSAPI_PRIVATE are for CORE USE ONLY!
 *   
 *   They MUST NOT be used by any module.
 *   Use outside of the CORE may cause instability and unexpected behavior.
 */

#include "lsapidefines.h"
#include "../utility/common.h"
#include <stdio.h>


#if defined(LSAPI_INTERNAL) /* only defined when compiling lsapi */
#  define LSAPI __declspec(dllexport)
#else
#  define LSAPI __declspec(dllimport)
#endif /* LSAPI_INTERNAL */


#if defined(__cplusplus)
extern "C"
{
#endif  // __cplusplus
    
    LSAPI LPVOID LCOpen(LPCSTR szPath);
    LSAPI BOOL LCClose(LPVOID pFile);
    LSAPI BOOL LCReadNextCommand(LPVOID pFile, LPSTR pszValue, size_t cchValue);
    LSAPI BOOL LCReadNextConfig(LPVOID pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue);
    LSAPI BOOL LCReadNextLine(LPVOID pFile, LPSTR pszValue, size_t cchValue);
    LSAPI int LCTokenize (LPCSTR szString, LPSTR * lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);
    
    LSAPI int GetRCInt(LPCSTR lpKeyName, int nDefault);
    LSAPI BOOL GetRCString(LPCSTR lpKeyName, LPSTR value, LPCSTR defStr, int maxLen);
    LSAPI BOOL GetRCBool(LPCSTR lpKeyName, BOOL ifFound);
    LSAPI BOOL GetRCBoolDef(LPCSTR lpKeyName, BOOL bDefault);
    LSAPI BOOL GetRCLine(LPCSTR, LPSTR, UINT, LPCSTR);
    LSAPI COLORREF GetRCColor(LPCSTR lpKeyName, COLORREF colDef);
    
    LSAPI BOOL LSGetVariable(LPCSTR pszKeyName, LPSTR pszValue);
    LSAPI BOOL LSGetVariableEx(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength);
    LSAPI void LSSetVariable(LPCSTR pszKeyName, LPCSTR pszValue);
    
    LSAPI BOOL AddBangCommand(LPCSTR pszCommand, BangCommand pfnBangCommand);
    LSAPI BOOL AddBangCommandEx(LPCSTR pszCommand, BangCommandEx pfnBangCommand);
    LSAPI BOOL RemoveBangCommand(LPCSTR pszCommand);
    LSAPI BOOL ParseBangCommand (HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs);
    
    LSAPI HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, int xoffset, int yoffset);
    LSAPI HBITMAP BitmapFromIcon (HICON hIcon);
    LSAPI HBITMAP LoadLSImage(LPCSTR szFile, LPCSTR szImage);
    LSAPI HICON LoadLSIcon (LPCSTR szImage, LPCSTR szFile);
    LSAPI void GetLSBitmapSize(HBITMAP hBitmap, LPINT nWidth, LPINT nHeight);
    LSAPI void TransparentBltLS (HDC dc, int nXDest, int nYDest, int nWidth, int nHeight, HDC tempDC, int nXSrc, int nYSrc, COLORREF colorTransparent);
    
    LSAPI int CommandTokenize (LPCSTR szString, LPSTR * lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);
    LSAPI void CommandParse(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs);
    
    LSAPI HINSTANCE LSExecute(HWND Owner, LPCSTR szCommand, int nShowCmd);
    LSAPI HINSTANCE LSExecuteEx(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd);
    
    LSAPI HWND GetLitestepWnd(void);
    LSAPI BOOL WINAPI LSGetLitestepPath(LPSTR pszPath, size_t cchPath);
    LSAPI BOOL WINAPI LSGetImagePath(LPSTR pszPath, size_t cchPath);
    
    LSAPI void VarExpansion(LPSTR pszExpandedString, LPCSTR pszTemplate);
    LSAPI void VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchExpandedString);
    
    LSAPI BOOL GetToken(LPCSTR szString, LPSTR szToken, LPCSTR * szNextToken, BOOL useBrackets);
    LSAPI void Frame3D(HDC dc, RECT rect, COLORREF TopColor, COLORREF BottomColor, int Width);
    LSAPI void SetDesktopArea(int left, int top, int right, int bottom);
    
    LSAPI BOOL match(LPCSTR pattern, LPCSTR text);
    LSAPI int matche(LPCSTR pattern, LPCSTR text);
    LSAPI BOOL is_valid_pattern(LPCSTR p, LPINT error_type);
    
    LSAPI void GetResStr(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText);
    LSAPI void GetResStrEx(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...);
    
    LSAPI int LSGetSystemMetrics(int);
    LSAPI HMONITOR LSMonitorFromWindow(HWND, DWORD);
    LSAPI HMONITOR LSMonitorFromRect(LPCRECT, DWORD);
    LSAPI HMONITOR LSMonitorFromPoint(POINT, DWORD);
    LSAPI BOOL LSGetMonitorInfo(HMONITOR, LPMONITORINFO);
    LSAPI BOOL LSEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
    LSAPI BOOL LSEnumDisplayDevices(LPCSTR, DWORD, PDISPLAY_DEVICEA, DWORD);
    
    LSAPI BOOL WINAPI LSLog(int nLevel, LPCSTR pszModule, LPCSTR pszMessage);
    LSAPI BOOL WINAPIV LSLogPrintf(int nLevel, LPCSTR pszModule, LPCSTR pszFormat, ...);
    LSAPI int GetRCCoordinate(LPCSTR pszKeyName, int nDefault, int nMaxVal);
    LSAPI int ParseCoordinate(LPCSTR szString, int nDefault, int nMaxVal);
    
    LSAPI HRESULT EnumLSData(UINT uInfo, FARPROC pfnCallback, LPARAM lParam);
    
#if defined(LSAPI_PRIVATE)
    LSAPI BOOL LSAPIInitialize(LPCSTR pszLitestepPath, LPCSTR pszRcPath);
    LSAPI void LSAPIReloadBangs(void);
    LSAPI void LSAPIReloadSettings(void);
    LSAPI void LSAPISetLitestepWindow(HWND hLitestepWnd);
    LSAPI BOOL InternalExecuteBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs);
#endif /* LSAPI_PRIVATE */
    
#if defined(__cplusplus)
};
#endif  // __cplusplus

#endif  // LSAPI_H
