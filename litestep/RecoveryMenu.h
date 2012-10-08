//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2012  LiteStep Development Team
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
#if !defined RECOVERYMENU_H
#define RECOVERYMENU_H

#include "../utility/common.h"
#include "../utility/IService.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// RecoveryMenu
// Provides a recovery menu in a separate thread which can be accessed
// in case the other threads are rendered unusable
//
class RecoveryMenu : public IService
{
    HINSTANCE m_hInstance;
    
    DWORD m_dwThreadId;
    HANDLE m_hThread;
    
public:
    RecoveryMenu(HINSTANCE hInstance);
    
    //
    // IService methods
    //
    HRESULT Start();
    HRESULT Stop();
    
private:
    static DWORD WINAPI ThreadThunk(LPVOID pParam);
    DWORD ThreadProc();
    
    static LRESULT WINAPI WindowThunk(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    int ShowMenu(HWND hWnd) const;
    void HandleMenuCommand(int nCommand) const;
};


#endif // RECOVERYMENU_H
