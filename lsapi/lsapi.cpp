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
#include "lsapi.h"
#include "lsapiinit.h"
#include "BangCommand.h"
#include "../utility/core.hpp"

static int _Tokenize(LPCSTR pszString, LPSTR* lpszBuffers, DWORD dwNumBuffers,
                     LPSTR pszExtraParameters, BOOL bUseBrackets);

BOOL LSAPIInitialize(LPCWSTR pszLitestepPath, LPCWSTR pszRcPath)
{
    try
    {
        g_LSAPIManager.Initialize(pszLitestepPath, pszRcPath);
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }
    
    return g_LSAPIManager.IsInitialized() ? TRUE : FALSE;
}


void LSAPIReloadBangs(VOID)
{
    try
    {
        g_LSAPIManager.ReloadBangs();
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }
}


void LSAPIReloadSettings(VOID)
{
    try
    {
        g_LSAPIManager.ReloadSettings();
    }
    catch(LSAPIException& lse)
    {
        lse.Type();
    }
}


void LSAPISetLitestepWindow(HWND hLitestepWnd)
{
    g_LSAPIManager.SetLitestepWindow(hLitestepWnd);
}


template<typename BangType>
static BOOL AddBangCommandWorker(LPCWSTR pwzCommand, BangType pfnBangCommand)
{
    BOOL bReturn = FALSE;
    
    if (pwzCommand != nullptr && pfnBangCommand != nullptr)
    {
        DWORD dwCurrentThreadID = GetCurrentThreadId();
        
        Bang* pBang = new Bang(dwCurrentThreadID, pfnBangCommand, pwzCommand);
        
        if (pBang != nullptr)
        {
            //bBang->AddRef();
            g_LSAPIManager.GetBangManager()->AddBangCommand(pwzCommand, pBang);
            pBang->Release();
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}


//
// AddBangCommandW
//
BOOL AddBangCommandW(LPCWSTR pwzCommand, BangCommandW pfnBangCommand)
{
    return AddBangCommandWorker(pwzCommand, pfnBangCommand);
}


//
// AddBangCommandA
//
BOOL AddBangCommandA(LPCSTR pszCommand, BangCommandA pfnBangCommand)
{
    return AddBangCommandWorker(std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get(),
        pfnBangCommand);
}


//
// AddBangCommandExW
//
BOOL AddBangCommandExW(LPCWSTR pwzCommand, BangCommandExW pfnBangCommand)
{
    return AddBangCommandWorker(pwzCommand, pfnBangCommand);
}


//
// AddBangCommandExA
//
BOOL AddBangCommandExA(LPCSTR pszCommand, BangCommandExA pfnBangCommand)
{
    return AddBangCommandWorker(std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get(),
        pfnBangCommand);
}


//
// RemoveBangCommandW(LPCWSTR pwzCommand)
//
BOOL RemoveBangCommandW(LPCWSTR pwzCommand)
{
    BOOL bResult = FALSE;
    
    if (pwzCommand != nullptr)
    {
        bResult = \
            g_LSAPIManager.GetBangManager()->RemoveBangCommand(pwzCommand);
    }
    
    return bResult;
}


//
// RemoveBangCommandA(LPCSTR pszCommand)
//
BOOL RemoveBangCommandA(LPCSTR pszCommand)
{
    BOOL bResult = FALSE;
    
    if (pszCommand != nullptr)
    {
        bResult = g_LSAPIManager.GetBangManager()->RemoveBangCommand(
            std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get()
        );
    }
    
    return bResult;
}


//
// InternalExecuteBangCommand
//   (Just like ParseBangCommand but without the variable expansion)
//
BOOL InternalExecuteBangCommand(HWND hCaller, LPCWSTR pwzCommand, LPCWSTR pwzArgs)
{
    return g_LSAPIManager.GetBangManager()->
        ExecuteBangCommand(pwzCommand, hCaller, pwzArgs);
}


//
// ParseBangCommandW
//
BOOL ParseBangCommandW(HWND hCaller, LPCWSTR pwzCommand, LPCWSTR pwzArgs)
{
    TRACE("ParseBangCommand(%p, \"%ls\", \"%ls\");",
        hCaller, pwzCommand, pwzArgs);
    
    wchar_t wzExpandedArgs[MAX_LINE_LENGTH] = { 0 };
    BOOL bReturn = FALSE;
    
    if (pwzCommand != nullptr)
    {
        if (pwzArgs != nullptr)
        {
            VarExpansionExW(wzExpandedArgs, pwzArgs, MAX_LINE_LENGTH);
        }
        
        bReturn = \
            InternalExecuteBangCommand(hCaller, pwzCommand, wzExpandedArgs);
    }
    
    return bReturn;
}


//
// ParseBangCommandA
//
BOOL ParseBangCommandA(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
{
    return ParseBangCommandW(hCaller,
        std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get(),
        std::unique_ptr<wchar_t>(WCSFromMBS(pszArgs)).get());
}


//
// CommandParseW
//
void CommandParseW(LPCWSTR pwzCommand, LPWSTR pwzOutCommand, LPWSTR pwzOutArgs, size_t cchOutCommand, size_t cchOutArgs)
{
    wchar_t wzCommand[MAX_LINE_LENGTH];
    wchar_t wzTempCommand[MAX_LINE_LENGTH];
    LPCWSTR pwzTempArgs = nullptr;
    
    if (pwzCommand != nullptr)
    {
        if (pwzOutCommand != nullptr && cchOutCommand > 0)
        {
            VarExpansionExW(wzCommand, pwzCommand, MAX_LINE_LENGTH);
            
            GetTokenW(wzCommand, wzTempCommand, &pwzTempArgs, TRUE);
            
            StringCchCopyW(pwzOutCommand, cchOutCommand, wzTempCommand);
        }
        
        if (pwzOutArgs != nullptr && cchOutArgs > 0)
        {
            StringCchCopyW(pwzOutArgs, cchOutArgs, pwzTempArgs);
        }
    }
}


//
// CommandParseA
//
void CommandParseA(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs)
{
    char szCommand[MAX_LINE_LENGTH];
    char szTempCommand[MAX_LINE_LENGTH];
    LPCSTR pszTempArgs = nullptr;
    
    if (pszCommand != nullptr)
    {
        if (pszOutCommand != nullptr && cchOutCommand > 0)
        {
            VarExpansionExA(szCommand, pszCommand, MAX_LINE_LENGTH);
            
            GetTokenA(szCommand, szTempCommand, &pszTempArgs, TRUE);
            
            StringCchCopyA(pszOutCommand, cchOutCommand, szTempCommand);
        }
        
        if (pszOutArgs != nullptr && cchOutArgs > 0)
        {
            StringCchCopyA(pszOutArgs, cchOutArgs, pszTempArgs);
        }
    }
}


//
// LSExecuteExW
//
HINSTANCE LSExecuteExW(HWND hOwner, LPCWSTR pwzOperation, LPCWSTR pwzCommand, LPCWSTR pwzArgs, LPCWSTR pwzDirectory, int nShowCmd)
{
    HINSTANCE hReturn = HINSTANCE(32);
    
    if (pwzCommand != nullptr)
    {
        if (pwzCommand[0] == L'!')
        {
            hReturn = ParseBangCommandW(hOwner, pwzCommand, pwzArgs) ?
                HINSTANCE(33) : HINSTANCE(32);
        }
        else
        {
            TRACE("LSExecuteEx(%p, \"%ls\", \"%ls\", \"%ls\", \"%ls\", %d);",
                hOwner, pwzOperation, pwzCommand, pwzArgs, pwzDirectory,
                nShowCmd);
            
            if (PathIsDirectoryW(pwzCommand))
            {
                hReturn = LSShellExecute(hOwner, pwzOperation, pwzCommand,
                    pwzArgs, NULL, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
            }
            else
            {
                SHELLEXECUTEINFOW seiCommand = { 0 };
                seiCommand.cbSize = sizeof(SHELLEXECUTEINFOW);
                seiCommand.hwnd = hOwner;
                seiCommand.lpVerb = pwzOperation;
                seiCommand.lpFile = pwzCommand;
                seiCommand.lpParameters = pwzArgs;
                seiCommand.lpDirectory = pwzDirectory;
                seiCommand.nShow = nShowCmd;
                seiCommand.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;
                
                LSShellExecuteEx(&seiCommand);
                
                hReturn = seiCommand.hInstApp;
            }
        }
    }
    
    return hReturn;
}


//
// LSExecuteExA
//
HINSTANCE LSExecuteExA(HWND hOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, int nShowCmd)
{
    return LSExecuteExW(
        hOwner,
        std::unique_ptr<wchar_t>(WCSFromMBS(pszOperation)).get(),
        std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get(),
        std::unique_ptr<wchar_t>(WCSFromMBS(pszArgs)).get(),
        std::unique_ptr<wchar_t>(WCSFromMBS(pszDirectory)).get(),
        nShowCmd);
}


//
// LSExecuteW
//
HINSTANCE LSExecuteW(HWND hOwner, LPCWSTR pwzCommand, int nShowCmd)
{
    wchar_t wzCommand[MAX_LINE_LENGTH];
    wchar_t wzExpandedCommand[MAX_LINE_LENGTH];
    LPCWSTR pwzArgs;
    HINSTANCE hResult = HINSTANCE(32);
    
    if (pwzCommand != nullptr)
    {
        VarExpansionExW(wzExpandedCommand, pwzCommand, MAX_LINE_LENGTH);
        
        if (GetTokenW(wzExpandedCommand, wzCommand, &pwzArgs, TRUE))
        {
            if (pwzArgs > (wzExpandedCommand + wcslen(wzExpandedCommand)))
            {
                pwzArgs = nullptr;
            }
            
            if (wzCommand[0] == L'!')
            {
                hResult = LSExecuteExW(hOwner, nullptr,
                    wzCommand, pwzArgs, nullptr, 0);
            }
            else
            {
                wchar_t wzDir[_MAX_DIR];
                wchar_t wzFullDir[_MAX_DIR + _MAX_DRIVE];
                
                _wsplitpath_s(wzCommand, wzFullDir, _countof(wzFullDir), wzDir, _countof(wzDir), nullptr, 0, nullptr, 0);
                StringCchCatW(wzFullDir, _MAX_DIR + _MAX_DRIVE, wzDir);
                
                hResult = LSExecuteExW(hOwner, NULL, wzCommand, pwzArgs,
                    wzFullDir, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
            }
        }
    }
    
    return hResult;
}


//
// LSExecuteA
//
HINSTANCE LSExecuteA(HWND hOwner, LPCSTR pszCommand, int nShowCmd)
{
    return LSExecuteW(hOwner, std::unique_ptr<wchar_t>(WCSFromMBS(pszCommand)).get(), nShowCmd);
}


//
// SetDesktopArea
//
void SetDesktopArea(int left, int top, int right, int bottom)
{
    RECT r = { left, top, right, bottom };
    
    SystemParametersInfo(SPI_SETWORKAREA, 0, (PVOID)&r, SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&r, SPIF_SENDCHANGE);
}


//
// GetLitestepWnd
//
HWND GetLitestepWnd()
{
    return g_LSAPIManager.GetLitestepWnd();
}


//
// GetResStrW
//
void GetResStrW(HINSTANCE hInstance, UINT uIDText, LPWSTR pszText, size_t cchText, LPCWSTR pszDefText)
{
    if (pszText != NULL && cchText > 0)
    {
        if (LoadString(hInstance, uIDText, pszText, (int)cchText) == 0)
        {
            StringCchCopy(pszText, cchText, pszDefText);
        }
    }
}


//
// GetResStrA
//
void GetResStrA(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText)
{
    if (pszText != nullptr && cchText > 0)
    {
        if (LoadStringA(hInstance, uIDText, pszText, (int)cchText) == 0)
        {
            StringCchCopyA(pszText, cchText, pszDefText);
        }
    }
}


//
// GetResStrExW
//
void GetResStrExW(HINSTANCE hInstance, UINT uIDText, LPWSTR pszText, size_t cchText, LPCWSTR pszDefText, ...)
{
    wchar_t szFormat[MAX_LINE_LENGTH];
    va_list vargs;
    
    if (pszText != NULL && cchText > 0)
    {
        GetResStrW(hInstance, uIDText, szFormat, MAX_LINE_LENGTH, pszDefText);
        
        va_start(vargs, pszDefText);
        StringCchVPrintf(pszText, cchText, szFormat, vargs);
        va_end(vargs);
    }
}


//
// GetResStrExA
//
void GetResStrExA(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...)
{
    char szFormat[MAX_LINE_LENGTH];
    va_list vargs;
    
    if (pszText != NULL && cchText > 0)
    {
        GetResStrA(hInstance, uIDText, szFormat, MAX_LINE_LENGTH, pszDefText);
        
        va_start(vargs, pszDefText);
        StringCchVPrintfA(pszText, cchText, szFormat, vargs);
        va_end(vargs);
    }
}


//
// LSGetLitestepPathW
//
BOOL WINAPI LSGetLitestepPathW(LPWSTR pwzPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pwzPath != nullptr && cchPath > 0)
    {
        // Default to user defined variable
        if (GetRCStringW(L"litestepdir", pwzPath, nullptr, (int)cchPath))
        {
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}


//
// LSGetLitestepPathA
//
BOOL WINAPI LSGetLitestepPathA(LPSTR pszPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pszPath != nullptr && cchPath > 0)
    {
        // Default to user defined variable
        if (GetRCStringA("litestepdir", pszPath, nullptr, (int)cchPath))
        {
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}


//
// LSGetImagePathW
//
BOOL WINAPI LSGetImagePathW(LPWSTR pwzPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pwzPath != nullptr && cchPath > 0)
    {
        if (GetRCStringW(L"LSImageFolder", pwzPath, nullptr, (int)cchPath))
        {
            bReturn = SUCCEEDED(PathAddBackslashEx(pwzPath, cchPath));
        }
        else
        {
            if (LSGetLitestepPathW(pwzPath, cchPath))
            {
                StringCchCat(pwzPath, cchPath, L"images\\");
                bReturn = TRUE;
            }
        }
    }
    
    return bReturn;
}


//
// LSGetImagePathA
//
BOOL WINAPI LSGetImagePathA(LPSTR pszPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pszPath != nullptr && cchPath > 0)
    {
        if (GetRCStringA("LSImageFolder", pszPath, nullptr, (int)cchPath))
        {
            bReturn = SUCCEEDED(PathAddBackslashExA(pszPath, cchPath));
        }
        else
        {
            if (LSGetLitestepPathA(pszPath, cchPath))
            {
                StringCchCatA(pszPath, cchPath, "images\\");
                bReturn = TRUE;
            }
        }
    }
    
    return bReturn;
}


//
// _Tokenize
//   (local helper function)
//
static int _Tokenize(LPCWSTR pwzString, LPWSTR* lpwzBuffers, DWORD dwNumBuffers, LPWSTR pwzExtraParameters, BOOL bUseBrackets)
{
    wchar_t wzBuffer[MAX_LINE_LENGTH];
    LPCWSTR pwzNextToken;
    DWORD dwTokens = 0;
    
    if (pwzString != nullptr)
    {
        pwzNextToken = pwzString;
        
        if ((lpwzBuffers != nullptr) && (dwNumBuffers > 0))
        {
            for (; pwzNextToken && dwTokens < dwNumBuffers; ++dwTokens)
            {
                GetTokenW(pwzNextToken, wzBuffer, &pwzNextToken, bUseBrackets);
                
                if (lpwzBuffers[dwTokens] != nullptr)
                {
                    StringCchCopyW(lpwzBuffers[dwTokens],
                        wcslen(wzBuffer) + 1, wzBuffer);
                }
            }
            
            for (DWORD dwClear = dwTokens; dwClear < dwNumBuffers; ++dwClear)
            {
                if (lpwzBuffers[dwClear] != nullptr)
                {
                    lpwzBuffers[dwClear][0] = L'\0';
                }
            }
            
            if (pwzExtraParameters != nullptr)
            {
                if (pwzNextToken)
                {
                    StringCchCopyW(pwzExtraParameters,
                        wcslen(pwzNextToken) + 1, pwzNextToken);
                }
                else
                {
                    pwzExtraParameters[0] = L'\0';
                }
            }
        }
        else
        {
            while (GetTokenW(pwzNextToken, nullptr, &pwzNextToken, bUseBrackets))
            {
                ++dwTokens;
            }
        }
    }
    
    return dwTokens;
}


//
// _Tokenize
//   (local helper function)
//
static int _Tokenize(LPCSTR pszString, LPSTR* lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters, BOOL bUseBrackets)
{
    char szBuffer[MAX_LINE_LENGTH];
    LPCSTR pszNextToken;
    DWORD dwTokens = 0;
    
    if (pszString != nullptr)
    {
        pszNextToken = pszString;
        
        if ((lpszBuffers != nullptr) && (dwNumBuffers > 0))
        {
            for (; pszNextToken && dwTokens < dwNumBuffers; ++dwTokens)
            {
                GetTokenA(pszNextToken, szBuffer, &pszNextToken, bUseBrackets);
                
                if (lpszBuffers[dwTokens] != nullptr)
                {
                    StringCchCopyA(lpszBuffers[dwTokens],
                        strlen(szBuffer) + 1, szBuffer);
                }
            }
            
            for (DWORD dwClear = dwTokens; dwClear < dwNumBuffers; ++dwClear)
            {
                if (lpszBuffers[dwClear] != nullptr)
                {
                    lpszBuffers[dwClear][0] = '\0';
                }
            }
            
            if (pszExtraParameters != nullptr)
            {
                if (pszNextToken)
                {
                    StringCchCopyA(pszExtraParameters,
                        strlen(pszNextToken) + 1, pszNextToken);
                }
                else
                {
                    pszExtraParameters[0] = '\0';
                }
            }
        }
        else
        {
            while (GetTokenA(pszNextToken, nullptr, &pszNextToken, bUseBrackets))
            {
                ++dwTokens;
            }
        }
    }
    
    return dwTokens;
}


//
// LCTokenizeW
//
int LCTokenizeW(LPCWSTR pwzString, LPWSTR *lpwzBuffers, DWORD dwNumBuffers, LPWSTR pwzExtraParameters)
{
    return _Tokenize(pwzString,
        lpwzBuffers, dwNumBuffers,
        pwzExtraParameters,
        FALSE);
}

//
// LCTokenizeW
//
int LCTokenizeA(LPCSTR pszString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters)
{
    return _Tokenize(pszString,
        lpszBuffers, dwNumBuffers,
        pszExtraParameters,
        FALSE);
}

//
// GetTokenW
//
BOOL GetTokenW(LPCWSTR pszString, LPWSTR pszToken, LPCWSTR* pszNextToken, BOOL bUseBrackets)
{
    LPCWSTR pszCurrent = pszString;
    LPCWSTR pszStartMarker = nullptr;
    int iBracketLevel = 0;
    WCHAR cQuote = L'\0';
    bool bIsToken = false;
    bool bAppendNextToken = false;
    
    if (pszString)
    {
        if (pszToken)
        {
            pszToken[0] = '\0';
        }
        
        if (pszNextToken)
        {
            *pszNextToken = nullptr;
        }
        
        pszCurrent += wcsspn(pszCurrent, WHITESPACEW);
        
        for (; *pszCurrent; ++pszCurrent)
        {
            if (iswspace((wint_t)*pszCurrent) && !cQuote)
            {
                break;
            }
            
            if (bUseBrackets && wcschr(L"[]", *pszCurrent) &&
                (!wcschr(L"\'\"", cQuote) || !cQuote))
            {
                if (*pszCurrent == L'[')
                {
                    if (bIsToken && !cQuote)
                    {
                        break;
                    }
                    
                    ++iBracketLevel;
                    cQuote = L'[';
                    
                    if (iBracketLevel == 1)
                    {
                        continue;
                    }
                }
                else
                {
                    --iBracketLevel;
                    
                    if (iBracketLevel <= 0)
                    {
                        break;
                    }
                }
            }
            
            if (wcschr(L"\'\"", *pszCurrent) && (cQuote != L'['))
            {
                if (!cQuote)
                {
                    if (bIsToken)
                    {
                        bAppendNextToken = true;
                        break;
                    }
                    
                    cQuote = *pszCurrent;
                    continue;
                }
                else if (*pszCurrent == cQuote)
                {
                    break;
                }
            }
            
            if (!bIsToken)
            {
                bIsToken = true;
                pszStartMarker = pszCurrent;
            }
        }
        
        if (pszStartMarker && pszToken)
        {
            wcsncpy(pszToken, pszStartMarker, pszCurrent - pszStartMarker);
            pszToken[pszCurrent - pszStartMarker] = L'\0';
        }
        
        if (!bAppendNextToken && *pszCurrent)
        {
            ++pszCurrent;
        }
        
        pszCurrent += wcsspn(pszCurrent, WHITESPACEW);
        
        if (*pszCurrent && pszNextToken)
        {
            *pszNextToken = pszCurrent;
        }
        
        if (bAppendNextToken && *pszCurrent)
        {
            LPWSTR pszNewToken = pszToken;
            
            if (pszNewToken)
            {
                pszNewToken += wcslen(pszToken);
            }
            
            GetTokenW(pszCurrent, pszNewToken, pszNextToken, bUseBrackets);
        }
        
        return pszStartMarker != nullptr;
    }
    
    return FALSE;
}


//
// GetTokenA
//
BOOL GetTokenA(LPCSTR pszString, LPSTR pszToken, LPCSTR* pszNextToken, BOOL bUseBrackets)
{
    LPCSTR pszCurrent = pszString;
    LPCSTR pszStartMarker = nullptr;
    int iBracketLevel = 0;
    CHAR cQuote = '\0';
    bool bIsToken = false;
    bool bAppendNextToken = false;
    
    if (pszString)
    {
        if (pszToken)
        {
            pszToken[0] = '\0';
        }
        
        if (pszNextToken)
        {
            *pszNextToken = nullptr;
        }
        
        pszCurrent += strspn(pszCurrent, WHITESPACEA);
        
        for (; *pszCurrent; ++pszCurrent)
        {
            if (isspace((unsigned char)*pszCurrent) && !cQuote)
            {
                break;
            }
            
            if (bUseBrackets && strchr("[]", *pszCurrent) &&
                (!strchr("\'\"", cQuote) || !cQuote))
            {
                if (*pszCurrent == '[')
                {
                    if (bIsToken && !cQuote)
                    {
                        break;
                    }
                    
                    ++iBracketLevel;
                    cQuote = '[';
                    
                    if (iBracketLevel == 1)
                    {
                        continue;
                    }
                }
                else
                {
                    --iBracketLevel;
                    
                    if (iBracketLevel <= 0)
                    {
                        break;
                    }
                }
            }
            
            if (strchr("\'\"", *pszCurrent) && (cQuote != '['))
            {
                if (!cQuote)
                {
                    if (bIsToken)
                    {
                        bAppendNextToken = true;
                        break;
                    }
                    
                    cQuote = *pszCurrent;
                    continue;
                }
                else if (*pszCurrent == cQuote)
                {
                    break;
                }
            }
            
            if (!bIsToken)
            {
                bIsToken = true;
                pszStartMarker = pszCurrent;
            }
        }
        
        if (pszStartMarker && pszToken)
        {
            //StringCchCopyNA(pszToken, cchToken, pszStartMarker, pszCurrent - pszStartMarker);
            strncpy(pszToken, pszStartMarker, pszCurrent - pszStartMarker);
            pszToken[pszCurrent - pszStartMarker] = '\0';
        }
        
        if (!bAppendNextToken && *pszCurrent)
        {
            ++pszCurrent;
        }
        
        pszCurrent += strspn(pszCurrent, WHITESPACEA);
        
        if (*pszCurrent && pszNextToken)
        {
            *pszNextToken = pszCurrent;
        }
        
        if (bAppendNextToken && *pszCurrent)
        {
            LPSTR pszNewToken = pszToken;
            
            if (pszNewToken)
            {
                pszNewToken += strlen(pszToken);
            }
            
            GetTokenA(pszCurrent, pszNewToken, pszNextToken, bUseBrackets);
        }
        
        return pszStartMarker != nullptr;
    }
    
    return FALSE;
}


//
// CommandTokenize
//
int CommandTokenizeW(LPCWSTR pwzString, LPWSTR *lpwzBuffers, DWORD dwNumBuffers, LPWSTR pwzExtraParameters)
{
    return _Tokenize(pwzString,
        lpwzBuffers, dwNumBuffers,
        pwzExtraParameters,
        TRUE);
}


//
// CommandTokenize
//
int CommandTokenizeA(LPCSTR pszString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters)
{
    return _Tokenize(pszString,
        lpszBuffers, dwNumBuffers,
        pszExtraParameters,
        TRUE);
}


//
// VarExpansionW
//
void VarExpansionW(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate)
{
    if (pwzExpandedString != nullptr && pwzTemplate != nullptr)
    {
        wchar_t wzTempBuffer[MAX_LINE_LENGTH];
        
        VarExpansionExW(wzTempBuffer, pwzTemplate, MAX_LINE_LENGTH);
        
        // bad mojo, but its a limitation of this old API
        StringCchCopyW(pwzExpandedString,
            wcslen(wzTempBuffer) + 1, wzTempBuffer);
    }
}


//
// VarExpansionA
//
void VarExpansionA(LPSTR pszExpandedString, LPCSTR pszTemplate)
{
    if (pszExpandedString != nullptr && pszTemplate != nullptr)
    {
        char szTempBuffer[MAX_LINE_LENGTH];
        
        VarExpansionExA(szTempBuffer, pszTemplate, MAX_LINE_LENGTH);
        
        // bad mojo, but its a limitation of this old API
        StringCchCopyA(pszExpandedString,
            strlen(szTempBuffer) + 1, szTempBuffer);
    }
}


//
// VarExpansionExW
//
void VarExpansionExW(LPWSTR pwzExpandedString, LPCWSTR pwzTemplate, size_t cchExpandedString)
{
    if (pwzExpandedString != nullptr &&
        cchExpandedString > 0 &&
        pwzTemplate != nullptr)
    {
        if (g_LSAPIManager.IsInitialized())
        {
            g_LSAPIManager.GetSettingsManager()->VarExpansionEx(
                pwzExpandedString, pwzTemplate, cchExpandedString);
        }
        else
        {
            StringCchCopyW(pwzExpandedString, cchExpandedString, pwzTemplate);
        }
    }
}


//
// VarExpansionExA
//
void VarExpansionExA(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchExpandedString)
{
    if (pszExpandedString != nullptr &&
        cchExpandedString > 0 &&
        pszTemplate != nullptr)
    {
        if (g_LSAPIManager.IsInitialized())
        {
            std::unique_ptr<wchar_t> temp(new wchar_t[cchExpandedString]);
            g_LSAPIManager.GetSettingsManager()->VarExpansionEx(
                temp.get(), MBSTOWCS(pszTemplate), cchExpandedString);
            WideCharToMultiByte(CP_ACP, 0, temp.get(), -1,
                pszExpandedString, (int)cchExpandedString, "?", nullptr);
        }
        else
        {
            StringCchCopyA(pszExpandedString, cchExpandedString, pszTemplate);
        }
    }
}


//
// ENUM_DATA
// Helper struct for EnumBangsThunk
//
typedef struct ENUM_DATA
{
    FARPROC fnCallback;
    LPARAM lParam;
} *LPENUM_DATA;


//
// EnumBangsThunk
// An ELD_BANGS_V2 thunk that translates to an ELD_BANGS-style callback
//
static BOOL CALLBACK EnumBangsThunk(HINSTANCE, LPCWSTR pwzBang, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMBANGSPROCW(pData->fnCallback)(pwzBang, pData->lParam);
}


//
// EnumLSDataW
//
// Return values:
//   E_INVALIDARG - Invalid value for uInfo
//   E_POINTER    - Invalid callback
//   E_FAIL       - Unspecified error
//   E_UNEXPECTED - Callback crashed or other catastrophic failure
//   S_OK         - Enumeration successful, callback always returned TRUE
//   S_FALSE      - Enumeration successful, but cancelled by callback
//
HRESULT EnumLSDataW(UINT uInfo, FARPROC pfnCallback, LPARAM lParam)
{
    HRESULT hr = E_INVALIDARG;
    
    if (NULL != pfnCallback)
    {
        switch (uInfo)
        {
        case ELD_BANGS:
            {
                //
                // Call EnumLSData recursively and let a small thunk handle
                // the translation from ELD_BANGS_V2 to ELD_BANGS
                //
                ENUM_DATA data = { 0 };
                data.fnCallback = pfnCallback;
                data.lParam = lParam;
                
                hr = EnumLSDataW(ELD_BANGS_V2,
                    (FARPROC)EnumBangsThunk, (LPARAM)&data);
            }
            break;
            
        case ELD_BANGS_V2:
            {
                hr = g_LSAPIManager.GetBangManager()->
                    EnumBangs((LSENUMBANGSV2PROCW)pfnCallback, lParam);
            }
            break;
            
        case ELD_REVIDS:
            {
                hr = (HRESULT)SendMessage(GetLitestepWnd(), LM_ENUMREVIDS,
                    (WPARAM)pfnCallback, lParam);
            }
            break;
            
        case ELD_MODULES:
            {
                hr = (HRESULT)SendMessage(GetLitestepWnd(), LM_ENUMMODULES,
                    (WPARAM)pfnCallback, lParam);
            }
            break;
            
        case ELD_PERFORMANCE:
            {
                hr = (HRESULT)SendMessage(GetLitestepWnd(), LM_ENUMPERFORMANCE,
                    (WPARAM)pfnCallback, lParam);
            }
            break;
            
        default:
            {
                // do nothing
            }
            break;
        }
    }
    else
    {
        hr = E_POINTER;
    }
    
    return hr;
}


//
// ANSII wrappers
//
static BOOL CALLBACK EnumLSDataBangsANSIIWrapper(LPCWSTR pwzBang, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMBANGSPROCA(pData->fnCallback)(std::unique_ptr<char>(MBSFromWCS(pwzBang)).get(), pData->lParam);
}
static BOOL CALLBACK EnumLSDataBangsV2ANSIIWrapper(HINSTANCE hInst, LPCWSTR pwzBang, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMBANGSV2PROCA(pData->fnCallback)(hInst, std::unique_ptr<char>(MBSFromWCS(pwzBang)).get(), pData->lParam);
}
static BOOL CALLBACK EnumLSDataRevIDsANSIIWrapper(LPCWSTR pwzRevID, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMREVIDSPROCA(pData->fnCallback)(std::unique_ptr<char>(MBSFromWCS(pwzRevID)).get(), pData->lParam);

}
static BOOL CALLBACK EnumLSDataModulesANSIIWrapper(LPCWSTR pwzModule, DWORD fdwFlags, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMMODULESPROCA(pData->fnCallback)(std::unique_ptr<char>(MBSFromWCS(pwzModule)).get(), fdwFlags, pData->lParam);

}
static BOOL CALLBACK EnumLSDataPerformanceANSIIWrapper(LPCWSTR pwzModule, DWORD dwLoadTime, LPARAM lParam)
{
    LPENUM_DATA pData = (LPENUM_DATA)lParam;
    return LSENUMPERFORMANCEPROCA(pData->fnCallback)(std::unique_ptr<char>(MBSFromWCS(pwzModule)).get(), dwLoadTime, pData->lParam);
}


//
// EnumLSDataA
//
// Return values:
//   E_INVALIDARG - Invalid value for uInfo
//   E_POINTER    - Invalid callback
//   E_FAIL       - Unspecified error
//   E_UNEXPECTED - Callback crashed or other catastrophic failure
//   S_OK         - Enumeration successful, callback always returned TRUE
//   S_FALSE      - Enumeration successful, but cancelled by callback
//
HRESULT EnumLSDataA(UINT uInfo, FARPROC pfnCallback, LPARAM lParam)
{
    HRESULT hr = E_INVALIDARG;

    if (nullptr != pfnCallback)
    {
        ENUM_DATA data;
        data.lParam = lParam;
        data.fnCallback = pfnCallback;

        pfnCallback = nullptr;

        switch (uInfo)
        {
        case ELD_BANGS:
            {
                pfnCallback = FARPROC(EnumLSDataBangsANSIIWrapper);
            }
            break;
            
        case ELD_BANGS_V2:
            {
                pfnCallback = FARPROC(EnumLSDataBangsV2ANSIIWrapper);
            }
            break;
            
        case ELD_REVIDS:
            {
                pfnCallback = FARPROC(EnumLSDataRevIDsANSIIWrapper);
            }
            break;
            
        case ELD_MODULES:
            {
                pfnCallback = FARPROC(EnumLSDataModulesANSIIWrapper);
            }
            break;
            
        case ELD_PERFORMANCE:
            {
                pfnCallback = FARPROC(EnumLSDataPerformanceANSIIWrapper);
            }
            break;
        }

        if (nullptr != pfnCallback)
        {
            hr = EnumLSDataW(uInfo, pfnCallback, (LPARAM)&data);
        }
    }
    else
    {
        hr = E_POINTER;
    }
    
    return hr;
}