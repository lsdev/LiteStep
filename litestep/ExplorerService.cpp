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
#include "../utility/debug.hpp"
#include "ExplorerService.h"
#include "ShellDesktopTray.h"


//
// ExplorerService
//
ExplorerService::ExplorerService() : 
    m_dwThreadID(0),
    m_hExplorerThread(nullptr)
{
}


//
// ~ExplorerService
//
ExplorerService::~ExplorerService()
{
}


//
// IService::Stop
//
HRESULT ExplorerService::Stop()
{
    if (m_dwThreadID)
    {
        HWND hProgman = FindWindow(_T("Progman"), NULL);
        SendMessage(hProgman, 0x44D, 0, 0);
        PostMessage(hProgman, WM_QUIT, 0, 1);
    }

    if (m_hExplorerThread)
    {
        if (WaitForSingleObject(m_hExplorerThread, 1000) != WAIT_OBJECT_0)
        {
            TerminateThread(m_hExplorerThread, 0);
        }

        TRACE("%p", GetShellWindow());

        CloseHandle(m_hExplorerThread);
        m_hExplorerThread = nullptr;
    }

    return S_OK;
}


//
// IService::Start
//
HRESULT ExplorerService::Start()
{
    m_hExplorerThread = CreateThread(NULL, 0, ExplorerThread, NULL, 0, &m_dwThreadID);
    Sleep(1000);
    return S_OK;
}


//
// IService::Recycle
//
HRESULT ExplorerService::Recycle()
{
    return S_OK;
}


//
// ExplorerThread
//
DWORD WINAPI ExplorerService::ExplorerThread(LPVOID)
{
    typedef void *(WINAPI *SHCREATEDESKTOP)(void *);
    typedef bool (WINAPI *SHDESKTOPMESSAGELOOP)(void *);

    TShellDesktopTray *pExplorerTray = nullptr;
    IShellDesktopTray *pTray = nullptr;
    SHCREATEDESKTOP fnSHCreateDesktop = nullptr;
    SHDESKTOPMESSAGELOOP fnSHDesktopMessageLoop = nullptr;
    HANDLE hDesktop = nullptr;
    DWORD dwReturn = 0;

    fnSHCreateDesktop = (SHCREATEDESKTOP)GetProcAddress(
        GetModuleHandle(_T("SHELL32.DLL")), (LPCSTR)0xC8);
    fnSHDesktopMessageLoop = (SHDESKTOPMESSAGELOOP)GetProcAddress(
        GetModuleHandle(_T("SHELL32.DLL")), (LPCSTR)0xC9);

    if (fnSHCreateDesktop && fnSHDesktopMessageLoop)
    {
        pExplorerTray = new TShellDesktopTray();
        pExplorerTray->QueryInterface(IID_IShellDesktopTray,
            reinterpret_cast<LPVOID*>(&pTray));

        if ((hDesktop = fnSHCreateDesktop(pTray)) != nullptr)
        {
            fnSHDesktopMessageLoop(hDesktop);
        }
        else
        {
            dwReturn = 1;
        }

        pTray->Release();
        pExplorerTray->Release();
    }
    else
    {
        dwReturn = 1;
    }

    return dwReturn;
}
