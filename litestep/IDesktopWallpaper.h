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
#if !defined(IDESKTOPWALLPAPER_H)
#define IDESKTOPWALLPAPER_H

//
// This is backport of the IDesktopWallpaper class, and related types, since
// they do not exist in the XP toolkit (and LS will not run on XP if compiled
// with the post-XP toolkit).
// 

#include <ShlObj.h>

enum DESKTOP_SLIDESHOW_DIRECTION
{
    DSD_FORWARD = 0,
    DSD_BACKWARD = 1
};

enum DESKTOP_WALLPAPER_POSITION
{
    DWPOS_CENTER = 0,
    DWPOS_TILE = 1,
    DWPOS_STRETCH = 2,
    DWPOS_FIT = 3,
    DWPOS_FILL = 4,
    DWPOS_SPAN = 5
};

enum DESKTOP_SLIDESHOW_OPTIONS
{
    DSO_SHUFFLEIMAGES = 0x1
};

enum DESKTOP_SLIDESHOW_STATE
{
    DSS_ENABLED = 0x1,
    DSS_SLIDESHOW = 0x2,
    DSS_DISABLED_BY_REMOTE_SESSION = 0x4
};

const GUID IID_IDesktopWallpaper = { 0xb92b56a9, 0x8b55, 0x4e14, { 0x9a, 0x89, 0x01, 0x99, 0xbb, 0xb6, 0xf9, 0x3b} };

class IDesktopWallpaper : public IUnknown
{
    STDMETHOD(SetWallpaper) (LPCWSTR, LPCWSTR) = 0;
    STDMETHOD(GetWallpaper) (LPCWSTR, LPWSTR*) = 0;
    STDMETHOD(GetMonitorDevicePathAt) (unsigned int, LPWSTR*) = 0;
    STDMETHOD(GetMonitorDevicePathCount) (unsigned int*) = 0;
    STDMETHOD(GetMonitorRECT) (LPCWSTR, tagRECT*) = 0;
    STDMETHOD(SetBackgroundColor) (unsigned long) = 0;
    STDMETHOD(GetBackgroundColor) (unsigned long*) = 0;
    STDMETHOD(SetPosition) (DESKTOP_WALLPAPER_POSITION) = 0;
    STDMETHOD(GetPosition) (DESKTOP_WALLPAPER_POSITION*) = 0;
    STDMETHOD(SetSlideshow) (IShellItemArray*) = 0;
    STDMETHOD(GetSlideshow) (IShellItemArray**) = 0;
    STDMETHOD(SetSlideshowOptions) (DESKTOP_SLIDESHOW_OPTIONS, unsigned int) = 0;
    STDMETHOD(GetSlideshowOptions) (DESKTOP_SLIDESHOW_OPTIONS*, unsigned int*) = 0;
    STDMETHOD(AdvanceSlideshow) (LPCWSTR, DESKTOP_SLIDESHOW_DIRECTION) = 0;
    STDMETHOD(GetStatus) (DESKTOP_SLIDESHOW_STATE*) = 0;
    STDMETHOD(Enable) (BOOL) = 0;
};

#endif // IDESKTOPWALLPAPER_H
