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
#include "lsapiinit.h"
#include "lsapi.h"
#include "../utility/core.hpp"
#include <time.h>
#include <algorithm>

LSAPIInit g_LSAPIManager;


LSAPIInit::LSAPIInit()
:m_bmBangManager(NULL)
,m_smSettingsManager(NULL)
,m_hLitestepWnd(NULL)
,m_bIsInitialized(false)
{
    m_dwMainThreadID = GetCurrentThreadId();
    
    m_wzLitestepPath[0] = L'\0';
    m_wzRcPath[0] = L'\0';
}


LSAPIInit::~LSAPIInit()
{
    m_bIsInitialized = false;
    delete m_bmBangManager;
    delete m_smSettingsManager;
}


void LSAPIInit::Initialize(LPCWSTR pwzLitestepPath, LPCWSTR pwzRcPath)
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
        if (FAILED(StringCchCopyW(m_wzLitestepPath, MAX_PATH, pwzLitestepPath)))
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        if (FAILED(StringCchCopyW(m_wzRcPath, MAX_PATH, pwzRcPath)))
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
        m_smSettingsManager->ParseFile(m_wzRcPath);
        
        // Add our internal bang commands to the Bang Manager.
        SetupBangs();
    }
    catch(LSAPIException& lse)
    {
        if (LSAPI_ERROR_RECURRENT != lse.Type())
        {
            m_bIsInitialized = false;
            
            delete m_smSettingsManager;
            m_smSettingsManager = nullptr;
            delete m_bmBangManager;
            m_bmBangManager = nullptr;
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
    m_smSettingsManager->ParseFile(m_wzRcPath);
}


void LSAPIInit::setLitestepVars()
{
    wchar_t wzTemp[MAX_PATH];
    DWORD dwLength = MAX_PATH;
    
    // just using a shorter name, no real reason to re-assign.
    SettingsManager *pSM = m_smSettingsManager;
    
    // Set the variable "litestepdir" since it was never set
    if (SUCCEEDED(StringCchCopyW(wzTemp, MAX_PATH, m_wzLitestepPath)))
    {
        PathAddBackslashEx(wzTemp, MAX_PATH);
        PathQuoteSpacesW(wzTemp);
        pSM->SetVariable(L"litestepdir", wzTemp);
    }
    
    if (GetWindowsDirectoryW(wzTemp, MAX_PATH))
    {
        PathAddBackslashEx(wzTemp, MAX_PATH);
        pSM->SetVariable(L"windir", wzTemp);
    }
    
    if (GetUserNameW(wzTemp, &dwLength))
    {
        PathQuoteSpacesW(wzTemp);
        pSM->SetVariable(L"username", wzTemp);
    }

    pSM->SetVariable(L"nl", L"\n", true);
    pSM->SetVariable(L"cr", L"\r", true);
    pSM->SetVariable(L"dollar", L"$", true);
    pSM->SetVariable(L"at", L"@", true);
    
    pSM->SetVariable(L"bitbucket", L"::{645FF040-5081-101B-9F08-00AA002F954E}");
    pSM->SetVariable(L"documents", L"::{450D8FBA-AD25-11D0-98A8-0800361B1103}");
    pSM->SetVariable(L"drives", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
    pSM->SetVariable(L"network", L"::{208D2C60-3AEA-1069-A2D7-08002B30309D}");
    pSM->SetVariable(L"controls", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}");
    pSM->SetVariable(L"dialup", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{992CFFA0-F557-101A-88EC-00DD010CCC48}");
    pSM->SetVariable(L"networkanddialup", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}");
    pSM->SetVariable(L"printers", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{2227A280-3AEA-1069-A2DE-08002B30309D}");
    pSM->SetVariable(L"scheduled", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{D6277990-4C6A-11CF-8D87-00AA0060F5BF}");
    pSM->SetVariable(L"admintools", L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D20EA4E1-3957-11d2-A40B-0C5020524153}");
    
    setShellFolderVariable(L"quicklaunch", LS_CSIDL_QUICKLAUNCH);
    setShellFolderVariable(L"commondesktopdir", CSIDL_COMMON_DESKTOPDIRECTORY);
    setShellFolderVariable(L"commonfavorites", CSIDL_COMMON_FAVORITES);
    setShellFolderVariable(L"commonprograms", CSIDL_COMMON_PROGRAMS);
    setShellFolderVariable(L"commonstartmenu", CSIDL_COMMON_STARTMENU);
    setShellFolderVariable(L"commonstartup", CSIDL_COMMON_STARTUP);
    setShellFolderVariable(L"cookies", CSIDL_COOKIES);
    setShellFolderVariable(L"desktop", CSIDL_DESKTOP);
    setShellFolderVariable(L"desktopdir", CSIDL_DESKTOPDIRECTORY);
    setShellFolderVariable(L"favorites", CSIDL_FAVORITES);
    setShellFolderVariable(L"fonts", CSIDL_FONTS);
    setShellFolderVariable(L"history", CSIDL_HISTORY);
    setShellFolderVariable(L"internet", CSIDL_INTERNET);
    setShellFolderVariable(L"internetcache", CSIDL_INTERNET_CACHE);
    setShellFolderVariable(L"nethood", CSIDL_NETHOOD);
    setShellFolderVariable(L"documentsdir", CSIDL_PERSONAL);
    setShellFolderVariable(L"printhood", CSIDL_PRINTHOOD);
    setShellFolderVariable(L"programs", CSIDL_PROGRAMS);
    setShellFolderVariable(L"recent", CSIDL_RECENT);
    setShellFolderVariable(L"sendto", CSIDL_SENDTO);
    setShellFolderVariable(L"startmenu", CSIDL_STARTMENU);
    setShellFolderVariable(L"startup", CSIDL_STARTUP);
    setShellFolderVariable(L"templates", CSIDL_TEMPLATES);
    setShellFolderVariable(L"commonadmintoolsdir", CSIDL_COMMON_ADMINTOOLS);
    setShellFolderVariable(L"admintoolsdir", CSIDL_ADMINTOOLS);
    
    //
    // Set version identification variables
    //
    struct VersionToVariable
    {
        UINT uVersion;
        LPCWSTR pszVariable;
    }
    versions[] = \
    {
        { WINVER_WIN95,     L"Win95"      },
        { WINVER_WIN98,     L"Win98"      },
        { WINVER_WINME,     L"WinME"      },
        
        { WINVER_WINNT4,    L"WinNT4"     },
        { WINVER_WIN2000,   L"Win2000"    },
        { WINVER_WINXP,     L"WinXP"      },
        { WINVER_VISTA,     L"WinVista"   },
        { WINVER_WIN7,      L"Win7"       },
        { WINVER_WIN8,      L"Win8"       },
        { WINVER_WIN81,     L"Win81"      },
        
        { WINVER_WIN2003,   L"Win2003"    },
        { WINVER_WHS,       L"Win2003"    },  // WHS is Win2003 in disguise
        { WINVER_WIN2008,   L"Win2008"    },
        { WINVER_WIN2008R2, L"Win2008R2"  },
        { WINVER_WIN2012,   L"Win2012"    },
        { WINVER_WIN2012R2, L"Win2012R2"  }
    };
    
    UINT uVersion = GetWindowsVersion();
    
    for (size_t idx = 0; idx < COUNTOF(versions); ++idx)
    {
        if (versions[idx].uVersion == uVersion)
        {
            pSM->SetVariable(versions[idx].pszVariable, L"true");
        }
        else
        {
            pSM->SetVariable(versions[idx].pszVariable, L"false");
        }
    }
    
    if (IsOS(OS_NT))
    {
        pSM->SetVariable(L"Win9x", L"false");
        pSM->SetVariable(L"WinNT", L"true");
    }
    else
    {
        pSM->SetVariable(L"Win9x", L"true");
        pSM->SetVariable(L"WinNT", L"false");
    }

#if defined(_WIN64)
    pSM->SetVariable(L"Win64", L"true");
#else
    if (IsOS(OS_WOW6432))
    {
        pSM->SetVariable(L"Win64", L"true");
    }
    else
    {
        pSM->SetVariable(L"Win64", L"false");
    }
#endif
    
    // screen resolution
    StringCchPrintfW(wzTemp, MAX_PATH, L"%d", GetSystemMetrics(SM_CXSCREEN));
    pSM->SetVariable(L"ResolutionX", wzTemp);
    
    StringCchPrintfW(wzTemp, MAX_PATH, L"%d", GetSystemMetrics(SM_CYSCREEN));
    pSM->SetVariable(L"ResolutionY", wzTemp);
    
    // build date/time from PE headers
    getCompileTime(wzTemp, MAX_PATH);
    pSM->SetVariable(L"CompileDate", wzTemp);
    
#if defined(LS_CUSTOM_INCLUDEFOLDER)
    pSM->SetVariable(L"IncludeFolder", L"1");
#endif // LS_CUSTOM_INCLUDEFOLDER
}


bool LSAPIInit::setShellFolderVariable(LPCWSTR pwzVariable, int nFolder)
{
    bool bSuccess = false;
    wchar_t wzPath[MAX_PATH] = { 0 };
    
    if (GetShellFolderPath(nFolder, wzPath, MAX_PATH))
    {
        PathAddBackslashEx(wzPath, MAX_PATH);
        PathQuoteSpacesW(wzPath);
        
        m_smSettingsManager->SetVariable(pwzVariable, wzPath);
        bSuccess = true;
    }
    
    return bSuccess;
}


// Gets the compiletime/date from the PE header
//
#define MakePtr(cast, ptr, addValue) \
    (cast)((DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))
void LSAPIInit::getCompileTime(LPWSTR pwzValue, size_t cchValue)
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
    tm timeStruct;
    
    if (gmtime_s(&timeStruct, &compiletime) == 0)
    {
        wcsftime(pwzValue, cchValue,
            L"\"Compiled on %b %d %Y at %H:%M:%S UTC\"", &timeStruct);
    }
    else
    {
        StringCchPrintfW(pwzValue, cchValue,
            L"\"Compiled at an unknown time\"");
    }
}
