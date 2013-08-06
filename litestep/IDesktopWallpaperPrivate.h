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
#if !defined(IDESKTOPWALLPAPERPRIVATE_H)
#define IDESKTOPWALLPAPERPRIVATE_H

//
// The functions listed here, and their functionality are guesses, which were
// obtained by examining how explorer handles these calls.
// 

const GUID IID_IDesktopWallpaperPrivate = { 0xC182461F, 0xDFAC, 0x4375, { 0xAB, 0x6E, 0x4C, 0xC4, 0x5A, 0xA7, 0xF9, 0xCC } };
class IDesktopWallpaperPrivate : public IDesktopWallpaper
{
public:
    STDMETHOD(SetWallpaper2) (LPCWSTR, LPCWSTR) = 0;
    STDMETHOD(PrivateB) (LPVOID) = 0;
    STDMETHOD(GetWallpaperColor) (PULONG) = 0;
    STDMETHOD(GetMonitorNumber) (LPCWSTR, LPUINT) = 0;
    STDMETHOD(PrivateE) (LPVOID) = 0;
    STDMETHOD(PrivateF) (LPVOID) = 0;
    STDMETHOD(PrivateG) (LPVOID) = 0;
    STDMETHOD(PrivateH) (LPVOID) = 0;
};

#endif // IDESKTOPWALLPAPERPRIVATE_H
