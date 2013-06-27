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
#if !defined(DDESERVICE_H)
#define DDESERVICE_H

#include "DDEWorker.h"
#include "../utility/IService.h"

class DDEService: public IService
{
public:
    DDEService();
    ~DDEService();
    
    HRESULT Start();
    HRESULT Stop();
    
    static HDDEDATA CALLBACK DdeCallback(
        UINT wType, UINT wFmt, HCONV hConv, HSZ hszTopic,
        HSZ hszItem, HDDEDATA hData, DWORD lData1, DWORD lData2);
    
private:
    HRESULT _RegisterDDE();
    
    bool _DoStart();
    void _DoStop();
    
    static DWORD WINAPI _DDEThreadProc(LPVOID pvService);
    
    static DWORD m_idThread;
    static HANDLE m_hThread;
    static HANDLE m_hStartEvent;
    
    static DDEWorker m_DDEWorker;
    static HSZ m_hszProgman;
    static HSZ m_hszGroups;
    static HSZ m_hszFolders;
    static HSZ m_hszAppProperties;
    static DWORD m_dwDDEInst;
};

#endif // DDESERVICE_H
