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
#if !defined(TASKBARLISTHANDLER_H)
#define TASKBARLISTHANDLER_H

class TaskbarListHandler {
public:
    explicit TaskbarListHandler();
    
    HRESULT Start(HWND hWndTray);
    HRESULT Stop();

private:
    HWND m_hWndTaskband;
    ATOM m_aWndClass;
    
    HWND m_hLiteStep;
    DWORD m_dwLiteStepProc;
    HINSTANCE m_hInstance;
    UINT WM_ShellHook;


    LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT WINAPI WindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT WINAPI WindowProcedureInit(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif // TASKBARLISTHANDLER_H
