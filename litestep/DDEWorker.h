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
#if !defined(DDEWORKER_H)
#define DDEWORKER_H

#include "../utility/common.h"
#include <ddeml.h>

#define DDE_REQUEST_NONE          0x0000
#define DDE_REQUEST_CREATEGROUP   0x0001
#define DDE_REQUEST_DELETEGROUP   0x0002
#define DDE_REQUEST_SHOWGROUP     0x0004
#define DDE_REQUEST_ADDITEM       0x0006
#define DDE_REQUEST_DELETEITEM    0x0008
#define DDE_REQUEST_EXPLOREFOLDER 0x0010
#define DDE_REQUEST_VIEWFOLDER    0x0020
#define DDE_REQUEST_FINDFOLDER    0x0040
#define DDE_REQUEST_OPENFINDFILE  0x0060

class DDEWorker
{
public:
    DDEWorker();
    ~DDEWorker();
    
    BOOL ParseRequest(LPCSTR pszRequest);
    BOOL ListGroups(LPVOID& pGroupList, UINT& ulSize);
    
private:
    BOOL _FindFiles(LPSTR pszPath, BOOL bFindFolder);
    BOOL _ShowGroup(LPCTSTR strGroupName, int nShow, BOOL bCommon);
    BOOL _CreateGroup(LPCTSTR strGroupName, BOOL bCommon);
    BOOL _DeleteGroup(LPCTSTR strGroupName, BOOL bCommon);
    BOOL _DeleteItem(LPCTSTR strItem);
    BOOL _AddItem(
      LPCTSTR strCmdLine, LPCTSTR pszDescription, LPCTSTR pszIconPath,
      int nIconIndex, LPCTSTR pszDefDir, WORD dwHotKey, BOOL bMinimize);
    BOOL _ListGroupsHelper(
      HANDLE hHeap, char* szPath, LPVOID& pGroupList, UINT& ulSize);
    DWORD _MatchRequest(LPCSTR pszCommand);
    
    char m_szCurrentGroup[MAX_PATH];
    BOOL m_bIsUserAnAdmin;
};

#endif // DDEWORKER_H
