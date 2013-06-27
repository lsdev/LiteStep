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
#if !defined(STARTUPRUNNER_H)
#define STARTUPRUNNER_H

#include "../utility/common.h"

class StartupRunner
{
public:
    StartupRunner();
    virtual ~StartupRunner();
    
    static void Run(BOOL bForce);
    static bool IsFirstRunThisSession(LPCTSTR pszSubkey);
    
private:
    static DWORD WINAPI _ThreadProc(LPVOID lpData);
    static HKEY _CreateSessionInfoKey();
    static void _RunRegKeys(HKEY hkParent, LPCTSTR ptzSubKey, DWORD dwFlags);
    static void _RunRegKeysWorker(HKEY hkParent, LPCTSTR ptzSubKey, DWORD dwFlags);
    static void _RunRunOnceEx();
    static void _RunStartupMenu();
    static void _RunShellFolderContents(int nFolder);
    static void _SpawnProcess(LPTSTR ptzCommandLine, DWORD dwFlags);
    static HANDLE _CreateProcess(LPTSTR ptzCommandLine);
    static HANDLE _ShellExecuteEx(LPCTSTR ptzExecutable, LPCTSTR ptzArgs);
};

#endif // STARTUPRUNNER_H
