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

BOOL LSAPIInitialize(LPCSTR pszLitestepPath, LPCSTR pszRcPath)
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


template<typename T>
BOOL AddBangCommandWorker(LPCSTR pszCommand, T pfnBangCommand)
{
    BOOL bReturn = FALSE;
    
    if (pszCommand != NULL && pfnBangCommand != NULL)
    {
        DWORD dwCurrentThreadID = GetCurrentThreadId();
        
        Bang* pBang = new Bang(dwCurrentThreadID, pfnBangCommand, pszCommand);
        
        if (pBang != NULL)
        {
            //bBang->AddRef();
            g_LSAPIManager.GetBangManager()->AddBangCommand(pszCommand, pBang);
            pBang->Release();
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}


//
// AddBangCommand
//
BOOL AddBangCommand(LPCSTR pszCommand, BangCommand pfnBangCommand)
{
    return AddBangCommandWorker(pszCommand, pfnBangCommand);
}


//
// AddBangCommandEx
//
BOOL AddBangCommandEx(LPCSTR pszCommand, BangCommandEx pfnBangCommand)
{
    return AddBangCommandWorker(pszCommand, pfnBangCommand);
}


//
// RemoveBangCommand(LPCSTR pszCommand)
//
BOOL RemoveBangCommand(LPCSTR pszCommand)
{
    BOOL bResult = FALSE;
    
    if (pszCommand != NULL)
    {
        bResult = \
            g_LSAPIManager.GetBangManager()->RemoveBangCommand(pszCommand);
    }
    
    return bResult;
}


//
// InternalExecuteBangCommand
//   (Just like ParseBangCommand but without the variable expansion)
//
BOOL InternalExecuteBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
{
    return g_LSAPIManager.GetBangManager()->
        ExecuteBangCommand(pszCommand, hCaller, pszArgs);
}


//
// ParseBangCommand
//
BOOL ParseBangCommand(HWND hCaller, LPCSTR pszCommand, LPCSTR pszArgs)
{
    TRACE("ParseBangCommand(%.8X, \"%s\", \"%s\");",
        hCaller, pszCommand, pszArgs);
    
    char szExpandedArgs[MAX_LINE_LENGTH] = { 0 };
    BOOL bReturn = FALSE;
    
    if (pszCommand != NULL)
    {
        if (pszArgs != NULL)
        {
            VarExpansionEx(szExpandedArgs, pszArgs, MAX_LINE_LENGTH);
        }
        
        bReturn = \
            InternalExecuteBangCommand(hCaller, pszCommand, szExpandedArgs);
    }
    
    return bReturn;
}


//
// CommandParse
//
void CommandParse(LPCSTR pszCommand, LPSTR pszOutCommand, LPSTR pszOutArgs, size_t cchOutCommand, size_t cchOutArgs)
{
    char szCommand[MAX_LINE_LENGTH];
    char szTempCommand[MAX_LINE_LENGTH];
    LPCSTR pszTempArgs = NULL;
    
    if (pszCommand != NULL)
    {
        if (pszOutCommand != NULL && cchOutCommand > 0)
        {
            VarExpansionEx(szCommand, pszCommand, MAX_LINE_LENGTH);
            
            GetToken(szCommand, szTempCommand, &pszTempArgs, TRUE);
            
            StringCchCopy(pszOutCommand, cchOutCommand, szTempCommand);
        }
        
        if (pszOutArgs != NULL && cchOutArgs > 0)
        {
            StringCchCopy(pszOutArgs, cchOutArgs, pszTempArgs);
        }
    }
}


//
// LSExecuteEx
//
HINSTANCE LSExecuteEx(HWND hOwner, LPCSTR pszOperation, LPCSTR pszCommand, LPCSTR pszArgs, LPCSTR pszDirectory, int nShowCmd)
{
    HINSTANCE hReturn = HINSTANCE(32);
    
    if (pszCommand != NULL)
    {
        if (pszCommand[0] == '!')
        {
            hReturn = ParseBangCommand(hOwner, pszCommand, pszArgs) ?
                HINSTANCE(33) : HINSTANCE(32);
        }
        else
        {
            TRACE("LSExecuteEx(%.8X, \"%s\", \"%s\", \"%s\", \"%s\", %d);",
                hOwner, pszOperation, pszCommand, pszArgs,pszDirectory,
                nShowCmd);
            
            if (PathIsDirectory(pszCommand))
            {
                hReturn = LSShellExecute(hOwner, pszOperation, pszCommand,
                    pszArgs, NULL, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
            }
            else
            {
                SHELLEXECUTEINFO seiCommand = { 0 };
                seiCommand.cbSize = sizeof(SHELLEXECUTEINFO);
                seiCommand.hwnd = hOwner;
                seiCommand.lpVerb = pszOperation;
                seiCommand.lpFile = pszCommand;
                seiCommand.lpParameters = pszArgs;
                seiCommand.lpDirectory = pszDirectory;
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
// LSExecute
//
HINSTANCE LSExecute(HWND hOwner, LPCSTR pszCommand, int nShowCmd)
{
    char szCommand[MAX_LINE_LENGTH];
    char szExpandedCommand[MAX_LINE_LENGTH];
    LPCSTR pszArgs;
    HINSTANCE hResult = HINSTANCE(32);
    
    if (pszCommand != NULL)
    {
        VarExpansionEx(szExpandedCommand, pszCommand, MAX_LINE_LENGTH);
        
        if (GetToken(szExpandedCommand, szCommand, &pszArgs, TRUE))
        {
            if (pszArgs > (szExpandedCommand + strlen(szExpandedCommand)))
            {
                pszArgs = NULL;
            }
            
            if (szCommand[0] == '!')
            {
                hResult = LSExecuteEx(hOwner, NULL,
                    szCommand, pszArgs, NULL, 0);
            }
            else
            {
                char szDir[_MAX_DIR];
                char szFullDir[_MAX_DIR + _MAX_DRIVE];
                
                _splitpath(szCommand, szFullDir, szDir, NULL, NULL);
                StringCchCat(szFullDir, _MAX_DIR + _MAX_DRIVE, szDir);
                
                hResult = LSExecuteEx(hOwner, NULL, szCommand, pszArgs,
                    szFullDir, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
            }
        }
    }
    
    return hResult;
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
//    GetLitestepWnd
//
HWND GetLitestepWnd()
{
    return g_LSAPIManager.GetLitestepWnd();
}


//
// void GetResStr
//
void GetResStr(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText)
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
// GetResStrEx
//
void GetResStrEx(HINSTANCE hInstance, UINT uIDText, LPSTR pszText, size_t cchText, LPCSTR pszDefText, ...)
{
    char szFormat[MAX_LINE_LENGTH];
    va_list vargs;
    
    if (pszText != NULL && cchText > 0)
    {
        GetResStr(hInstance, uIDText, szFormat, MAX_LINE_LENGTH, pszDefText);
        
        va_start(vargs, pszDefText);
        StringCchVPrintf(pszText, cchText, szFormat, vargs);
        va_end(vargs);
    }
}


//
// LSGetLitestepPath
//
BOOL WINAPI LSGetLitestepPath(LPSTR pszPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pszPath != NULL && cchPath > 0)
    {
        // Default to user defined variable
        if (GetRCString("litestepdir", pszPath, NULL, (int)cchPath))
        {
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}


//
// LSGetImagePath
//
BOOL WINAPI LSGetImagePath(LPSTR pszPath, size_t cchPath)
{
    BOOL bReturn = FALSE;
    
    if (pszPath != NULL && cchPath > 0)
    {
        if (GetRCString("LSImageFolder", pszPath, NULL, (int)cchPath))
        {
            bReturn = SUCCEEDED(PathAddBackslashEx(pszPath, cchPath));
        }
        else
        {
            if (LSGetLitestepPath(pszPath, cchPath))
            {
                StringCchCat(pszPath, cchPath, "images\\");
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
static int _Tokenize(LPCSTR pszString, LPSTR* lpszBuffers, DWORD dwNumBuffers, LPSTR pszExtraParameters, BOOL bUseBrackets)
{
    char szBuffer[MAX_LINE_LENGTH];
    LPCSTR pszNextToken;
    DWORD dwTokens = 0;
    
    if (pszString != NULL)
    {
        pszNextToken = pszString;
        
        if ((lpszBuffers != NULL) && (dwNumBuffers > 0))
        {
            for (; pszNextToken && dwTokens < dwNumBuffers; ++dwTokens)
            {
                GetToken(pszNextToken, szBuffer, &pszNextToken, bUseBrackets);
                
                if (lpszBuffers[dwTokens] != NULL)
                {
                    StringCchCopy(lpszBuffers[dwTokens],
                        strlen(szBuffer) + 1, szBuffer);
                }
            }
            
            for (DWORD dwClear = dwTokens; dwClear < dwNumBuffers; ++dwClear)
            {
                if (lpszBuffers[dwClear] != NULL)
                {
                    lpszBuffers[dwClear][0] = '\0';
                }
            }
            
            if (pszExtraParameters != NULL)
            {
                if (pszNextToken)
                {
                    StringCchCopy(pszExtraParameters,
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
            while (GetToken(pszNextToken, NULL, &pszNextToken, bUseBrackets))
            {
                ++dwTokens;
            }
        }
    }
    
    return dwTokens;
}


//
// LCTokenize
//
int LCTokenize(LPCSTR szString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters)
{
    return _Tokenize(szString,
        lpszBuffers, dwNumBuffers,
        szExtraParameters,
        FALSE);
}


//
// GetToken
//
BOOL GetToken(LPCSTR pszString, LPSTR pszToken, LPCSTR* pszNextToken, BOOL bUseBrackets)
{
    LPCSTR pszCurrent = pszString;
    LPCSTR pszStartMarker = NULL;
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
            *pszNextToken = NULL;
        }
        
        pszCurrent += strspn(pszCurrent, WHITESPACE);
        
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
            strncpy(pszToken, pszStartMarker, pszCurrent - pszStartMarker);
            pszToken[pszCurrent - pszStartMarker] = '\0';
        }
        
        if (!bAppendNextToken && *pszCurrent)
        {
            ++pszCurrent;
        }
        
        pszCurrent += strspn(pszCurrent, WHITESPACE);
        
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
            
            GetToken(pszCurrent, pszNewToken, pszNextToken, bUseBrackets);
        }
        
        return pszStartMarker != NULL;
    }
    
    return FALSE;
}


//
// CommandTokenize
//
int CommandTokenize(LPCSTR szString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters)
{
    return _Tokenize(szString,
        lpszBuffers, dwNumBuffers,
        szExtraParameters,
        TRUE);
}


//
// VarExpansion
//
void VarExpansion(LPSTR pszExpandedString, LPCSTR pszTemplate)
{
    if (pszExpandedString != NULL && pszTemplate != NULL)
    {
        char szTempBuffer[MAX_LINE_LENGTH];
        
        VarExpansionEx(szTempBuffer, pszTemplate, MAX_LINE_LENGTH);
        
        // bad mojo, but its a limitation of this old API
        StringCchCopy(pszExpandedString,
            strlen(szTempBuffer) + 1, szTempBuffer);
    }
}


//
// VarExpansionEx
//
void VarExpansionEx(LPSTR pszExpandedString, LPCSTR pszTemplate, size_t cchExpandedString)
{
    if (pszExpandedString != NULL &&
        cchExpandedString > 0 &&
        pszTemplate != NULL)
    {
        if (g_LSAPIManager.IsInitialized())
        {
            g_LSAPIManager.GetSettingsManager()->VarExpansionEx(
                pszExpandedString, pszTemplate, cchExpandedString);
        }
        else
        {
            StringCchCopy(pszExpandedString, cchExpandedString, pszTemplate);
        }
    }
}


//
// ENUMBANG_DATA
// Helper struct for EnumBangsThunk
//
struct ENUMBANG_DATA
{
    LSENUMBANGSPROC fnCallback;
    LPARAM lParam;
};


//
// EnumBangsThunk
// An ELD_BANGS_V2 thunk that translates to an ELD_BANGS-style callback
//
static BOOL CALLBACK EnumBangsThunk(HINSTANCE, LPCSTR pszBang, LPARAM lParam)
{
    ENUMBANG_DATA* pData = (ENUMBANG_DATA*)lParam;
    return pData->fnCallback(pszBang, pData->lParam);
}


//
// EnumLSData
//
// Return values:
//   E_INVALIDARG - Invalid value for uInfo
//   E_POINTER    - Invalid callback
//   E_FAIL       - Unspecified error
//   E_UNEXPECTED - Callback crashed or other catastrophic failure
//   S_OK         - Enumeration successful, callback always returned TRUE
//   S_FALSE      - Enumeration successful, but cancelled by callback
//
HRESULT EnumLSData(UINT uInfo, FARPROC pfnCallback, LPARAM lParam)
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
                ENUMBANG_DATA data = { 0 };
                data.fnCallback = (LSENUMBANGSPROC)pfnCallback;
                data.lParam = lParam;
                
                hr = EnumLSData(ELD_BANGS_V2,
                    (FARPROC)EnumBangsThunk, (LPARAM)&data);
            }
            break;
            
        case ELD_BANGS_V2:
            {
                hr = g_LSAPIManager.GetBangManager()->
                    EnumBangs((LSENUMBANGSV2PROC)pfnCallback, lParam);
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
