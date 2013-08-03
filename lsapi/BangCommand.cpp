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
#include "BangCommand.h"
#include "ThreadedBangCommand.h"
#include <memory>
#include "../utility/stringutility.h"


Bang::Bang(DWORD dwThread, BangCommandW pfnBang, LPCWSTR pwzCommand) :
    m_sCommand(pwzCommand)
{
    m_bEX = false;
    m_bBang = pfnBang;
    m_pAddress = pfnBang;
    m_dwThreadID = dwThread;
}


Bang::Bang(DWORD dwThread, BangCommandA pfnBang, LPCWSTR pwzCommand) :
    m_sCommand(pwzCommand)
{
    m_bEX = false;
    m_bBang = [pfnBang] (HWND hOwner, LPCWSTR pwzArgs) -> void {
        pfnBang(hOwner, std::unique_ptr<char>(MBSFromWCS(pwzArgs)).get());
    };
    m_pAddress = pfnBang;
    m_dwThreadID = dwThread;
}


Bang::Bang(DWORD dwThread, BangCommandExW pfnBang, LPCWSTR pwzCommand) :
    m_sCommand(pwzCommand)
{
    m_bEX = true;
    m_bBangEX = pfnBang;
    m_pAddress = pfnBang;
    m_dwThreadID = dwThread;
}


Bang::Bang(DWORD dwThread, BangCommandExA pfnBang, LPCWSTR pwzCommand) :
    m_sCommand(pwzCommand)
{
    m_bEX = true;
    m_bBangEX = [pfnBang] (HWND hOwner, LPCWSTR pwzCommand, LPCWSTR pwzArgs) -> void {
        pfnBang(hOwner,
            std::unique_ptr<char>(MBSFromWCS(pwzCommand)).get(),
            std::unique_ptr<char>(MBSFromWCS(pwzArgs)).get());
    };;
    m_pAddress = pfnBang;
    m_dwThreadID = dwThread;
}


Bang::~Bang()
{
    // do nothing
}


void Bang::Execute(HWND hCaller, LPCWSTR pwzParams)
{
    DWORD dwThreadID = GetCurrentThreadId();
    
    if (dwThreadID != m_dwThreadID)
    {
        ThreadedBangCommand * pInfo = new ThreadedBangCommand(hCaller,
            m_sCommand.c_str(), pwzParams);
        
        if (pInfo != nullptr)
        {
            // target thread releases pInfo
            PostThreadMessageW(m_dwThreadID, LM_THREAD_BANGCOMMAND,
                (WPARAM)pInfo, 0);
        }
    }
    else
    {
        if (m_bEX)
        {
            m_bBangEX(hCaller, m_sCommand.c_str(), pwzParams);
        }
        else
        {
            m_bBang(hCaller, pwzParams);
        }
    }
}


HINSTANCE Bang::GetModule() const
{
    HINSTANCE hModule = nullptr;

    //
    // Given the BangProc's address, VirtualQuery can
    // figure out the module's HMODULE
    //
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    size_t cbBuffer = sizeof(mbi);
    
    if (VirtualQuery(m_pAddress, &mbi, cbBuffer) == cbBuffer)
    {
        hModule = (HINSTANCE)mbi.AllocationBase;
    }
    
    return hModule;
}
