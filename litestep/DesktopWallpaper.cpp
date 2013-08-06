//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1998 (e)
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
#include "../utility/common.h"
#include "DesktopWallpaper.h"
#include <Shlwapi.h>
#include "../utility/shellhlp.h"


//
// DesktopWallpaper
//
DesktopWallpaper::DesktopWallpaper()
{
    m_uRefCount = 1;

    monitors.push_back(L"\\\\?Monitor1");
    monitors.push_back(L"\\\\?Monitor2");
    monitors.push_back(L"\\\\?Monitor3");
}


//
// ~DesktopWallpaper
//
DesktopWallpaper::~DesktopWallpaper()
{
}


//
// IUnknown::AddRef
// Increments the reference count for an interface on an object.
//
ULONG DesktopWallpaper::AddRef()
{
    return InterlockedIncrement(&m_uRefCount);
}


//
// IUnknown::Release
// Decrements the reference count for an interface on an object.
//
ULONG DesktopWallpaper::Release()
{
    if (InterlockedDecrement(&m_uRefCount) == 0)
    {
        delete this;
        return 0;
    }

    return m_uRefCount;
}


//
// IUnknown::QueryInterface
// Retrieves pointers to the supported interfaces on an object.
//
HRESULT DesktopWallpaper::QueryInterface(REFIID riid, void **ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }

    if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)this;
    }
    else if (riid == IID_IDesktopWallpaper)
    {
        *ppvObject = (IDesktopWallpaper*)this;
    }
    else if (riid == IID_IDesktopWallpaperPrivate)
    {
        *ppvObject = (IDesktopWallpaperPrivate*)this;
    }
    else
    {
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


//
// IDesktopWallpaper::SetWallpaper
// Sets the desktop wallpaper for the specified monitor.
//
HRESULT DesktopWallpaper::SetWallpaper(LPCWSTR pwzMonitorID, LPCWSTR pwzWallpaper)
{
    // TODO::Handle the case where this is not null
    if (pwzMonitorID != nullptr)
    {
    }

    SHSetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", REG_SZ,
        pwzWallpaper, (DWORD)wcslen(pwzWallpaper)*sizeof(pwzWallpaper[0]));
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);

    return S_OK;
}


//
// IDesktopWallpaper::GetWallpaper
// Retrives the current wallpaper at the specified monitor
//
HRESULT DesktopWallpaper::GetWallpaper(LPCWSTR pwzMonitorID, LPWSTR* ppwzWallpaper)
{
    // TODO::Handle the case where this is not null
    if (pwzMonitorID != nullptr)
    {
    }

    DWORD dwType = REG_SZ;
    DWORD dwSize = MAX_PATH*sizeof(WCHAR);
    *ppwzWallpaper = (LPWSTR)CoTaskMemAlloc(MAX_PATH*sizeof(WCHAR));
    SHGetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", &dwType, *ppwzWallpaper, &dwSize);
    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorDevicePathAt
// Retrives a string which uniquely identifies the specified monitor.
//
HRESULT DesktopWallpaper::GetMonitorDevicePathAt(UINT uMonitorIndex, LPWSTR* ppwzMonitorID)
{
    *ppwzMonitorID = (LPWSTR)CoTaskMemAlloc(MAX_PATH * 2);
    StringCchCopy(*ppwzMonitorID, MAX_PATH, monitors[uMonitorIndex]);
    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorDevicePathCount
// Retrives the number of monitors.
//
HRESULT DesktopWallpaper::GetMonitorDevicePathCount(LPUINT puCount)
{
    *puCount = GetSystemMetrics(SM_CMONITORS);
    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorRECT
// Retrives the rectangle of a particular monitor.
//
HRESULT DesktopWallpaper::GetMonitorRECT(LPCWSTR pwzMonitorID, tagRECT* pDisplayRect)
{
    for (LPWSTR monitor : monitors)
    {
        if (_wcsicmp(monitor, pwzMonitorID) == 0)
        {
            pDisplayRect->bottom = 1200;
            pDisplayRect->left = 0;
            pDisplayRect->right = 1920;
            pDisplayRect->top = 0;
            return S_OK;
        }
    }
    return S_OK;
}


//
// IDesktopWallpaper::SetBackgroundColor
// Sets the desktop background color.
//
HRESULT DesktopWallpaper::SetBackgroundColor(ULONG uColor)
{
    WCHAR wzColor[64];
    StringCchPrintfW(wzColor, _countof(wzColor), L"%d %d %d", uColor & 0xFF, (uColor >> 8) & 0xFF, (uColor >> 16) & 0xFF);
    SHSetValue(HKEY_CURRENT_USER, L"Control Panel\\Colors", L"Background", REG_SZ,
        wzColor, (DWORD)wcslen(wzColor)*sizeof(wzColor[0]));
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);

    return S_OK;
}


//
// IDesktopWallpaper::GetBackgroundColor
// Retrives the desktop background color.
//
HRESULT DesktopWallpaper::GetBackgroundColor(PULONG puColor)
{
    // Get the current color from the registry
    WCHAR wzColor[64];
    DWORD dwType = REG_SZ;
    DWORD dwSize = sizeof(wzColor);
    SHGetValue(HKEY_CURRENT_USER, L"Control Panel\\Colors", L"Background", &dwType, wzColor, &dwSize);
    
    // Parse the registry value
    ULONG uRed, uGreen, uBlue;
    LPWSTR pEndPtr;
    uRed = wcstoul(wzColor, &pEndPtr, 10);
    uGreen = wcstoul(pEndPtr, &pEndPtr, 10);
    uBlue = wcstoul(pEndPtr, &pEndPtr, 10);

    // Return the color :)
    *puColor = (uBlue << 16) | (uGreen << 8) | uRed;
    return S_OK;
}


//
// IDesktopWallpaper::SetPosition
// Sets the wallpapers style and tiling options.
//
HRESULT DesktopWallpaper::SetPosition(DESKTOP_WALLPAPER_POSITION position)
{
    LPCWSTR pwzStyle = L"0";
    LPCWSTR pwzTile = L"0";

    switch (position)
    {
    case DWPOS_CENTER:
        {
        }
        break;

    case DWPOS_FILL:
        {
            pwzStyle = L"10";
        }
        break;

    case DWPOS_FIT:
        {
            pwzStyle = L"6";
        }
        break;

    case DWPOS_SPAN:
        {
            pwzStyle = L"22";
        }
        break;

    case DWPOS_STRETCH:
        {
            pwzStyle = L"2";
        }
        break;

    case DWPOS_TILE:
        {
            pwzTile = L"1";
        }
        break;
    }

    SHSetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle", REG_SZ,
        pwzStyle, (DWORD)wcslen(pwzStyle)*sizeof(pwzStyle[0]));
    SHSetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"TileWallpaper", REG_SZ,
        pwzTile, (DWORD)wcslen(pwzTile)*sizeof(pwzTile[0]));
    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);

    return S_OK;
}


//
// IDesktopWallpaper::GetPosition
// Gets the current wallpaper style and tiling mode.
//
HRESULT DesktopWallpaper::GetPosition(DESKTOP_WALLPAPER_POSITION* pPosition)
{
    WCHAR wzStyle[64], wzTiling[64];
    DWORD dwType = REG_SZ;
    DWORD dwSize = sizeof(wzStyle);
    SHGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle", &dwType, wzStyle, &dwSize);
    SHGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"TileWallpaper", &dwType, wzTiling, &dwSize);

    if (*wzTiling == L'\1')
    {
        *pPosition = DWPOS_TILE;
    }
    else if (_wcsicmp(wzStyle, L"0") == 0)
    {
        *pPosition = DWPOS_CENTER;
    }
    else if (_wcsicmp(wzStyle, L"2") == 0)
    {
        *pPosition = DWPOS_STRETCH;
    }
    else if (_wcsicmp(wzStyle, L"6") == 0)
    {
        *pPosition = DWPOS_FIT;
    }
    else if (_wcsicmp(wzStyle, L"10") == 0)
    {
        *pPosition = DWPOS_FILL;
    }
    else if (_wcsicmp(wzStyle, L"22") == 0)
    {
        *pPosition = DWPOS_SPAN;
    }
    else
    {
        *pPosition = DWPOS_CENTER;
    }

    return S_OK;
}


//
// IDesktopWallpaper::SetSlideshow
// 
//
HRESULT DesktopWallpaper::SetSlideshow(IShellItemArray* pItems)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaper::GetSlideshow
// 
//
HRESULT DesktopWallpaper::GetSlideshow(IShellItemArray** ppItems)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaper::SetSlideshowOptions
// 
//
HRESULT DesktopWallpaper::SetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS options, UINT uSlideshowTick)
{
    return S_OK;
}


//
// IDesktopWallpaper::GetSlideshowOptions
// 
//
HRESULT DesktopWallpaper::GetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS *pOptions, LPUINT puSlideshowTick)
{
    *pOptions = DESKTOP_SLIDESHOW_OPTIONS(0);
    return S_OK;
}


//
// IDesktopWallpaper::AdvanceSlideshow
// 
//
HRESULT DesktopWallpaper::AdvanceSlideshow(LPCWSTR pwzMonitorID, DESKTOP_SLIDESHOW_DIRECTION direction)
{
    return S_OK;
}


//
// IDesktopWallpaper::GetStatus
// 
//
HRESULT DesktopWallpaper::GetStatus(DESKTOP_SLIDESHOW_STATE* pState)
{
    *pState = DESKTOP_SLIDESHOW_STATE::DSS_ENABLED;
    return S_OK;
}


//
// IDesktopWallpaper::Enable
// 
//
HRESULT DesktopWallpaper::Enable(BOOL bEnable)
{
    return S_OK;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateA(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateB(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateC(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
// Something to do with monitors.
//
HRESULT DesktopWallpaper::PrivateD(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateE(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateF(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateG(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// ???
//
HRESULT DesktopWallpaper::PrivateH(LPVOID)
{
    return E_NOTIMPL;
}
