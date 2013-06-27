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
#include "../utility/core.hpp"


BOOL WINAPI LSLog(int nLevel, LPCSTR pszModule, LPCSTR pszMessage)
{
#if defined(LS_COMPAT_LOGGING)
    char szLogFile[MAX_PATH] = { 0 };
    
    int nLogLevel = GetRCInt("LSLogLevel", 2);
    
    // Should this message be logged?
    if (!pszModule || !pszMessage ||
        (nLevel > nLogLevel) || (nLevel < 1) || (nLevel > 4))
    {
        return FALSE;
    }
    
    // Has a log file been assigned?
    if (!GetRCString("LSLogFile", szLogFile, NULL, MAX_PATH))
    {
        return FALSE;
    }
    
    // If so, open it
    HANDLE hLogFile = CreateFile(szLogFile,
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    // Did open succeed?
    if (hLogFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    
    // Move to the end of the file
    SetFilePointer(hLogFile, 0, NULL, FILE_END);
    
    // Get timestamp
    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);
    
    // Add timestamp and module name to message
    LPCSTR rszLevel[4] = { "Error", "Warning", "Notice", "Debug" };
    
    TCHAR szLine[MAX_LINE_LENGTH] = { 0 };
    size_t cbLine = sizeof(szLine);
    size_t cbRemaining = 0;
    
    if (SUCCEEDED(StringCbPrintfEx(szLine, cbLine, NULL, &cbRemaining,
        STRSAFE_IGNORE_NULLS, "%02d-%02d-%04d %02d:%02d:%02d - %s - %s: %s\r\n",
        st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond,
        rszLevel[nLevel-1], pszModule, pszMessage)))
    {
        size_t cbToWrite = cbLine - cbRemaining;
        ASSERT(cbToWrite <= MAXDWORD);

        // Write it to the log file
        DWORD dwCount = 0;
        WriteFile(hLogFile, szLine, (DWORD)cbToWrite, &dwCount, NULL);
    }
    
    // Close the log
    CloseHandle(hLogFile);
#endif // LS_COMPAT_LOGGING
    
    return TRUE;
}

BOOL WINAPIV LSLogPrintf(int nLevel, LPCSTR pszModule, LPCSTR pszFormat, ...)
{
#if defined(LS_COMPAT_LOGGING)
    if (!pszModule || !pszFormat)
    {
        return FALSE;
    }
    
    BOOL bReturn = FALSE;
    char szMessage[MAX_LINE_LENGTH];
    
    va_list argList;
    va_start(argList, pszFormat);
    
    if (SUCCEEDED(StringCchVPrintf(szMessage, MAX_LINE_LENGTH,
        pszFormat, argList)))
    {
        bReturn = LSLog(nLevel, pszModule, szMessage);
    }
    
    va_end(argList);
    
    return bReturn;
#else
    return TRUE;
#endif // LS_COMPAT_LOGGING
}

int GetRCCoordinate(LPCSTR pszKeyName, int nDefault, int nMaxVal)
{
    char strVal[MAX_LINE_LENGTH];
    
    if (!GetRCString(pszKeyName, strVal, NULL, MAX_LINE_LENGTH))
    {
        return nDefault;
    }
    
    return ParseCoordinate(strVal, nDefault, nMaxVal);
}

int ParseCoordinate(LPCSTR szString, int nDefault, int nMaxVal)
{
    BOOL negative = false;
    BOOL center = false;
    BOOL percentual = false;
    
    int value = 0;
    
    if (!szString || !szString[0])
    {
        return nDefault;
    }
    
    if (szString[0] == '-')
    {
        negative = true;
        ++szString;
    }
    else if (szString[0] == '+')
    {
        ++szString;
    }
    
    size_t length = strlen(szString);
    size_t i = 0;
    
    while (i < length)
    {
        if (szString[i] < '0' || szString[i] > '9')
        {
            break;
        }
        
        value = (value * 10) + (szString[i] - '0');
        
        ++i;
    }
    
    if (i == 0)
    {
        return nDefault;
    }
    
    while (i < length)
    {
        if ((szString[i] == 'c' || szString[i] == 'C') && !center)
        {
            center = true;
        }
        else if (szString[i] == '%' && !percentual)
        {
            percentual = true;
        }
        else
        {
            break;
        }
        
        ++i;
    }
    
    if (percentual)
    {
        value = nMaxVal * value / 100;
    }
    
    if (center)
    {
        if (negative)
        {
            value = (nMaxVal / 2) - value;
        }
        else
        {
            value = (nMaxVal / 2) + value;
        }
    }
    else if (negative)
    {
        value = nMaxVal - value;
    }
    
    return value;
}

int LSGetSystemMetrics(int nIndex)
{
    return GetSystemMetrics(nIndex);
}

HMONITOR LSMonitorFromPoint(POINT pt, DWORD dwFlags)
{
    return MonitorFromPoint(pt, dwFlags);
}

HMONITOR LSMonitorFromRect(LPCRECT lprc, DWORD dwFlags)
{
    return MonitorFromRect(lprc, dwFlags);
}

HMONITOR LSMonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
    return MonitorFromWindow(hwnd, dwFlags);
}

BOOL LSGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
    return GetMonitorInfoA(hMonitor, lpmi);
}

BOOL LSEnumDisplayMonitors(
    HDC hdc,
    LPCRECT lprcClip,
    MONITORENUMPROC lpfnEnum,
    LPARAM dwData)
{
    return EnumDisplayMonitors(hdc, lprcClip, lpfnEnum, dwData);
}

BOOL LSEnumDisplayDevices(
    LPCSTR lpDevice,
    DWORD iDevNum,
    PDISPLAY_DEVICEA lpDisplayDevice,
    DWORD dwFlags)
{
    return EnumDisplayDevicesA(lpDevice, iDevNum, lpDisplayDevice, dwFlags);
}
