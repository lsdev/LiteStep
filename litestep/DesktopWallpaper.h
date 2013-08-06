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
#if !defined(DESKTOPWALLPAPER_H)
#define DESKTOPWALLPAPER_H

#include "IDesktopWallpaper.h"
#include "IDesktopWallpaperPrivate.h"
#include <vector>

class DesktopWallpaper : public IDesktopWallpaperPrivate
{
public:
    DesktopWallpaper();

private:
    virtual ~DesktopWallpaper();

    // IUnknown Implementation
public:
    ULONG STDMETHODCALLTYPE AddRef();
    STDMETHOD(QueryInterface) (REFIID, void**);
    ULONG STDMETHODCALLTYPE Release();

    // IDesktopWallpaper Implementation
public:
    STDMETHOD(SetWallpaper) (LPCWSTR, LPCWSTR);
    STDMETHOD(GetWallpaper) (LPCWSTR, LPWSTR*);
    STDMETHOD(GetMonitorDevicePathAt) (UINT, LPWSTR*);
    STDMETHOD(GetMonitorDevicePathCount) (LPUINT);
    STDMETHOD(GetMonitorRECT) (LPCWSTR, tagRECT*);
    STDMETHOD(SetBackgroundColor) (ULONG);
    STDMETHOD(GetBackgroundColor) (PULONG);
    STDMETHOD(SetPosition) (DESKTOP_WALLPAPER_POSITION);
    STDMETHOD(GetPosition) (DESKTOP_WALLPAPER_POSITION*);
    STDMETHOD(SetSlideshow) (IShellItemArray*);
    STDMETHOD(GetSlideshow) (IShellItemArray**);
    STDMETHOD(SetSlideshowOptions) (DESKTOP_SLIDESHOW_OPTIONS, UINT);
    STDMETHOD(GetSlideshowOptions) (DESKTOP_SLIDESHOW_OPTIONS*, LPUINT);
    STDMETHOD(AdvanceSlideshow) (LPCWSTR, DESKTOP_SLIDESHOW_DIRECTION);
    STDMETHOD(GetStatus) (DESKTOP_SLIDESHOW_STATE*);
    STDMETHOD(Enable) (BOOL);

    // IDesktopWallpaperPrivate Implementation
public:
    STDMETHOD(SetWallpaper2) (LPCWSTR, LPCWSTR);
    STDMETHOD(PrivateB) (LPVOID);
    STDMETHOD(GetWallpaperColor) (PULONG);
    STDMETHOD(GetMonitorNumber) (LPCWSTR, LPUINT);
    STDMETHOD(PrivateE) (LPVOID);
    STDMETHOD(PrivateF) (LPVOID);
    STDMETHOD(PrivateG) (LPVOID);
    STDMETHOD(PrivateH) (LPVOID);

private:
    struct MonitorData
    {
        RECT rRect;
        WCHAR wzDeviceString[MAX_PATH];
        UINT uNumber;
    };

private:
    ULONG m_uRefCount;
    std::vector<MonitorData> m_vMonitors;
};

#endif // DESKTOPWALLPAPER_H
