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
#include "DDEWorker.h"
#include "../utility/core.hpp"

DDEWorker::DDEWorker()
{
    memset(m_szCurrentGroup, 0, MAX_PATH);
      
    SC_HANDLE hSC = OpenSCManager(
        NULL, NULL, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE);
    
    if (hSC)
    {
        m_bIsUserAnAdmin = TRUE;
    }
    else
    {
        m_bIsUserAnAdmin = FALSE;
    }
}


DDEWorker::~DDEWorker()
{
    // do nothing
}


// Handle incoming DDE requests and pass to relevant functions
BOOL DDEWorker::ParseRequest(LPCSTR pszRequest)
{
// 10 is the maximum number of parameters passed in AddItem PROGMAN DDE call
#define DDE_ADDITEM_PARAM_COUNT (10)
    LPSTR pszParamList[DDE_ADDITEM_PARAM_COUNT];
    LPSTR pszTmp;
    LPSTR pszWorkRequest;
    BOOL bReturn = FALSE;
    
    DWORD dwRequest = _MatchRequest(pszRequest);
    if (dwRequest)
    {
        size_t stLength = strlen(pszRequest) + 1;
        pszWorkRequest = new char[stLength];
        
        StringCchCopy(pszWorkRequest, stLength, pszRequest);
        pszWorkRequest[stLength - 3] = '\0';
        pszWorkRequest = strchr(pszWorkRequest, '(');
        ++pszWorkRequest;
        
        pszParamList[0] = pszWorkRequest;
        
        int nCurIndex = 1;
        pszTmp = strchr(pszWorkRequest, ','); // find a delimiter
        while (NULL != pszTmp)
        {
            *(pszTmp++) = '\0'; // null terminate and cut.
            pszParamList[nCurIndex++] = pszTmp; // put it in the pointer list
            pszTmp = strchr(pszTmp, ','); // get the next onf
        }
        
        // Set initial state of the group to act on:
        // bCommon ? Perform function on a common group :
        //           Perform function on a private group
        BOOL bCommon = m_bIsUserAnAdmin;
        
        switch (dwRequest)
        {
        // DDE call: e.g. ExploreFolder(idlist[,object])
        case DDE_REQUEST_EXPLOREFOLDER:
            {
                char szParam[MAX_PATH];
                
                StringCchPrintf(szParam, MAX_PATH,
                    "/e,/idlist,%s", pszParamList[1]);
                
                bReturn = ((LSShellExecute(NULL, "open", "explorer.exe",
                    szParam, NULL, SW_SHOW) > HINSTANCE(32)) ? TRUE : FALSE);
            }
            break;
            
        // DDE call: e.g. ViewFolder(idlist[,object])
        case DDE_REQUEST_VIEWFOLDER:
            {
                char szParam[MAX_PATH];
                
                StringCchPrintf(szParam, MAX_PATH,
                    "/idlist,%s", pszParamList[1]);
                
                bReturn = ((LSShellExecute(NULL, "open", "explorer.exe",
                    szParam, NULL, SW_SHOW) > HINSTANCE(32)) ? TRUE : FALSE);
                
                // Will return too soon if we don't sleep here... funky
                // TODO: Figure out why, and fix!
                Sleep(100);
            }
            break;
            
        // DDE call: e.g. FindFolder(idlist[,object])
        case DDE_REQUEST_FINDFOLDER:
            {
                bReturn = _FindFiles(pszParamList[0], TRUE);
            }
            break;
            
        // DDE call: e.g. OpenFindFile(idlist[,object])
        case DDE_REQUEST_OPENFINDFILE:
            {
                bReturn = _FindFiles(pszParamList[0], FALSE);
            }
            break;
            
        // DDE call: e.g. CreateGroup(GroupName[,CommonGroupFlag])
        case DDE_REQUEST_CREATEGROUP:
            {
                if (2 == nCurIndex) // second parameter forces common/private
                {
                    bCommon = atoi(pszParamList[1]);
                }
                bReturn = _CreateGroup(pszParamList[0], bCommon);
            }
            break;
            
        // DDE call: e.g. DeleteGroup(GroupName[,CommonGroupFlag])
        case DDE_REQUEST_DELETEGROUP:
            {
                if (2 == nCurIndex) // second parameter forces common/private
                {
                    bCommon = atoi(pszParamList[1]);
                }
                bReturn = _DeleteGroup(pszParamList[0], bCommon);
            }
            break;
            
        // DDE call: e.g. ShowGroup(GroupName,ShowCommand[,CommonGroupFlag])
        case DDE_REQUEST_SHOWGROUP:
            {
                if (nCurIndex >= 2) // show command: maps to SW_
                {
                    if (3 == nCurIndex) // third parameter forces common/private
                    {
                        bCommon = atoi(pszParamList[2]);
                    }
                    int nShow = atoi(pszParamList[1]);
                    bReturn = _ShowGroup(pszParamList[0], nShow, bCommon);
                }
            }
            break;
            
        // DDE call: e.g. DeleteItem(ItemName) : applies to current group
        case DDE_REQUEST_DELETEITEM:
            {
                bReturn = _DeleteItem(pszParamList[0]);
            }
            break;
            
        //DDE call:
        //  AddItem(CmdLine[,Name[,IconPath[,IconIndex[,xPos,yPos[,
        //      DefDir[,HotKey[,fMinimize[fSeparateMemSpace]]]]]]])
        //  applies to current group
        case DDE_REQUEST_ADDITEM:
            {
                BOOL bMinimize = FALSE;
                WORD dwHotKey = 0;
                LPCSTR pszDefDir = NULL;
                int nIconIndex = 0;
                LPCSTR pszIconPath = NULL;
                LPCSTR pszDescription = NULL;
                
                // fall through each case
                switch (nCurIndex)
                {
                case 10:  // SeparateMemSpace ignored for now
                case 9:
                    { // Minimize
                        bMinimize = (BOOL)atoi(pszParamList[8]);
                    }
                case 8:
                    { // HotKey
                        dwHotKey = (WORD)atoi(pszParamList[7]);
                    }
                case 7:
                    { // DefDir
                        pszDefDir = pszParamList[6];
                    }
                case 6:  //xPos and yPos ignored. Not necessary
                case 5:
                case 4:
                    { // IconIndex
                        nIconIndex = atoi(pszParamList[3]);
                    }
                case 3:
                    { // IconPath
                        pszIconPath = pszParamList[2];
                    }
                case 2:
                    { // Description
                        pszDescription = pszParamList[1];
                    }
                default:
                    {
                        // do nothing
                    }
                    break;
                }
                
                bReturn = _AddItem(pszParamList[0], pszDescription, pszIconPath,
                    nIconIndex, pszDefDir, dwHotKey, bMinimize);
            }
            break;
            
        default:
            {
                // do nothing
            }
            break;
        }
    }
    
    return bReturn;
}


// List the program groups
BOOL DDEWorker::ListGroups(LPVOID& pGroupList, UINT& ulSize)
{
    char szPath[MAX_PATH];
    pGroupList = NULL;
    ulSize = 0;
    LPSTR pszTemp = NULL;
    BOOL bReturn = FALSE;
    
    // Get user specific folders
    if (GetShellFolderPath(CSIDL_PROGRAMS, szPath, MAX_PATH))
    {
        PathAppend(szPath, "*.*");
        
        HANDLE hHeap = GetProcessHeap();
        if (hHeap)
        {
            if (_ListGroupsHelper(hHeap, szPath, pGroupList, ulSize))
            {
                // Get common folders
                if (GetShellFolderPath(CSIDL_COMMON_PROGRAMS, szPath, MAX_PATH))
                {
                    PathAppend(szPath, "*.*");
                    
                    if (_ListGroupsHelper(hHeap, szPath, pGroupList, ulSize))
                    {
                        // add null terminator
                        pszTemp = (char*) HeapReAlloc(hHeap, HEAP_ZERO_MEMORY,
                            pGroupList, ulSize + 1);
                        
                        if (pszTemp != NULL)
                        {
                            pszTemp[ulSize] = '\0';
                            pGroupList = (LPVOID) pszTemp;
                            ulSize += 1;
                            bReturn = TRUE;
                        }
                    }
                }
            }
        }
    }
    
    return bReturn;
}


DWORD DDEWorker::_MatchRequest(LPCSTR pszCommand)
{
    DWORD dwReturn = DDE_REQUEST_NONE;
    
    if (strstr(pszCommand, "[ExploreFolder(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_EXPLOREFOLDER;
    }
    else if (strstr(pszCommand, "[ViewFolder(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_VIEWFOLDER;
    }
    else if (strstr(pszCommand, "[FindFolder(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_FINDFOLDER;
    }
    else if (strstr(pszCommand, "[OpenFindFile(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_OPENFINDFILE;
    }
    else if (strstr(pszCommand, "[CreateGroup(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_CREATEGROUP;
    }
    else if (strstr(pszCommand, "[DeleteGroup(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_DELETEGROUP;
    }
    else if (strstr(pszCommand, "[ShowGroup(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_SHOWGROUP;
    }
    else if (strstr(pszCommand, "[AddItem(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_ADDITEM;
    }
    else if (strstr(pszCommand, "[DeleteItem(") == pszCommand)
    {
        dwReturn = DDE_REQUEST_DELETEITEM;
    }
    
    return dwReturn;
}


BOOL DDEWorker::_FindFiles(LPSTR pszPath, BOOL bFindFolder)
{
    BOOL bReturn = FALSE;
    
    PathUnquoteSpaces(pszPath);
    PIDLIST_ABSOLUTE pidl = ILCreateFromPath(pszPath);
    
    if (pidl)
    {
        if (bFindFolder) // FindFolder
        {
            bReturn = (BOOL)SHFindFiles(pidl, NULL);
        }
        else // OpenFindFile
        {
            bReturn = (BOOL)SHFindFiles(NULL, pidl);
        }
        
        ILFree(pidl);
    }
    
    return bReturn;
}


BOOL DDEWorker::_ShowGroup(LPCTSTR strGroupName, int nShow, BOOL bCommon)
{
    char szFullPath[MAX_PATH];
    char szPath[MAX_PATH];
    BOOL bReturn = FALSE;
    
    // Get the program group path
    if (GetShellFolderPath(bCommon ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS,
        szPath, MAX_PATH))
    {
        StringCchPrintf(szFullPath, MAX_PATH, "%s\\%s\\", szPath, strGroupName);
        PathQuoteSpaces(szFullPath);
        
        if (PathIsDirectory(szFullPath))
        {
            // open it up!
            LSShellExecute(NULL, "open", szFullPath, NULL, NULL, nShow);
            
            // set our current group to this one, as per Progman DDE
            StringCchCopy(m_szCurrentGroup, MAX_PATH, szFullPath);
            
            bReturn = TRUE;
        }
    }
    
    return bReturn;
}

// Create the program group
BOOL DDEWorker::_CreateGroup(LPCTSTR strGroupName, BOOL bCommon)
{
    char szPath[MAX_PATH];
    char szFullPath[MAX_PATH];
    BOOL bReturn = FALSE;
    
    // Get the program group path
    if (GetShellFolderPath(bCommon ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS,
        szPath, MAX_PATH))
    {
        StringCchPrintf(szFullPath, MAX_PATH, "%s\\%s", szPath, strGroupName);
        PathQuoteSpaces(szFullPath);
        
        // standard create directory call
        if (CreateDirectory(szFullPath, NULL))
        {
            StringCchCopy(m_szCurrentGroup, MAX_PATH, szFullPath);
            
            // Hmmm should we show the group here???? Maybe
            // but not for now.
            //ShellExecute(NULL, "open", szPath, NULL, NULL, nShow);
            
            // Tell the shell that something changed
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATH, szFullPath, 0);
            
            bReturn = TRUE;
        }
        else
        {
            if (ERROR_ALREADY_EXISTS == GetLastError())
            {
                bReturn = TRUE;
            }
        }
    }
    
    return bReturn;
}

// Remove the group
BOOL DDEWorker::_DeleteGroup(LPCTSTR strGroupName, BOOL bCommon)
{
    char szTemp[MAX_PATH];
    char szPath[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    BOOL bFindFile = TRUE;
    BOOL bReturn = FALSE;
    
    // Get the program group path
    if (GetShellFolderPath(bCommon ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS,
        szPath, MAX_PATH))
    {
        // Append \*.* for FindFirstFile
        StringCchCopy(szTemp, MAX_PATH, "\\");
        StringCchCat(szTemp, MAX_PATH, strGroupName);
        StringCchCat(szPath, MAX_PATH, szTemp);
        
        StringCchCopy(szTemp, MAX_PATH, szPath);
        StringCchCat(szTemp, MAX_PATH, "\\*.*");
        
        // Use FindFirstFile to list dir. contents
        hFind = FindFirstFile(szTemp, &FindData);
        
        // kill them all off
        while ((INVALID_HANDLE_VALUE != hFind) && bFindFile)
        {
            if (*(FindData.cFileName) != '.')
            {
                PathAppend(szTemp, FindData.cFileName);
                DeleteFile(szTemp);
            }
            bFindFile = FindNextFile(hFind, &FindData);
        }
        
        FindClose(hFind);
        
        bReturn = RemoveDirectory(szPath);
    }
    
    return bReturn;
}

// helper function to do allocation for the list of program groups
BOOL DDEWorker::_ListGroupsHelper(
    HANDLE hHeap, char* szPath, LPVOID& pGroupList, UINT& ulSize)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    BOOL bFindFile = TRUE;
    char* pszTemp = NULL;
    BOOL bReturn = FALSE;
    
    // allocate the group list with base zero terminator if NULL
    if (NULL == pGroupList)
    {
        pGroupList = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 1);
    }
    
    // check allocation
    if (pGroupList)
    {
        // Get the first one
        hFind = FindFirstFile(szPath, &FindData);
        
        // iterate through the files
        // copy their names into a string in which each field is terminated
        // by \r\n
        while ((INVALID_HANDLE_VALUE != hFind) && bFindFile)
        {
            if (*(FindData.cFileName) != '.' &&
                (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                ULONG uLen = (ULONG)strlen(FindData.cFileName);
                
                pszTemp = (char*) HeapReAlloc(hHeap, HEAP_ZERO_MEMORY,
                    pGroupList, ulSize + uLen + 2);
                
                if (NULL == pszTemp)
                {
                    // Cleanup
                    FindClose(hFind);
                    bReturn = FALSE;
                    break;
                }
                
                CopyMemory(&pszTemp[ulSize], FindData.cFileName, uLen);
                CopyMemory(&pszTemp[ulSize + uLen], "\r\n", 2);
                pGroupList = (LPVOID) pszTemp;
                ulSize += uLen + 2;
                bReturn = TRUE;
            }
            // continue to next
            bFindFile = FindNextFile(hFind, &FindData);
        }
        
        // Cleanup
        FindClose(hFind);
    }
    
    return bReturn;
}


// kill an item
BOOL DDEWorker::_DeleteItem(LPCTSTR strItem)
{
    char szPath[MAX_PATH];
    BOOL bReturn = FALSE;
    
    if (m_szCurrentGroup[0])
    {
        StringCchPrintf(szPath, MAX_PATH, "%s\\%s", m_szCurrentGroup, strItem);
        bReturn = DeleteFile(szPath);
    }
    
    return bReturn;
}

// Add an item
BOOL DDEWorker::_AddItem(
    LPCTSTR strCmdLine, LPCTSTR strDescription, LPCTSTR strIconPath,
    int nIconIndex, LPCTSTR strDefDir, WORD dwHotKey, BOOL bMinimize)
{
    char szPath[MAX_PATH];
    HRESULT hr;
    IShellLink* pShellLink;
    IPersistFile* pPersistFile;
    BOOL bReturn = FALSE;
    
    // check that we've set a current group via showgroup or creategroup
    if (m_szCurrentGroup[0])
    {
        char szDesc[MAX_PATH];
        char szArgs[MAX_PATH];
        char szCmd[MAX_PATH];
        
        PathQuoteSpaces((char*)strCmdLine);
        
        LPCTSTR strArgs = PathGetArgs(strCmdLine);
        if (strArgs)
        {
            StringCchCopy(szArgs, MAX_PATH, strArgs);
            PathRemoveArgs((char*)strCmdLine);
        }
        
        PathUnquoteSpaces((char*) strCmdLine);
        StringCchCopy(szCmd, MAX_PATH, strCmdLine);
        
        if (strDescription)
        {
            StringCchCopy(szDesc, MAX_PATH, PathFindFileName(strCmdLine));
            PathRemoveExtension(szDesc);
        }
        else
        {
            PathUnquoteSpaces((char*) strDescription);
            StringCchCopy(szDesc, MAX_PATH, strDescription);
        }
        
        StringCchPrintf(szPath, MAX_PATH,
            "%s\\%s.lnk", m_szCurrentGroup, szDesc);
        
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                              IID_IShellLink, (LPVOID*) & pShellLink);
        if (SUCCEEDED(hr))
        {
            // Set the shortcut info.
            pShellLink->SetDescription(szDesc);
            pShellLink->SetHotkey(dwHotKey);
            pShellLink->SetWorkingDirectory(strDefDir);
            pShellLink->SetShowCmd(bMinimize ? SW_SHOWMINIMIZED : SW_SHOWNORMAL);
            pShellLink->SetArguments(szArgs);
            pShellLink->SetPath(szCmd);
            
            if (strIconPath)
            {
                pShellLink->SetIconLocation(strIconPath, nIconIndex);
            }
            
            // Save it.
            hr = pShellLink->QueryInterface(IID_IPersistFile,
                (LPVOID*)&pPersistFile);
            
            if (SUCCEEDED(hr))
            {
                WCHAR wsz[MAX_PATH];
                
                // Ensure that the string is ANSI.
                MultiByteToWideChar(CP_ACP, 0, szPath, -1, wsz, MAX_PATH);
                
                // Save the link by calling IPersistFile::Save.
                hr = pPersistFile->Save(wsz, TRUE);
                if (SUCCEEDED(hr))
                {
                    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH, m_szCurrentGroup, 0);
                    SHChangeNotify(SHCNE_CREATE, SHCNF_PATH, szPath, 0);
                    bReturn = TRUE;
                }
                pPersistFile->Release();
            }
            
            pShellLink->Release();
        }
    }
    
    return bReturn;
}
