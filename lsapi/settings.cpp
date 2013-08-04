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
#include "settingsmanager.h"
#include "lsapiInit.h"
#include "../utility/core.hpp"
#include "../utility/stringutility.h"


LPVOID LCOpenW(LPCWSTR pwzPath)
{
    LPVOID pFile = nullptr;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pwzPath)
        {
            if (pwzPath[0] != L'\0')
            {
                pFile = g_LSAPIManager.GetSettingsManager()->LCOpen(pwzPath);
            }
        }
        if (pFile == nullptr)
        {
            pFile = g_LSAPIManager.GetSettingsManager()->LCOpen(nullptr);
        }
    }
    
    return pFile;
}

LPVOID LCOpenA(LPCSTR pszPath)
{
    return LCOpenW(std::unique_ptr<wchar_t>(WCSFromMBS(pszPath)).get());
}


BOOL LCClose(LPVOID pFile)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr)
        {
            bReturn = g_LSAPIManager.GetSettingsManager()->LCClose(pFile);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextCommandW(LPVOID pFile, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pwzValue != nullptr && cchValue > 0)
        {
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextCommand(
                pFile, pwzValue, cchValue);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextCommandA(LPVOID pFile, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pszValue != nullptr && cchValue > 0)
        {
            std::unique_ptr<wchar_t> temp(new wchar_t[cchValue]);
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextCommand(
                pFile, temp.get(), cchValue);
            WideCharToMultiByte(CP_ACP, 0, temp.get(), (int)cchValue, pszValue, (int)cchValue, "?", nullptr);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextConfigW(LPVOID pFile, LPCWSTR pwzConfig, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pwzConfig != nullptr &&
            pwzValue != nullptr && cchValue > 0)
        {
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextConfig(
                pFile, pwzConfig, pwzValue, cchValue);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextConfigA(LPVOID pFile, LPCSTR pszConfig, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pszConfig != nullptr &&
            pszValue != nullptr && cchValue > 0)
        {
            std::unique_ptr<wchar_t> temp(new wchar_t[cchValue]);
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextConfig(
                pFile, MBSTOWCS(pszConfig), temp.get(), cchValue);
            WideCharToMultiByte(CP_ACP, 0, temp.get(), (int)cchValue, pszValue, (int)cchValue, "?", nullptr);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextLineW(LPVOID pFile, LPWSTR pwzValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pwzValue != nullptr && cchValue > 0)
        {
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextLine(
                pFile, pwzValue, cchValue);
        }
    }
    
    return bReturn;
}


BOOL LCReadNextLineA(LPVOID pFile, LPSTR pszValue, size_t cchValue)
{
    BOOL bReturn = FALSE;
    
    if (g_LSAPIManager.IsInitialized())
    {
        if (pFile != nullptr && pszValue != nullptr && cchValue > 0)
        {
            std::unique_ptr<wchar_t> value(new wchar_t[cchValue]);
            bReturn = g_LSAPIManager.GetSettingsManager()->LCReadNextLine(
                pFile, value.get(), cchValue);
            WideCharToMultiByte(CP_ACP, 0, value.get(), (int)cchValue, pszValue, (int)cchValue, "?", nullptr);
        }
    }
    
    return bReturn;
}


__int64 GetRCInt64W(LPCWSTR pwzKeyName, __int64 nDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCInt64(
            pwzKeyName, nDefault);
    }
    
    return nDefault;
}


__int64 GetRCInt64A(LPCSTR pszKeyName, __int64 nDefault)
{
    return GetRCInt64W(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), nDefault);
}


int GetRCIntW(LPCWSTR pwzKeyName, int nDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCInt(
            pwzKeyName, nDefault);
    }
    
    return nDefault;
}


int GetRCIntA(LPCSTR pszKeyName, int nDefault)
{
    return GetRCIntW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), nDefault);
}


float GetRCFloatW(LPCWSTR pwzKeyName, float fDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCFloat(
            pwzKeyName, fDefault);
    }
    
    return fDefault;
}


float GetRCFloatA(LPCSTR pszKeyName, float fDefault)
{
    return GetRCFloatW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), fDefault);
}


double GetRCDoubleW(LPCWSTR pwzKeyName, double dDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCDouble(
            pwzKeyName, dDefault);
    }
    
    return dDefault;
}


double GetRCDoubleA(LPCSTR pszKeyName, double dDefault)
{
    return GetRCDoubleW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), dDefault);
}


BOOL GetRCBoolW(LPCWSTR pwzKeyName, BOOL ifFound)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCBool(
            pwzKeyName, ifFound);
    }
    
    return !ifFound;
}


BOOL GetRCBoolA(LPCSTR pszKeyName, BOOL ifFound)
{
    return GetRCBoolW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), ifFound);
}


BOOL GetRCBoolDefW(LPCWSTR pwzKeyName, BOOL bDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCBoolDef(
            pwzKeyName, bDefault);
    }
    
    return bDefault;
}


BOOL GetRCBoolDefA(LPCSTR pszKeyName, BOOL bDefault)
{
    return GetRCBoolDefW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), bDefault);
}


BOOL GetRCStringW(LPCWSTR pwzKeyName, LPWSTR pwzValue, LPCWSTR pwzDefStr, int maxLen)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCString(
            pwzKeyName, pwzValue, pwzDefStr, maxLen);
    }
    else if (pwzValue && pwzDefStr)
    {
        StringCchCopyW(pwzValue, maxLen, pwzDefStr);
    }
    
    return FALSE;
}


BOOL GetRCStringA(LPCSTR pszKeyName, LPSTR pszValue, LPCSTR pszDefStr, int maxLen)
{
    if (g_LSAPIManager.IsInitialized())
    {
        std::unique_ptr<wchar_t> tempValue(new wchar_t[maxLen]);
        std::unique_ptr<wchar_t> key (WCSFromMBS(pszKeyName));
        std::unique_ptr<wchar_t> def (WCSFromMBS(pszDefStr));

        *tempValue = L'\0';

        BOOL bRet = g_LSAPIManager.GetSettingsManager()->GetRCString(
            key.get(), tempValue.get(), def.get(), maxLen);

        if (pszValue)
        {
            WideCharToMultiByte(CP_ACP, 0, tempValue.get(), maxLen, pszValue, maxLen, "?", nullptr);
        }

        return bRet;
    }
    else if (pszValue && pszDefStr)
    {
        StringCchCopyA(pszValue, maxLen, pszDefStr);
    }
    
    return FALSE;
}


COLORREF GetRCColorW(LPCWSTR pwzKeyName, COLORREF colDef)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCColor(
            pwzKeyName, colDef);
    }
    
    return colDef;
}


COLORREF GetRCColorA(LPCSTR pszKeyName, COLORREF colDef)
{
    return GetRCColorW(std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(), colDef);
}


BOOL GetRCLineW(LPCWSTR pwzKeyName, LPWSTR pwzBuffer, UINT nBufLen, LPCWSTR pwzDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetRCLine(
            pwzKeyName, pwzBuffer, nBufLen, pwzDefault);
    }
    else if (pwzBuffer && pwzDefault)
    {
        StringCchCopyW(pwzBuffer, nBufLen, pwzDefault);
    }
    
    return FALSE;
}


BOOL GetRCLineA(LPCSTR pszKeyName, LPSTR pszBuffer, UINT nBufLen, LPCSTR pszDefault)
{
    if (g_LSAPIManager.IsInitialized())
    {
        std::unique_ptr<wchar_t> tempValue(new wchar_t[nBufLen]);
        std::unique_ptr<wchar_t> key (WCSFromMBS(pszKeyName));
        std::unique_ptr<wchar_t> def (WCSFromMBS(pszDefault));

        *tempValue = L'\0';

        BOOL bRet = g_LSAPIManager.GetSettingsManager()->GetRCLine(
            key.get(), tempValue.get(), nBufLen, def.get());

        if (pszBuffer)
        {
            WideCharToMultiByte(CP_ACP, 0, tempValue.get(), nBufLen, pszBuffer, nBufLen, "?", nullptr);
        }

        return bRet;
    }
    else if (pszBuffer && pszDefault)
    {
        StringCchCopyA(pszBuffer, nBufLen, pszDefault);
    }
    
    return FALSE;
}


BOOL LSGetVariableExW(LPCWSTR pszKeyName, LPWSTR pszValue, DWORD dwLength)
{
    if (g_LSAPIManager.IsInitialized())
    {
        return g_LSAPIManager.GetSettingsManager()->GetVariable(
            pszKeyName, pszValue, dwLength);
    }
    
    return FALSE;
}


BOOL LSGetVariableExA(LPCSTR pszKeyName, LPSTR pszValue, DWORD dwLength)
{
    if (g_LSAPIManager.IsInitialized())
    {
        std::unique_ptr<wchar_t> temp(new wchar_t[dwLength]);

        BOOL bRet = g_LSAPIManager.GetSettingsManager()->GetVariable(
            MBSTOWCS(pszKeyName), temp.get(), dwLength);

        WideCharToMultiByte(CP_ACP, 0, temp.get(), dwLength, pszValue, dwLength, "?", nullptr);

        return bRet;
    }
    
    return FALSE;
}


BOOL LSGetVariableW(LPCWSTR pszKeyName, LPWSTR pszValue)
{
    BOOL bReturn = FALSE;
    wchar_t szTempValue[MAX_LINE_LENGTH];
    
    if (pszValue)
    {
        pszValue[0] = L'\0';
    }
    
    if (g_LSAPIManager.IsInitialized())
    {
        bReturn = g_LSAPIManager.GetSettingsManager()->GetVariable(
            pszKeyName, szTempValue, MAX_LINE_LENGTH);
        
        if (bReturn && pszValue)
        {
            StringCchCopy(pszValue, wcslen(szTempValue) + 1, szTempValue);
        }
    }
    
    return bReturn;
}


BOOL LSGetVariableA(LPCSTR pszKeyName, LPSTR pszValue)
{
    BOOL bReturn = FALSE;
    wchar_t szTempValue[MAX_LINE_LENGTH];
    
    if (pszValue)
    {
        pszValue[0] = L'\0';
    }
    
    if (g_LSAPIManager.IsInitialized())
    {
        bReturn = g_LSAPIManager.GetSettingsManager()->GetVariable(
            MBSTOWCS(pszKeyName), szTempValue, MAX_LINE_LENGTH);
        
        if (bReturn && pszValue)
        {
            StringCchCopyA(pszValue, _countof(szTempValue), WCSTOMBS(szTempValue));
        }
    }
    
    return bReturn;
}


void LSSetVariableW(LPCWSTR pszKeyName, LPCWSTR pszValue)
{
    if (g_LSAPIManager.IsInitialized())
    {
        g_LSAPIManager.GetSettingsManager()->SetVariable(pszKeyName, pszValue);
    }
}


void LSSetVariableA(LPCSTR pszKeyName, LPCSTR pszValue)
{
    LSSetVariableW(
        std::unique_ptr<wchar_t>(WCSFromMBS(pszKeyName)).get(),
        std::unique_ptr<wchar_t>(WCSFromMBS(pszValue)).get()
        );
}
