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
#include <WinCrypt.h>

//
// Wrapper around SHCreateShellItemArrayFromIDLists.
// Until we drop XP support.
//
static HRESULT LSCreateShellItemArrayFromIDLists(UINT cidl, PCIDLIST_ABSOLUTE_ARRAY rgpidl,
                                          IShellItemArray **ppsiItemArray)
{
    typedef HRESULT (WINAPI* PROCTYPE)(UINT cidl, PCIDLIST_ABSOLUTE_ARRAY rgpidl,
                                       IShellItemArray **ppsiItemArray);
    static PROCTYPE proc = nullptr;

    if (proc == nullptr)
    {
        proc = (PROCTYPE)GetProcAddress(GetModuleHandle(_T("Shell32.dll")),
            "SHCreateShellItemArrayFromIDLists");
    }

    if (proc == nullptr)
    {
        return E_FAIL;
    }

    return proc(cidl, rgpidl, ppsiItemArray);
}


//
// Wrapper around SHGetValue to avoid having to declare extra variables.
//
static LSTATUS GetRegValue(HKEY key, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD dwType,
                           LPVOID pvData, DWORD cbData)
{
    return SHGetValue(key, pszSubKey, pszValue, &dwType, pvData, &cbData);
}


//
// DesktopWallpaper
//
DesktopWallpaper::DesktopWallpaper()
{
    m_uRefCount = 1;
    EnumDisplayMonitors(nullptr, nullptr, [] (HMONITOR hMonitor, HDC, LPRECT rect, LPARAM lParam) -> BOOL
    {
        DesktopWallpaper* self = (DesktopWallpaper*)lParam;

        MONITORINFOEXW monitorInfoEx;
        ZeroMemory(&monitorInfoEx, sizeof(MONITORINFOEXW));
        monitorInfoEx.cbSize = sizeof(MONITORINFOEXW);
        GetMonitorInfoW(hMonitor, &monitorInfoEx);

        MonitorData data;
        data.rRect = *rect;
        data.uNumber = UINT(self->m_vMonitors.size() + 1);
        StringCchCopyW(data.wzDeviceString, _countof(data.wzDeviceString), monitorInfoEx.szDevice);

        self->m_vMonitors.push_back(data);

        return TRUE;

    }, (LPARAM)this);
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
    if (pwzWallpaper == nullptr)
    {
        return E_INVALIDARG;
    }

    if (!PathFileExistsW(pwzWallpaper))
    {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    WCHAR wzThemeFile[MAX_PATH];
    GetRegValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes",
        L"CurrentTheme", REG_SZ, wzThemeFile, sizeof(wzThemeFile));

    // TODO::Handle the case where this is not null
    if (pwzMonitorID == nullptr)
    {
        SHSetValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", REG_SZ,
            pwzWallpaper, (DWORD)wcslen(pwzWallpaper)*sizeof(pwzWallpaper[0]));
        WritePrivateProfileStringW(
            L"Control Panel\\Desktop", L"MultimonBackgrounds", L"0", wzThemeFile);
        WritePrivateProfileStringW(
            L"Control Panel\\Desktop", L"Wallpaper", pwzWallpaper, wzThemeFile);
    }
    else
    {
        UINT uMonitor;
        if (!SUCCEEDED(GetMonitorNumber(pwzMonitorID, &uMonitor)))
        {
            return E_INVALIDARG;
        }

        WritePrivateProfileStringW(
            L"Control Panel\\Desktop", L"MultimonBackgrounds", L"1", wzThemeFile);
        WCHAR wzWallPaperKey[32];
        StringCchPrintf(wzWallPaperKey, 32, L"Wallpaper%u", uMonitor);
        WritePrivateProfileStringW(
            L"Control Panel\\Desktop", wzWallPaperKey, pwzWallpaper, wzThemeFile);
    }

    SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0);

    return S_OK;
}


//
// IDesktopWallpaper::GetWallpaper
// Retrives the current wallpaper at the specified monitor
//
HRESULT DesktopWallpaper::GetWallpaper(LPCWSTR pwzMonitorID, LPWSTR* ppwzWallpaper)
{
    if (ppwzWallpaper == nullptr)
    {
        return E_POINTER;
    }

    WCHAR wzThemeFile[MAX_PATH];
    GetRegValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes",
        L"CurrentTheme", REG_SZ, wzThemeFile, sizeof(wzThemeFile));

    bool bMultimonBackgrounds = GetPrivateProfileIntW(
        L"Control Panel\\Desktop", L"MultimonBackgrounds", 0, wzThemeFile) != 0;

    if (!bMultimonBackgrounds)
    {
        // TODO::Slideshow
        *ppwzWallpaper = (LPWSTR)CoTaskMemAlloc(MAX_PATH*sizeof(WCHAR));
        GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", REG_SZ,
            *ppwzWallpaper, MAX_PATH*sizeof(WCHAR));
    }
    else if (pwzMonitorID == nullptr)
    {
        *ppwzWallpaper = (LPWSTR)CoTaskMemAlloc(1 * sizeof(WCHAR));
        **ppwzWallpaper = L'\0';
        return S_FALSE;
    }
    else
    {
        UINT uMonitor;
        if (!SUCCEEDED(GetMonitorNumber(pwzMonitorID, &uMonitor)))
        {
            return E_INVALIDARG;
        }
        *ppwzWallpaper = (LPWSTR)CoTaskMemAlloc(MAX_PATH*sizeof(WCHAR));
        WCHAR wzWallPaperKey[32];
        StringCchPrintf(wzWallPaperKey, 32, L"Wallpaper%u", uMonitor);
        GetPrivateProfileStringW(L"Control Panel\\Desktop", wzWallPaperKey, L"", *ppwzWallpaper,
            MAX_PATH*sizeof(WCHAR), wzThemeFile);
    }

    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorDevicePathAt
// Retrives a string which uniquely identifies the specified monitor.
//
HRESULT DesktopWallpaper::GetMonitorDevicePathAt(UINT uMonitorIndex, LPWSTR* ppwzMonitorID)
{
    if (ppwzMonitorID == nullptr)
    {
        return E_POINTER;
    }

    if (uMonitorIndex >= m_vMonitors.size())
    {
        return E_INVALIDARG;
    }

    SHStrDupW(m_vMonitors[uMonitorIndex].wzDeviceString, ppwzMonitorID);

    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorDevicePathCount
// Retrives the number of monitors.
//
HRESULT DesktopWallpaper::GetMonitorDevicePathCount(LPUINT puCount)
{
    if (puCount == nullptr)
    {
        return E_POINTER;
    }

    *puCount = UINT(m_vMonitors.size());

    return S_OK;
}


//
// IDesktopWallpaper::GetMonitorRECT
// Retrives the rectangle of a particular monitor.
//
HRESULT DesktopWallpaper::GetMonitorRECT(LPCWSTR pwzMonitorID, tagRECT* pDisplayRect)
{
    if (pDisplayRect == nullptr)
    {
        return E_POINTER;
    }

    if (pwzMonitorID == nullptr)
    {
        // This is what explorer returns in this case.
        return 0x800706F4;
    }

    for (MonitorData &monitor : m_vMonitors)
    {
        if (_wcsicmp(monitor.wzDeviceString, pwzMonitorID) == 0)
        {
            *pDisplayRect = monitor.rRect;
            return S_OK;
        }
    }

    return E_INVALIDARG;
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
    if (puColor == nullptr)
    {
        return E_POINTER;
    }

    WCHAR wzColor[64];
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Colors", L"Background", REG_SZ, wzColor,
        sizeof(wzColor));

    ULONG uRed, uGreen, uBlue;
    LPWSTR pEndPtr;
    uRed = wcstoul(wzColor, &pEndPtr, 10);
    uGreen = wcstoul(pEndPtr, &pEndPtr, 10);
    uBlue = wcstoul(pEndPtr, &pEndPtr, 10);

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

    default:
        {
        }
        return E_INVALIDARG;
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
    if (pPosition == nullptr)
    {
        return E_POINTER;
    }

    WCHAR wzStyle[64], wzTiling[64];
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"WallpaperStyle", REG_SZ, wzStyle,
        sizeof(wzStyle));
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"TileWallpaper", REG_SZ, wzTiling,
        sizeof(wzTiling));

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
    UNREFERENCED_PARAMETER(pItems);
    return S_OK;
}


//
// IDesktopWallpaper::GetSlideshow
//
//
HRESULT DesktopWallpaper::GetSlideshow(IShellItemArray** ppItems)
{
    if (ppItems == nullptr)
    {
        return E_POINTER;
    }

    WCHAR wallpaper[MAX_PATH];
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"Wallpaper", REG_SZ, wallpaper,
        sizeof(wallpaper));

    HRESULT hr;

    IShellFolder *pRootFolder = nullptr;
    hr = SHGetDesktopFolder(&pRootFolder);

    LPITEMIDLIST idList = nullptr;
    if (SUCCEEDED(hr))
    {
        hr = pRootFolder->ParseDisplayName(nullptr, nullptr, wallpaper, nullptr, &idList, NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = LSCreateShellItemArrayFromIDLists(1, (LPCITEMIDLIST*)&idList, ppItems);
    }

    if (idList)
    {
        CoTaskMemFree(idList);
    }
    if (pRootFolder)
    {
        pRootFolder->Release();
    }

    // S_FALSE means that the slideshow is currently disabled
    return S_FALSE;
}


//
// IDesktopWallpaper::SetSlideshowOptions
//
//
HRESULT DesktopWallpaper::SetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS options, UINT uSlideshowTick)
{
    DWORD shuffle = options & DSO_SHUFFLEIMAGES ? 1 : 0;

    SHSetValueW(HKEY_CURRENT_USER, L"Control Panel\\Personalization\\Desktop Slideshow",
        L"Interval", REG_DWORD, &uSlideshowTick, sizeof(DWORD));
    SHSetValueW(HKEY_CURRENT_USER, L"Control Panel\\Personalization\\Desktop Slideshow",
        L"Shuffle", REG_DWORD, &shuffle, sizeof(DWORD));

    return S_OK;
}


//
// IDesktopWallpaper::GetSlideshowOptions
//
//
HRESULT DesktopWallpaper::GetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS *pOptions, LPUINT puSlideshowTick)
{
    if (pOptions == nullptr || puSlideshowTick == nullptr)
    {
        return E_POINTER;
    }

    DWORD interval, shuffle;
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Personalization\\Desktop Slideshow",
        L"Interval", REG_DWORD, &interval, sizeof(DWORD));
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Personalization\\Desktop Slideshow",
        L"Shuffle", REG_DWORD, &shuffle, sizeof(DWORD));

    *pOptions = shuffle ? DSO_SHUFFLEIMAGES : DESKTOP_SLIDESHOW_OPTIONS(0);
    *puSlideshowTick = interval;

    return S_OK;
}


//
// IDesktopWallpaper::AdvanceSlideshow
//
//
HRESULT DesktopWallpaper::AdvanceSlideshow(LPCWSTR pwzMonitorID, DESKTOP_SLIDESHOW_DIRECTION direction)
{
    UNREFERENCED_PARAMETER(pwzMonitorID);
    UNREFERENCED_PARAMETER(direction);

    // TODO:Implement this.
    return S_OK;
}


//
// IDesktopWallpaper::GetStatus
//
//
HRESULT DesktopWallpaper::GetStatus(DESKTOP_SLIDESHOW_STATE* pState)
{
    if (pState == nullptr)
    {
        return E_POINTER;
    }

    *pState = DESKTOP_SLIDESHOW_STATE::DSS_ENABLED;
    return S_OK;
}


//
// IDesktopWallpaper::Enable
//
//
HRESULT DesktopWallpaper::Enable(BOOL /* bEnable */)
{
    return S_OK;
}


//
// IDesktopWallpaperPrivate::??
// Never seen anything call this.
// Experiments with explorer, makes this seem like an alias of SetWallpaper
//
HRESULT DesktopWallpaper::SetWallpaper2(LPCWSTR pwzMonitor, LPCWSTR pwzWallpaper)
{
    return SetWallpaper(pwzMonitor, pwzWallpaper);
}


//
// IDesktopWallpaperPrivate::??
// Likes to give me E_PENDING. Does not care about parameters, not sure it actually
// takes any.
//
HRESULT DesktopWallpaper::PrivateB(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// Called by the Color and Appearance dialog when you select the automatic
// DWM color
//
HRESULT DesktopWallpaper::GetWallpaperColor(PULONG puColor)
{
    // TODO::We may need to actually compute the DWM colorization here...
    if (puColor == nullptr)
    {
        return E_POINTER;
    }

    DWORD dwColor;
    GetRegValue(HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"ImageColor", REG_DWORD, &dwColor,
        sizeof(DWORD));
    *puColor = dwColor;

    return S_OK;
}


//
// IDesktopWallpaperPrivate::??
// Seems to set the 2nd parameter to the monitor "#"
//
HRESULT DesktopWallpaper::GetMonitorNumber(LPCWSTR pwzMonitorID, LPUINT puMonitorNumber)
{
    if (puMonitorNumber == nullptr)
    {
        return E_POINTER;
    }

    if (pwzMonitorID == nullptr)
    {
        // This is what explorer returns in this case.
        return 0x800706F4;
    }

    for (MonitorData &monitor : m_vMonitors)
    {
        if (_wcsicmp(monitor.wzDeviceString, pwzMonitorID) == 0)
        {
            *puMonitorNumber = monitor.uNumber;
            return S_OK;
        }
    }

    return E_INVALIDARG;
}


//
// IDesktopWallpaperPrivate::??
// Called when you change themes.
// Explorer tends to return S_OK, in my own tests. Not really sure what it does.
// Can't decipher params. Might be bogus.
//
HRESULT DesktopWallpaper::PrivateE(LPVOID, LPVOID, LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// This function might exist.
//
HRESULT DesktopWallpaper::PrivateF(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// Pretty sure this does not actually exist
//
HRESULT DesktopWallpaper::PrivateG(LPVOID)
{
    return E_NOTIMPL;
}


//
// IDesktopWallpaperPrivate::??
// Pretty sure this doesn't exist either
//
HRESULT DesktopWallpaper::PrivateH(LPVOID)
{
    return E_NOTIMPL;
}
