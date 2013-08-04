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
    
    LSAPI LPVOID LCOpenA(LPCSTR szPath);
    LSAPI LPVOID LCOpenW(LPCWSTR wzPath);
    LSAPI BOOL LCClose(LPVOID pFile);
    LSAPI BOOL LCReadNextCommandA(LPVOID pFile, LPSTR pszValue, size_t cchValue);
    LSAPI BOOL LCReadNextCommandW(LPVOID pFile, LPWSTR pwzValue, size_t cchValue);
    LSAPI BOOL LCReadNextConfigA(LPVOID pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue);
    LSAPI BOOL LCReadNextConfigW(LPVOID pFile, LPCWSTR pwzConfig, LPWSTR pwzValue, size_t cchValue);
    LSAPI BOOL LCReadNextLineA(LPVOID pFile, LPSTR pszValue, size_t cchValue);
    LSAPI BOOL LCReadNextLineW(LPVOID pFile, LPWSTR pwzValue, size_t cchValue);
    LSAPI int LCTokenizeA(LPCSTR szString, LPSTR * lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);
    LSAPI int LCTokenizeW(LPCWSTR wzString, LPWSTR * lpwzBuffers, DWORD dwNumBuffers, LPWSTR wzExtraParameters);
    
    LSAPI int GetRCIntA(LPCSTR lpKeyName, int nDefault);
    LSAPI int GetRCIntW(LPCWSTR lpKeyName, int nDefault);
    LSAPI __int64 GetRCInt64A(LPCSTR lpKeyName, __int64 nDefault);
    LSAPI __int64 GetRCInt64W(LPCWSTR lpKeyName, __int64 nDefault);
    LSAPI float GetRCFloatA(LPCSTR lpKeyName, float fDefault);
    LSAPI float GetRCFloatW(LPCWSTR lpKeyName, float fDefault);
    LSAPI double GetRCDoubleA(LPCSTR lpKeyName, double dDefault);
    LSAPI double GetRCDoubleW(LPCWSTR lpKeyName, double dDefault);
    LSAPI BOOL GetRCStringA(LPCSTR lpKeyName, LPSTR value, LPCSTR defStr, int maxLen);
    LSAPI BOOL GetRCStringW(LPCWSTR lpKeyName, LPWSTR value, LPCWSTR defStr, int maxLen);
    LSAPI BOOL GetRCBoolA(LPCSTR lpKeyName, BOOL ifFound);
    LSAPI BOOL GetRCBoolW(LPCWSTR lpKeyName, BOOL ifFound);
    LSAPI BOOL GetRCBoolDefA(LPCSTR lpKeyName, BOOL bDefault);
    LSAPI BOOL GetRCBoolDefW(LPCWSTR lpKeyName, BOOL bDefault);
    LSAPI BOOL GetRCLineA(LPCSTR lpKeyName, LPSTR value, UINT maxLen, LPCSTR defStr);
    LSAPI BOOL GetRCLineW(LPCWSTR lpKeyName, LPWSTR value, UINT maxLen, LPCWSTR defStr);
    LSAPI COLORREF GetRCColorA(LPCSTR lpKeyName, COLORREF colDef);
    LSAPI COLORREF GetRCColorW(LPCWSTR lpKeyName, COLORREF colDef);
    
    LSAPI BOOL LSGetVariableA(LPCSTR pszKeyName, LPSTR pszValue);
    LSAPI BOOL LSGetVariableW(LPCWSTR pwzKeyName, LPWSTR pwzValue);
    LSAPI BOOL LSGetVariableExA(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength);
    LSAPI BOOL LSGetVariableExW(LPCWSTR pwzKeyName, LPWSTR pwzValue, DWORD dwLength);
    LSAPI void LSSetVariableA(LPCSTR pszKeyName, LPCSTR pszValue);
    LSAPI void LSSetVariableW(LPCWSTR pwzKeyName, LPCWSTR pwzValue);
    
    LSAPI BOOL AddBangCommandA(LPCSTR pszCommand, BangCommandA pfnBangCommand);
    LSAPI BOOL AddBangCommandW(LPCWSTR pwzCommand, BangCommandW pfnBangCommand);
    LSAPI BOOL AddBangCommandExA(LPCSTR pszCommand, BangCommandExA pfnBangCommand);
    LSAPI BOOL AddBangCommandExW(LPCWSTR pwzCommand, BangCommandExW pfnBangCommand);
    LSAPI BOOL RemoveBangCommandA(LPCSTR pszCommand);
    LSAPI BOOL RemoveBangCommandW(LPCWSTR pwzCommand);
    LSAPI BOOL ParseBangCommandA(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs);
    LSAPI BOOL ParseBangCommandW(HWND hCaller, LPCWSTR pwzCommand, LPCWSTR pwzArgs);
    
    LSAPI HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, int xoffset, int yoffset);
    LSAPI HBITMAP BitmapFromIcon (HICON hIcon);
    LSAPI HBITMAP LoadLSImageA(LPCSTR pszFile, LPCSTR pszImage);
    LSAPI HBITMAP LoadLSImageW(LPCWSTR pwzFile, LPCWSTR pwzImage);
    LSAPI HICON LoadLSIconA(LPCSTR pszImage, LPCSTR pszFile);
    LSAPI HICON LoadLSIconW(LPCWSTR pwzImage, LPCWSTR pwzFile);
    LSAPI void GetLSBitmapSize(HBITMAP hBitmap, LPINT nWidth, LPINT nHeight);
    LSAPI void TransparentBltLS(HDC dc, int nXDest, int nYDest, int nWidth, int nHeight, HDC tempDC, int nXSrc, int nYSrc, COLORREF colorTransparent);
    
    LSAPI int CommandTokenizeA(LPCSTR szString, LPSTR * lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);
    LSAPI int CommandTokenizeW(LPCWSTR szString, LPWSTR * lpszBuffers, DWORD dwNumBuffers, LPWSTR szExtraParameters);
    LSAPI void CommandParseA(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs);
    LSAPI void CommandParseW(LPCWSTR pszCommand, LPWSTR pszOutCommand, LPWSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs);
    
    LSAPI HINSTANCE LSExecuteA(HWND Owner, LPCSTR szCommand, int nShowCmd);
    LSAPI HINSTANCE LSExecuteW(HWND Owner, LPCWSTR wzCommand, int nShowCmd);
    LSAPI HINSTANCE LSExecuteExA(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd);
    LSAPI HINSTANCE LSExecuteExW(HWND Owner, LPCWSTR wzOperation, LPCWSTR wzCommand, LPCWSTR wzArgs, LPCWSTR wzDirectory, int nShowCmd);
    
    LSAPI HWND GetLitestepWnd(void);
    LSAPI BOOL WINAPI LSGetLitestepPathA(LPSTR pszPath, size_t cchPath);
    LSAPI BOOL WINAPI LSGetLitestepPathW(LPWSTR pwzPath, size_t cchPath);
    LSAPI BOOL WINAPI LSGetImagePathA(LPSTR pszPath, size_t cchPath);
    LSAPI BOOL WINAPI LSGetImagePathW(LPWSTR pwzPath, size_t cchPath);
    
    LSAPI void VarExpansionA(LPSTR pszExpandedString, LPCSTR pszTemplate);
    LSAPI void VarExpansionW(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate);
    LSAPI void VarExpansionExA(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchExpandedString);
    LSAPI void VarExpansionExW(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate, size_t cchExpandedString);
    
    LSAPI BOOL GetTokenA(LPCSTR pszString, LPSTR pszToken, LPCSTR * pszNextToken, BOOL useBrackets);
    LSAPI BOOL GetTokenW(LPCWSTR pwzString, LPWSTR pwzToken, LPCWSTR * pwzNextToken, BOOL useBrackets);
    LSAPI void Frame3D(HDC dc, RECT rect, COLORREF TopColor, COLORREF BottomColor, int Width);
    LSAPI void SetDesktopArea(int left, int top, int right, int bottom);
    
    LSAPI BOOL matchA(LPCSTR pattern, LPCSTR text);
    LSAPI BOOL matchW(LPCWSTR pattern, LPCWSTR text);
    LSAPI int matcheA(LPCSTR pattern, LPCSTR text);
    LSAPI int matcheW(LPCWSTR pattern, LPCWSTR text);
    LSAPI BOOL is_valid_patternA(LPCSTR p, LPINT error_type);
    LSAPI BOOL is_valid_patternW(LPCWSTR p, LPINT error_type);
    
    LSAPI void GetResStrA(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText);
    LSAPI void GetResStrW(HINSTANCE hInstance, UINT uIDText, LPWSTR pwzText, size_t cchText, LPCWSTR pwzDefText);
    LSAPI void GetResStrExA(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...);
    LSAPI void GetResStrExW(HINSTANCE hInstance, UINT uIDText, LPWSTR pwzText, size_t cchText, LPCWSTR pwzDefText, ...);
    
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
    
    LSAPI HRESULT EnumLSDataA(UINT uInfo, FARPROC pfnCallback, LPARAM lParam);
    LSAPI HRESULT EnumLSDataW(UINT uInfo, FARPROC pfnCallback, LPARAM lParam);
    
#if defined(LSAPI_PRIVATE)
    LSAPI BOOL LSAPIInitialize(LPCWSTR pwzLitestepPath, LPCWSTR pwzRcPath);
    LSAPI void LSAPIReloadBangs(void);
    LSAPI void LSAPIReloadSettings(void);
    LSAPI void LSAPISetLitestepWindow(HWND hLitestepWnd);
    LSAPI BOOL InternalExecuteBangCommand(HWND hCaller, LPCWSTR pszCommand, LPCWSTR pwzArgs);
#endif /* LSAPI_PRIVATE */
    
#if defined(__cplusplus)
};
#endif  // __cplusplus

#endif  // LSAPI_H
