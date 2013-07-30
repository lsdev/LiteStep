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
#include "../utility/common.h"
#include "ShellDesktopTray.h"
#include "../lsapi/lsapi.h"


TShellDesktopTray::TShellDesktopTray() :
    m_uRefCount(0),
    m_hDesktopWnd(nullptr)
{
}


TShellDesktopTray::~TShellDesktopTray()
{
}


HRESULT TShellDesktopTray::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    if (ppvObj == nullptr)
    {
        return E_POINTER;
    }

    if (riid == IID_IUnknown || riid == IID_IShellDesktopTray)
    {
        *ppvObj = this;
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


ULONG TShellDesktopTray::AddRef()
{
    ++m_uRefCount;

    return m_uRefCount;
}


ULONG TShellDesktopTray::Release()
{
    if (m_uRefCount > 0)
    {
        --m_uRefCount;
    }

    return m_uRefCount;
}


ULONG TShellDesktopTray::GetState()
{
    return 2;
}


HRESULT TShellDesktopTray::GetTrayWindow(HWND *hTrayWnd)
{
    *hTrayWnd = GetLitestepWnd();
    return S_OK;
}


HRESULT TShellDesktopTray::RegisterDesktopWindow(HWND hDesktopWindow)
{
    m_hDesktopWnd = hDesktopWindow;

    // This is called with Progman.
    ShowWindow(m_hDesktopWnd, SW_HIDE);

    return S_OK;
}


HRESULT TShellDesktopTray::SetVar(int, ULONG)
{
    return S_OK;
}
