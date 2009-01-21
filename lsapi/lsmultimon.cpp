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
/****************************************************************************
****************************************************************************/
#include "lsmultimon.h"

int (WINAPI* g_pfnGetSystemMetrics)(int) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromWindow)(HWND, DWORD) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromRect)(LPCRECT, DWORD) = NULL;
HMONITOR (WINAPI* g_pfnMonitorFromPoint)(POINT, DWORD) = NULL;
BOOL (WINAPI* g_pfnGetMonitorInfoA)(HMONITOR, LPMONITORINFO) = NULL;
BOOL (WINAPI* g_pfnEnumDisplayMonitors)(HDC, LPCRECT, MONITORENUMPROC, LPARAM) = NULL;
BOOL (WINAPI* g_pfnEnumDisplayDevicesA)(PVOID, DWORD, PDISPLAY_DEVICEA, DWORD) = NULL;

BOOL g_fMultiMonInitDone = FALSE;


BOOL InitMultipleMonitorStubs(void)
{
	if (g_fMultiMonInitDone)
	{
		return g_pfnGetMonitorInfoA != NULL;
	}

	HMODULE hUser32 = GetModuleHandle(_T("USER32"));

	if (hUser32 &&
	        (*(FARPROC*) & g_pfnGetSystemMetrics = GetProcAddress(hUser32, "GetSystemMetrics")) != NULL &&
	        (*(FARPROC*) & g_pfnMonitorFromWindow = GetProcAddress(hUser32, "MonitorFromWindow")) != NULL &&
	        (*(FARPROC*) & g_pfnMonitorFromRect = GetProcAddress(hUser32, "MonitorFromRect")) != NULL &&
	        (*(FARPROC*) & g_pfnMonitorFromPoint = GetProcAddress(hUser32, "MonitorFromPoint")) != NULL &&
	        (*(FARPROC*) & g_pfnEnumDisplayMonitors = GetProcAddress(hUser32, "EnumDisplayMonitors")) != NULL &&
	        (*(FARPROC*) & g_pfnGetMonitorInfoA = GetProcAddress(hUser32, "GetMonitorInfoA")) != NULL &&
	        (*(FARPROC*) & g_pfnEnumDisplayDevicesA = GetProcAddress(hUser32, "EnumDisplayDevicesA")) != NULL
	   )
	{
		g_fMultiMonInitDone = TRUE;
		return TRUE;
	}
	else
	{
		g_pfnGetSystemMetrics = NULL;
		g_pfnMonitorFromWindow = NULL;
		g_pfnMonitorFromRect = NULL;
		g_pfnMonitorFromPoint = NULL;
		g_pfnGetMonitorInfoA = NULL;
		g_pfnEnumDisplayMonitors = NULL;
		g_pfnEnumDisplayDevicesA = NULL;

		g_fMultiMonInitDone = TRUE;
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
//
// fake implementations of Monitor APIs that work with the primary display
// no special parameter validation is made since these run in client code
//
//-----------------------------------------------------------------------------

int LSGetSystemMetrics(int nIndex)
{
	if (InitMultipleMonitorStubs())
		return g_pfnGetSystemMetrics(nIndex);

	switch (nIndex)
	{
		case SM_CMONITORS:
		case SM_SAMEDISPLAYFORMAT:
		return 1;

		case SM_XVIRTUALSCREEN:
		case SM_YVIRTUALSCREEN:
		return 0;

		case SM_CXVIRTUALSCREEN:
		nIndex = SM_CXSCREEN;
		break;

		case SM_CYVIRTUALSCREEN:
		nIndex = SM_CYSCREEN;
		break;
	}

	return GetSystemMetrics(nIndex);
}

#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)

HMONITOR LSMonitorFromPoint(POINT ptScreenCoords, DWORD dwFlags)
{
	if (InitMultipleMonitorStubs())
		return g_pfnMonitorFromPoint(ptScreenCoords, dwFlags);

	if ((dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
	        ((ptScreenCoords.x >= 0) &&
	         (ptScreenCoords.x < GetSystemMetrics(SM_CXSCREEN)) &&
	         (ptScreenCoords.y >= 0) &&
	         (ptScreenCoords.y < GetSystemMetrics(SM_CYSCREEN))))
	{
		return xPRIMARY_MONITOR;
	}

	return NULL;
}

HMONITOR LSMonitorFromRect(LPCRECT lprcScreenCoords, DWORD dwFlags)
{
	if (InitMultipleMonitorStubs())
		return g_pfnMonitorFromRect(lprcScreenCoords, dwFlags);

	if (dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
	{
		return xPRIMARY_MONITOR;
	}

	RECT rScreen;
	rScreen.left = 0;
	rScreen.top = 0;
	rScreen.right = GetSystemMetrics(SM_CXSCREEN);
	rScreen.bottom = GetSystemMetrics(SM_CYSCREEN);

	RECT rDummy;
	if(IntersectRect(&rDummy, &rScreen, lprcScreenCoords))
	{
		return xPRIMARY_MONITOR;
	}

	return NULL;
}

HMONITOR LSMonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
	if (InitMultipleMonitorStubs())
		return g_pfnMonitorFromWindow(hWnd, dwFlags);

	if (dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
		return xPRIMARY_MONITOR;

	RECT rWnd = {0};

	if (IsIconic(hWnd))
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);

		if(GetWindowPlacement(hWnd, &wp))
		{
			// If the window does not have WS_EX_TOOLWINDOW set then the
			// coordinates are workspace coordinates and we must fix this.
			if (0 == (WS_EX_TOOLWINDOW & GetWindowLongPtr(hWnd, GWL_EXSTYLE)))
			{
				RECT rWA;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rWA, 0);
				OffsetRect(&wp.rcNormalPosition, rWA.left, rWA.top);
			}

			CopyRect(&rWnd, &wp.rcNormalPosition);
		}
	}
	else
	{
		GetWindowRect(hWnd, &rWnd);
	}

	return LSMonitorFromRect(&rWnd, dwFlags);
}

BOOL LSGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
	RECT rcWork;

	if (InitMultipleMonitorStubs())
	{
		return g_pfnGetMonitorInfoA(hMonitor, lpMonitorInfo);
	}

	if ((hMonitor == xPRIMARY_MONITOR) &&
	        lpMonitorInfo &&
	        (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
	        SystemParametersInfoA(SPI_GETWORKAREA, 0, &rcWork, 0))
	{
		lpMonitorInfo->rcMonitor.left = 0;
		lpMonitorInfo->rcMonitor.top = 0;
		lpMonitorInfo->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
		lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
		lpMonitorInfo->rcWork = rcWork;
		lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;

		if (lpMonitorInfo->cbSize == sizeof(MONITORINFOEXA))
		{
			StringCchCopyA(((MONITORINFOEXA*)lpMonitorInfo)->szDevice,
				CCHDEVICENAME, "DISPLAY");
		}
		else if (lpMonitorInfo->cbSize == sizeof(MONITORINFOEXW))
		{
			StringCchCopyW(((MONITORINFOEXW*)lpMonitorInfo)->szDevice,
				CCHDEVICENAME, L"DISPLAY");
		}

		return TRUE;
	}

	return FALSE;
}

BOOL LSEnumDisplayMonitors(
    HDC hdcOptionalForPainting,
    LPCRECT lprcEnumMonitorsThatIntersect,
    MONITORENUMPROC lpfnEnumProc,
    LPARAM dwData)
{
	RECT rcLimit;

	if (InitMultipleMonitorStubs())
	{
		return g_pfnEnumDisplayMonitors(
		           hdcOptionalForPainting,
		           lprcEnumMonitorsThatIntersect,
		           lpfnEnumProc,
		           dwData);
	}

	if (!lpfnEnumProc)
		return FALSE;

	rcLimit.left = 0;
	rcLimit.top = 0;
	rcLimit.right = GetSystemMetrics(SM_CXSCREEN);
	rcLimit.bottom = GetSystemMetrics(SM_CYSCREEN);

	if (hdcOptionalForPainting)
	{
		RECT rcClip;
		POINT ptOrg;

		switch (GetClipBox(hdcOptionalForPainting, &rcClip))
		{
			default:
			if (!GetDCOrgEx(hdcOptionalForPainting, &ptOrg))
				return FALSE;

			OffsetRect(&rcLimit, -ptOrg.x, -ptOrg.y);
			if (IntersectRect(&rcLimit, &rcLimit, &rcClip) &&
			        (!lprcEnumMonitorsThatIntersect ||
			         IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect)))
			{

				break;
			}
			//fall thru
			case NULLREGION:
			return TRUE;
			case ERROR:
			return FALSE;
		}
	}
	else
	{
		if (lprcEnumMonitorsThatIntersect &&
		        !IntersectRect(&rcLimit, &rcLimit, lprcEnumMonitorsThatIntersect))
		{

			return TRUE;
		}
	}

	return lpfnEnumProc(
	           xPRIMARY_MONITOR,
	           hdcOptionalForPainting,
	           &rcLimit,
	           dwData);
}

BOOL LSEnumDisplayDevices(
    PVOID Unused,
    DWORD iDevNum,
    PDISPLAY_DEVICEA lpDisplayDevice,
    DWORD dwFlags)
{
	if (InitMultipleMonitorStubs())
		return g_pfnEnumDisplayDevicesA(Unused, iDevNum, lpDisplayDevice, dwFlags);

	if (Unused != NULL)
		return FALSE;

	if (iDevNum != 0)
		return FALSE;

	if (lpDisplayDevice == NULL || lpDisplayDevice->cb != sizeof(DISPLAY_DEVICEA))
		return FALSE;

	StringCchCopyA(lpDisplayDevice->DeviceName, 32, "DISPLAY");
	StringCchCopyA(lpDisplayDevice->DeviceString, 128, "DISPLAY");

	lpDisplayDevice->StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP | DISPLAY_DEVICE_PRIMARY_DEVICE;

	return TRUE;
}
