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
#include "lsapiinit.h"
#include "lsapi.h"
#include "../utility/core.hpp"
#include <time.h>

LSAPIInit g_LSAPIManager;


LSAPIInit::LSAPIInit()
:m_bmBangManager(NULL)
,m_smSettingsManager(NULL)
,m_hLitestepWnd(NULL)
,m_bIsInitialized(false)
{
    m_dwMainThreadID = GetCurrentThreadId();
    
    m_szLitestepPath[0] = '\0';
    m_szRcPath[0] = '\0';
}

LSAPIInit::~LSAPIInit()
{
    m_bIsInitialized = false;
    delete m_bmBangManager;
    delete m_smSettingsManager;
}

void LSAPIInit::Initialize(LPCSTR pszLitestepPath, LPCSTR pszRcPath)
{
    try
    {
        // Error if called again
        if (IsInitialized())
        {
            throw LSAPIException(LSAPI_ERROR_RECURRENT);
        }
        
        // Do not allow any thread but the original to load us
        if (GetCurrentThreadId() != GetMainThreadID())
        {
            throw LSAPIException(LSAPI_ERROR_INVALIDTHREAD);
        }
        
        // Copy over the strings
        if (FAILED(StringCchCopy(m_szLitestepPath, MAX_PATH, pszLitestepPath)))
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        if (FAILED(StringCchCopy(m_szRcPath, MAX_PATH, pszRcPath)))
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        // Create the Bang Manager
        m_bmBangManager = new BangManager;
        
        if (!m_bmBangManager)
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        // Create the Settings Manager
        m_smSettingsManager = new SettingsManager();
        
        if (!m_smSettingsManager)
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        // Signal that we are initialized before continuing, so that
        // the LSAPI is available during setup
        m_bIsInitialized = true;
        
        // Initialize default variables
        setLitestepVars();
        
        // Load the default RC config file
        m_smSettingsManager->ParseFile(m_szRcPath);
        
        // Add our internal bang commands to the Bang Manager.
        SetupBangs();
    }
    catch(LSAPIException& lse)
    {
        if (LSAPI_ERROR_RECURRENT != lse.Type())
        {
            m_bIsInitialized = false;
            
            delete m_smSettingsManager;
            m_smSettingsManager = NULL;
            delete m_bmBangManager;
            m_bmBangManager = NULL;
        }
        
        throw; //rethrow
    }
}

void LSAPIInit::ReloadBangs()
{
    if (!IsInitialized())
    {
        throw LSAPIException(LSAPI_ERROR_NOTINITIALIZED);
    }
    
    m_bmBangManager->ClearBangCommands();
    SetupBangs();
}

void LSAPIInit::ReloadSettings()
{
    if (!IsInitialized())
    {
        throw LSAPIException(LSAPI_ERROR_NOTINITIALIZED);
    }
    
    m_bIsInitialized = false;
    
    delete m_smSettingsManager;
    m_smSettingsManager = NULL;
    
    m_smSettingsManager = new SettingsManager();
    
    if (!m_smSettingsManager)
    {
        throw LSAPIException(LSAPI_ERROR_GENERAL);
    }
    
    m_bIsInitialized = true;
    
    // Reinitialize default variables
    setLitestepVars();
    
    // Reload the default RC config file
    m_smSettingsManager->ParseFile(m_szRcPath);
}

void LSAPIInit::setLitestepVars()
{
    char szTemp[MAX_PATH];
    DWORD dwLength = MAX_PATH;
    
    // just using a shorter name, no real reason to re-assign.
    SettingsManager *pSM = m_smSettingsManager;
    
    // Set the variable "litestepdir" since it was never set
    if (SUCCEEDED(StringCchCopy(szTemp, MAX_PATH, m_szLitestepPath)))
    {
        PathAddBackslashEx(szTemp, MAX_PATH);
        PathQuoteSpaces(szTemp);
        pSM->SetVariable("litestepdir", szTemp);
    }
    
    if (GetWindowsDirectory(szTemp, MAX_PATH))
    {
        PathAddBackslashEx(szTemp, MAX_PATH);
        pSM->SetVariable("windir", szTemp);
    }
    
    if (GetUserName(szTemp, &dwLength))
    {
        PathQuoteSpaces(szTemp);
        pSM->SetVariable("username", szTemp);
    }
    
    pSM->SetVariable("bitbucket", "::{645FF040-5081-101B-9F08-00AA002F954E}");
    pSM->SetVariable("documents", "::{450D8FBA-AD25-11D0-98A8-0800361B1103}");
    pSM->SetVariable("drives", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
    pSM->SetVariable("network", "::{208D2C60-3AEA-1069-A2D7-08002B30309D}");
    pSM->SetVariable("controls", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}");
    pSM->SetVariable("dialup", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{992CFFA0-F557-101A-88EC-00DD010CCC48}");
    pSM->SetVariable("networkanddialup", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}");
    pSM->SetVariable("printers", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{2227A280-3AEA-1069-A2DE-08002B30309D}");
    pSM->SetVariable("scheduled", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{D6277990-4C6A-11CF-8D87-00AA0060F5BF}");
    pSM->SetVariable("admintools", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D20EA4E1-3957-11d2-A40B-0C5020524153}");
    
    setShellFolderVariable("quicklaunch", LS_CSIDL_QUICKLAUNCH);
    setShellFolderVariable("commondesktopdir", CSIDL_COMMON_DESKTOPDIRECTORY);
    setShellFolderVariable("commonfavorites", CSIDL_COMMON_FAVORITES);
    setShellFolderVariable("commonprograms", CSIDL_COMMON_PROGRAMS);
    setShellFolderVariable("commonstartmenu", CSIDL_COMMON_STARTMENU);
    setShellFolderVariable("commonstartup", CSIDL_COMMON_STARTUP);
    setShellFolderVariable("cookies", CSIDL_COOKIES);
    setShellFolderVariable("desktop", CSIDL_DESKTOP);
    setShellFolderVariable("desktopdir", CSIDL_DESKTOPDIRECTORY);
    setShellFolderVariable("favorites", CSIDL_FAVORITES);
    setShellFolderVariable("fonts", CSIDL_FONTS);
    setShellFolderVariable("history", CSIDL_HISTORY);
    setShellFolderVariable("internet", CSIDL_INTERNET);
    setShellFolderVariable("internetcache", CSIDL_INTERNET_CACHE);
    setShellFolderVariable("nethood", CSIDL_NETHOOD);
    setShellFolderVariable("documentsdir", CSIDL_PERSONAL);
    setShellFolderVariable("printhood", CSIDL_PRINTHOOD);
    setShellFolderVariable("programs", CSIDL_PROGRAMS);
    setShellFolderVariable("recent", CSIDL_RECENT);
    setShellFolderVariable("sendto", CSIDL_SENDTO);
    setShellFolderVariable("startmenu", CSIDL_STARTMENU);
    setShellFolderVariable("startup", CSIDL_STARTUP);
    setShellFolderVariable("templates", CSIDL_TEMPLATES);
    setShellFolderVariable("commonadmintoolsdir", CSIDL_COMMON_ADMINTOOLS);
    setShellFolderVariable("admintoolsdir", CSIDL_ADMINTOOLS);
    
    //
    // Set version identification variables
    //
    struct VersionToVariable
    {
        UINT uVersion;
        LPCTSTR pszVariable;
    }
    versions[] = \
    {
        { WINVER_WIN95,     _T("Win95")    },
        { WINVER_WIN98,     _T("Win98")    },
        { WINVER_WINME,     _T("WinME")    },
        
        { WINVER_WINNT4,    _T("WinNT4")   },
        { WINVER_WIN2000,   _T("Win2000")  },
        { WINVER_WINXP,     _T("WinXP")    },
        { WINVER_VISTA,     _T("WinVista") },
        { WINVER_WIN7,      _T("Win7")     },
        
        { WINVER_WIN2003,   _T("Win2003")  },
        { WINVER_WHS,       _T("Win2003")  },  // WHS is Win2003 in disguise
        { WINVER_WIN2008,   _T("Win2008")  }
    };
    
    UINT uVersion = GetWindowsVersion();
    
    for (size_t idx = 0; idx < COUNTOF(versions); ++idx)
    {
        if (versions[idx].uVersion == uVersion)
        {
            pSM->SetVariable(versions[idx].pszVariable, "true");
        }
        else
        {
            pSM->SetVariable(versions[idx].pszVariable, "false");
        }
    }
    
    if (IsOS(OS_NT))
    {
        pSM->SetVariable("Win9x", "false");
        pSM->SetVariable("WinNT", "true");
    }
    else
    {
        pSM->SetVariable("Win9x", "true");
        pSM->SetVariable("WinNT", "false");
    }

#if defined(_WIN64)
    pSM->SetVariable("Win64", "true");
#else
    if (IsOS(OS_WOW6432))
    {
        pSM->SetVariable("Win64", "true");
    }
    else
    {
        pSM->SetVariable("Win64", "false");
    }
#endif
    
    // screen resolution
    StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CXSCREEN));
    pSM->SetVariable("ResolutionX", szTemp);
    
    StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CYSCREEN));
    pSM->SetVariable("ResolutionY", szTemp);
    
    // build date/time from PE headers
    getCompileTime(szTemp, MAX_PATH);
    pSM->SetVariable("CompileDate", szTemp);
    
#if defined(LS_CUSTOM_INCLUDEFOLDER)
    pSM->SetVariable("IncludeFolder", "1");
#endif // LS_CUSTOM_INCLUDEFOLDER
}

bool LSAPIInit::setShellFolderVariable(LPCSTR pszVariable, int nFolder)
{
    bool bSuccess = false;
    char szPath[MAX_PATH] = { 0 };
    
    if (GetShellFolderPath(nFolder, szPath, MAX_PATH))
    {
        PathAddBackslashEx(szPath, MAX_PATH);
        PathQuoteSpaces(szPath);
        
        m_smSettingsManager->SetVariable(pszVariable, szPath);
        bSuccess = true;
    }
    
    return bSuccess;
}

// Gets the compiletime/date from the PE header
//
#define MakePtr(cast, ptr, addValue) \
    (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
void LSAPIInit::getCompileTime(LPSTR pszValue, size_t cchValue)
{
    IMAGE_DOS_HEADER* dosheader;
    IMAGE_NT_HEADERS* ntheader;
    time_t lsexetime;
    time_t lsapitime;
    time_t compiletime;
    
    // Get the litestep.exe build time.
    dosheader = (IMAGE_DOS_HEADER*)GetModuleHandle(NULL);
    ASSERT(dosheader);
    ASSERT(dosheader->e_magic == IMAGE_DOS_SIGNATURE);
    ntheader = MakePtr(IMAGE_NT_HEADERS*, dosheader, dosheader->e_lfanew);
    ASSERT(ntheader);
    lsexetime = (time_t)(ntheader->FileHeader.TimeDateStamp);
    
    // Get the lsapi.dll build time (TODO: don't hardcode "lsapi.dll")
    dosheader = (IMAGE_DOS_HEADER*)GetModuleHandle(_T("lsapi.dll"));
    ASSERT(dosheader);
    ASSERT(dosheader->e_magic == IMAGE_DOS_SIGNATURE);
    ntheader = MakePtr(IMAGE_NT_HEADERS*, dosheader, dosheader->e_lfanew);
    ASSERT(ntheader);
    lsapitime = (time_t)(ntheader->FileHeader.TimeDateStamp);
    
    compiletime = std::max(lsexetime, lsapitime);
    tm* timeStruct = gmtime(&compiletime);
    
    if (timeStruct)
    {
        _tcsftime(pszValue, cchValue,
            _T("\"Compiled on %b %d %Y at %H:%M:%S UTC\""), timeStruct);
    }
    else
    {
        StringCchPrintf(pszValue, cchValue,
            "\"Compiled at an unknown time\"");
    }
}
