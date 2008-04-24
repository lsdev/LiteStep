/*

This is a part of the LiteStep Shell Source code.

Copyright (C) 2006 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/ 
/****************************************************************************
****************************************************************************/
#include "lsapiinit.h"
#include "lsapi.h"
#include "bangs.h"
#include "../utility/shellhlp.h"
#include "../utility/core.hpp"

LSAPIInit g_LSAPIManager;


LSAPIInit::LSAPIInit()
:m_smSettingsManager(NULL)
,m_bmBangManager(NULL)
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
        if(IsInitialized())
        {
            throw LSAPIException(LSAPI_ERROR_RECURRENT);
        }
        
        // Do not allow any thread but the original to load us
        if(GetCurrentThreadId() != GetMainThreadID())
        {
            throw LSAPIException(LSAPI_ERROR_INVALIDTHREAD);
        }
        
        // Copy over the strings
        if(FAILED(StringCchCopy(m_szLitestepPath, MAX_PATH, pszLitestepPath)))
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        if(FAILED(StringCchCopy(m_szRcPath, MAX_PATH, pszRcPath)))
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        // Create the Bang Manager
        m_bmBangManager = new BangManager;
        
        if(!m_bmBangManager)
        {
            throw LSAPIException(LSAPI_ERROR_GENERAL);
        }
        
        // Create the Settings Manager
        m_smSettingsManager = new SettingsManager();
        
        if(!m_smSettingsManager)
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
        if(LSAPI_ERROR_RECURRENT != lse.Type())
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
    if(!IsInitialized())
    {
        throw LSAPIException(LSAPI_ERROR_NOTINITIALIZED);
    }
    
    m_bmBangManager->ClearBangCommands();
    SetupBangs();
}

void LSAPIInit::ReloadSettings()
{
    if(!IsInitialized())
    {
        throw LSAPIException(LSAPI_ERROR_NOTINITIALIZED);
    }
    
    m_bIsInitialized = false;
    
    delete m_smSettingsManager;
    m_smSettingsManager = NULL;
    
    m_smSettingsManager = new SettingsManager();
    
    if(!m_smSettingsManager)
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
    OSVERSIONINFO OsVersionInfo;
    
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
    
    if (GetShellFolderPath(CSIDL_APPDATA, szTemp, MAX_PATH))
    {
        PathAppend(szTemp, "Microsoft\\Internet Explorer\\Quick Launch\\");
        PathQuoteSpaces(szTemp);
        
        pSM->SetVariable("quicklaunch", szTemp);
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
    
    OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);
    GetVersionEx(&OsVersionInfo);
    
    // Default platform conditionals to FALSE
    pSM->SetVariable("Win9x", "false");
    pSM->SetVariable("WinME", "false");
    pSM->SetVariable("Win98", "false");
    pSM->SetVariable("Win95", "false");
    pSM->SetVariable("WinNT", "false");
    pSM->SetVariable("Win2003", "false");
    pSM->SetVariable("WinXP", "false");
    pSM->SetVariable("Win2000", "false");
    pSM->SetVariable("WinNT4", "false");
    
    // Now set the correct platform conditional to TRUE
    if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        // Any Win9x-series OS
        pSM->SetVariable("Win9x", "true");
        
        if (OsVersionInfo.dwMinorVersion >= 90)         // Windows ME (4.90)
        {
            pSM->SetVariable("WinME", "true");
        }
        else if (OsVersionInfo.dwMinorVersion >= 10)    // Windows 98 (4.10)
        {
            pSM->SetVariable("Win98", "true");
        }
        else                                            // Windows 95 (4.00)
        {
            pSM->SetVariable("Win95", "true");
        }
    }
    else if (OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        // Any WinNT-series OS
        pSM->SetVariable("WinNT", "true");
        
        if (OsVersionInfo.dwMajorVersion == 5)
        {
            if (OsVersionInfo.dwMinorVersion >= 2)      // Windows 2003 (5.2)
            {
                pSM->SetVariable("Win2003", "true");
            }
            else if (OsVersionInfo.dwMinorVersion >= 1) // Windows XP (5.1)
            {
                pSM->SetVariable("WinXP", "true");
            }
            else                                        // Windows 2000 (5.0)
            {
                pSM->SetVariable("Win2000", "true");
            }
        }
        else if (OsVersionInfo.dwMajorVersion >= 4)     // Windows NT 4.0
        {
            pSM->SetVariable("WinNT4", "true");
        }
    }
    
    // screen resolution
    StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CXSCREEN));
    pSM->SetVariable("ResolutionX", szTemp);
    
    StringCchPrintf(szTemp, MAX_PATH, "%d", GetSystemMetrics(SM_CYSCREEN));
    pSM->SetVariable("ResolutionY", szTemp);
    
    StringCchPrintf(szTemp, MAX_PATH, "\"%s\"", __DATE__);
    pSM->SetVariable("CompileDate", szTemp);
    
#ifdef LS_CUSTOM_INCLUDEFOLDER
    pSM->SetVariable("IncludeFolder", "1");
#endif // LS_CUSTOM_INCLUDEFOLDER
}

bool LSAPIInit::setShellFolderVariable(LPCSTR pszVariable, int nFolder)
{
    char szPath[MAX_PATH] = { 0 };
    
    bool bReturn = GetShellFolderPath(nFolder, szPath, MAX_PATH);
    
    if (bReturn)
    {
        PathAddBackslashEx(szPath, MAX_PATH);
        PathQuoteSpaces(szPath);
        
        m_smSettingsManager->SetVariable(pszVariable, szPath);
    }
    
    return bReturn;    
}
