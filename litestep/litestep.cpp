/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2005 The LiteStep Development Team

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

#include "litestep.h"

// Misc Helpers
#include "RecoveryMenu.h"
#include "StartupRunner.h"
#include "../lsapi/ThreadedBangCommand.h"
#include "../utility/macros.h"
#include "../utility/shellhlp.h"

// Services
#include "DDEService.h"
#include "DDEStub.h"
#include "TrayService.h"

// Managers
#include "MessageManager.h"
#include "ModuleManager.h"

// Misc Helpers
#include "DataStore.h"

// STL headers
#include <algorithm>

#include "../utility/core.hpp"


// namespace stuff
using std::for_each;
using std::mem_fun;


// Parse the command line
bool ParseCmdLine(LPCSTR pszCmdLine);
HRESULT ExecuteCmdLineBang(LPCSTR pszCommand, LPCSTR pszArgs);

CLiteStep gLiteStep;
CHAR szAppPath[MAX_PATH];
CHAR szRcPath[MAX_PATH];

enum StartupMode
{
    STARTUP_DONT_RUN  = -1,
    STARTUP_DEFAULT   = 0,  // run only if first time
    STARTUP_FORCE_RUN = TRUE
};

int g_nStartupMode = STARTUP_DEFAULT;


//
// ExecuteCmdLineBang
//
HRESULT ExecuteCmdLineBang(LPCSTR pszCommand, LPCSTR pszArgs)
{
    ASSERT_ISNOTNULL(pszCommand);
    
    HRESULT hr = E_FAIL;
    HWND hWnd = FindWindow(szMainWindowClass, szMainWindowTitle);
    
    if (IsWindow(hWnd))
    {
        LMBANGCOMMAND bangCommand;
        bangCommand.cbSize = sizeof(LMBANGCOMMAND);
        bangCommand.hWnd = NULL;
        
        hr = StringCchCopy(bangCommand.szCommand, MAX_BANGCOMMAND, pszCommand);
        
        if (SUCCEEDED(hr))
        {
            if (pszArgs)
            {
                hr = StringCchCopy(bangCommand.szArgs, MAX_BANGARGS, pszArgs);
            }
            else
            {
                bangCommand.szArgs[0] = '\0';
            }
        }
        
        if (SUCCEEDED(hr))
        {
            COPYDATASTRUCT cds = { 0 };
            
            cds.cbData = sizeof(LMBANGCOMMAND);
            cds.dwData = LM_BANGCOMMAND;
            cds.lpData = &bangCommand;
            
            SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&cds);
            hr = S_OK;
        }
    }
    
    return hr;
}


//
// ParseCmdLine(LPCSTR pszCmdLine)
//
bool ParseCmdLine(LPCSTR pszCmdLine)
{
    if (IsValidStringPtr(pszCmdLine))
    {
        char szToken[MAX_LINE_LENGTH];
        LPCSTR pszNextToken = pszCmdLine;
        
        while (GetToken(pszNextToken, szToken, &pszNextToken, false))
        {
            switch (szToken[0])
            {
                case '-':
                {
                    if (!stricmp(szToken, "-nostartup"))
                    {
                        g_nStartupMode = STARTUP_DONT_RUN;
                    }
                    else if (!stricmp(szToken, "-startup"))
                    {
                        g_nStartupMode = STARTUP_FORCE_RUN;
                    }
                }
                break;
                
                case '!':
                {
                    ExecuteCmdLineBang(szToken, pszNextToken);
                    return false;
                }
                break;
                
                default:
                {
                    if (PathFileExists(szToken))
                    {
                        if (strchr(szToken, '\\'))
                        {
                            StringCchCopy(szRcPath, MAX_PATH, szToken);
                        }
                        else
                        {
                            StringCchPrintfEx(szRcPath, MAX_PATH, NULL, NULL, STRSAFE_NULL_ON_FAILURE, "%s\\%s", szAppPath, szToken);
                        }
                    }
                }
                break;
            }
        }
    }
    
    return true;
}


//
//
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
                   LPSTR lpCmdLine, int /* nCmdShow */)
{
    HRESULT hr = S_OK;
    
    // Determine our application's path
    if (LSGetModuleFileName(hInstance, szAppPath, sizeof(szAppPath)))
    {
        PathRemoveFileSpec(szAppPath);
        PathAddBackslashEx(szAppPath, MAX_PATH);
    }
    PathCombine(szRcPath, szAppPath, "step.rc");
    
    // Parse command line, setting appropriate variables
    if (!ParseCmdLine(lpCmdLine))
    {
        return 1;
    }
    
    // Tell the Welcome Screen to close
    // This has to be done before the first MessageBox call, else that box
    // would pop up "under" the welcome screen
    HANDLE hShellReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE,
        "msgina: ShellReadyEvent");
    
    if (hShellReadyEvent != NULL)
    {
        SetEvent(hShellReadyEvent);
        CloseHandle(hShellReadyEvent);
    }
    
    // If we can't find "step.rc", there's no point in proceeding
    if (!PathFileExists(szRcPath))
    {
        RESOURCE_STREX(hInstance, IDS_LITESTEP_ERROR2, resourceTextBuffer, MAX_LINE_LENGTH,
                       "Unable to find the file \"%s\".\nPlease verify the location of the file, and try again.", szRcPath);
        MessageBox(NULL, resourceTextBuffer, "LiteStep", MB_TOPMOST | MB_ICONEXCLAMATION);
        return 2;
    }
    
    // Check for previous instance
    HANDLE hMutex = CreateMutex(NULL, FALSE, "LiteStep");
    
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Prevent multiple instances of LiteStep
        RESOURCE_STR(hInstance, IDS_LITESTEP_ERROR1,
                     "A previous instance of LiteStep was detected.\nAre you sure you want to continue?");
        if (IDNO == MessageBox(NULL, resourceTextBuffer, "LiteStep", MB_TOPMOST | MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2))
        {
            hr = E_ABORT;
        }
    }
    
    if (SUCCEEDED(hr))
    {
        hr = gLiteStep.Start(szAppPath, szRcPath, hInstance, g_nStartupMode);
    }
    
    CloseHandle(hMutex);
    
    return HRESULT_CODE(hr);
}


//
// CLiteStep()
//
CLiteStep::CLiteStep()
{
    m_hInstance = NULL;
    m_bAutoHideModules = false;
    m_bAppIsFullScreen = false;
    m_hMainWindow = NULL;
    WM_ShellHook = 0;
    m_pModuleManager = NULL;
    m_pDataStoreManager = NULL;
    m_pMessageManager = NULL;
    m_bSignalExit = false;
    m_pTrayService = NULL;
    m_BlockRecycle = 0;
}


//
//
//
CLiteStep::~CLiteStep()
{
}


//
// Start(LPCSTR pszAppPath, LPCSTR pszRcPath, HINSTANCE hInstance, int nStartupMode)
//
HRESULT CLiteStep::Start(LPCSTR pszAppPath, LPCSTR pszRcPath, HINSTANCE hInstance, int nStartupMode)
{
    HRESULT hr;
    bool bUnderExplorer = false;
    
    m_sAppPath.assign(pszAppPath);  // could throw length_error
    m_sConfigFile.assign(pszRcPath);
    m_hInstance = hInstance;
    
    // Initialize OLE/COM
    OleInitialize(NULL);
    
    // before anything else, start the recovery menu thread
    DWORD dwRecoveryThreadID;
    HANDLE hRecoveryThread = CreateThread(NULL, 0, RecoveryThreadProc,
        (void*)m_hInstance, 0, &dwRecoveryThreadID);
    
    // configure the Win32 window manager to hide windows when they are minimized
    MINIMIZEDMETRICS mm = { 0 };
    mm.cbSize = sizeof(MINIMIZEDMETRICS);
    
    SystemParametersInfo(SPI_GETMINIMIZEDMETRICS, mm.cbSize, &mm, 0);
    
    if (!(mm.iArrange & ARW_HIDE))
    {
        mm.iArrange |= ARW_HIDE;
        SystemParametersInfo(SPI_SETMINIMIZEDMETRICS, mm.cbSize, &mm, 0);
    }
    
    SetupSettingsManager(m_sAppPath.c_str(), m_sConfigFile.c_str());
    
    if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
        (nStartupMode != STARTUP_FORCE_RUN && !GetRCBool("LSNoStartup", FALSE)))
    {
        nStartupMode = STARTUP_DONT_RUN;
    }
    
    m_bAutoHideModules = GetRCBool("LSAutoHideModules", TRUE) ? true : false;
    
    // Check for explorer
    if (FindWindow("Shell_TrayWnd", NULL)) // Running under Exploder
    {
        if (GetRCBool("LSNoShellWarning", FALSE))
        {
            RESOURCE_STR(hInstance, IDS_LITESTEP_ERROR3,
                "Litestep is not able to load as the system shell.\n"
                "Another shell is already active.\n"
                "\n"
                "Continuing to load Litestep will disable specific system\n"
                "shell functions of Litestep and some features will not\n"
                "function properly such as icon notifications (systray),\n"
                "the desktop and some task managers.\n"
                "\n"
                "To disable this message, place 'LSNoShellWarning' in\n"
                "your step.rc.\n"
                "\n"
                "Continue to load Litestep?\n");
            RESOURCE_TITLE(hInstance, IDS_LITESTEP_TITLE_WARNING, "Warning");
            if (MessageBox(0, resourceTextBuffer, resourceTitleBuffer, MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST) == IDNO)
            {
                return E_ABORT;
            }
        }
        bUnderExplorer = true;
    }
    
    // Register Window Class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = CLiteStep::InternalWndProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = szMainWindowClass;
    
    if (!RegisterClassEx(&wc))
    {
        RESOURCE_MSGBOX_T(hInstance, IDS_LITESTEP_ERROR4,
                          "Error registering main Litestep window class.",
                          IDS_LITESTEP_TITLE_ERROR, "Error");
        
        return E_FAIL;
    }
    
    // Create our main window
    m_hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
        szMainWindowClass, szMainWindowTitle,
        0, 0, 0, 0,
        0, NULL, NULL,
        m_hInstance,
        (void*)this);
    
    // Start up everything
    if (m_hMainWindow)
    {
        // Set magic DWORD to prevent VWM from seeing main window
        SetWindowLong (m_hMainWindow, GWL_USERDATA, magicDWord);
        
        FARPROC (__stdcall * RegisterShellHook)(HWND, DWORD) =
            (FARPROC (__stdcall *)(HWND, DWORD))GetProcAddress(
            GetModuleHandle("SHELL32.DLL"), (LPCSTR)((long)0xB5));
        
        WM_ShellHook = RegisterWindowMessage("SHELLHOOK");
        
        if (RegisterShellHook)
        {
            RegisterShellHook(NULL, RSH_REGISTER);
            
            OSVERSIONINFO verInfo = { 0 };
            verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
            GetVersionEx(&verInfo);
            
            if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
            {
                // c0atzin's fix for 9x
                RegisterShellHook(m_hMainWindow, RSH_REGISTER);
            }
            else
            {
                RegisterShellHook(m_hMainWindow, RSH_TASKMAN);
            }
        }
        
        // Set Shell Window
        if (!bUnderExplorer && (GetRCBool("LSSetAsShell", TRUE)))
        {
            FARPROC (__stdcall * SetShellWindow)(HWND) = NULL;
            SetShellWindow = (FARPROC (__stdcall *)(HWND))GetProcAddress(GetModuleHandle("USER32.DLL"), "SetShellWindow");
            if (SetShellWindow)
            {
                SetShellWindow(m_hMainWindow);
            }
        }
        
        hr = _InitServices();
        if (SUCCEEDED(hr))
        {
            hr = _StartServices();
            // Quietly swallow service errors... in the future.. do something
        }
        
        hr = _InitManagers();
        if (SUCCEEDED(hr))
        {
            hr = _StartManagers();
            // Quietly swallow manager errors... in the future.. do something
        }
        
        // Run startup items if the SHIFT key is not down
        if (nStartupMode != STARTUP_DONT_RUN)
        {
            DWORD dwThread;
            
            CloseHandle(CreateThread(NULL, 0, StartupRunner::Run,
                (void*)nStartupMode, 0, &dwThread));
        }
        
        // Undocumented call: Shell Loading Finished
        SendMessage(GetDesktopWindow(), WM_USER, 0, 0);
        
        // Main message pump
        MSG message;
        /* Note: check m_bSignalExit first, so that if MessageHandler()
         * was called externally from a response to PeekMessage() we
         * know right away if there was a WM_QUIT in the queue, and
         * subsequently do not incorrectly call GetMessage() again. */
        while (!m_bSignalExit && GetMessage(&message, 0, 0, 0) > 0)
        {
            MessageHandler(message);
        }
        
        if (RegisterShellHook)
        {
            RegisterShellHook(m_hMainWindow, RSH_UNREGISTER);
        }
        
        _StopManagers();
        _CleanupManagers();
        
        _StopServices();
        _CleanupServices();
        
        // Destroy _main window
        DestroyWindow(m_hMainWindow);
        m_hMainWindow = NULL;
    }
    else
    {
        RESOURCE_MSGBOX_T(hInstance, IDS_LITESTEP_ERROR5,
                          "Error creating Litestep main application window.",
                          IDS_LITESTEP_TITLE_ERROR, "Error");
    }
    
    // Unreg class
    UnregisterClass(szMainWindowClass, m_hInstance);
    
    // deinitialize stepsettings
    DeleteSettingsManager();
    
    // Uninitialize OLE/COM
    OleUninitialize();
    
    // close the recovery thread: tell the thread to quit
    PostThreadMessage(dwRecoveryThreadID, WM_QUIT, 0, 0);
    // wait until the thread is done quitting, at most three seconds though
    if (WaitForSingleObject(hRecoveryThread, 3000) == WAIT_TIMEOUT)
    {
        TerminateThread(hRecoveryThread, 0);
    }
    // close the thread handle
    CloseHandle(hRecoveryThread);
    
    return S_OK;
}

//
//
//
int CLiteStep::MessageHandler(MSG &message)
{
    if(WM_QUIT == message.message)
    {
        m_bSignalExit = true;
        return 0;
    }
    
#if !defined(LS_NO_EXCEPTION)
    try
    {
#endif /* LS_NO_EXCEPTION */
        if (NULL == message.hwnd)
        {
            // Thread message
            switch (message.message)
            {
                case LM_THREAD_BANGCOMMAND:
                {
                    ThreadedBangCommand* pInfo = \
                        (ThreadedBangCommand*)message.wParam;
                    
                    if (NULL != pInfo)
                    {
                        pInfo->Execute();
                        pInfo->Release(); // check BangCommand.cpp for the reason
                    }
                }
                break;
                
                default:
                break;
            }
        }
        else
        {
            TranslateMessage(&message);
            DispatchMessage (&message);
        }
#if !defined(LS_NO_EXCEPTION)
    }
    catch(...)
    {
        // MessageBox(m_hMainWindow, "exception", "oops", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
    }
#endif /* LS_NO_EXCEPTION */
    
    return 0;
}

//
//
//
LRESULT CALLBACK CLiteStep::InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static CLiteStep* pLiteStep = NULL;
    
    if (uMsg == WM_CREATE)
    {
        pLiteStep = static_cast<CLiteStep*>(
            reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        
        ASSERT_ISNOTNULL(pLiteStep);
    }
    
    if (pLiteStep)
    {
        return pLiteStep->ExternalWndProc(hWnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//
//
//
LRESULT CLiteStep::ExternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lReturn = FALSE;
    
    switch (uMsg)
    {
        case WM_KEYDOWN:
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case LM_SHUTDOWN:
                case SC_CLOSE:
                {
                    ParseBangCommand(hWnd, "!ShutDown", NULL);
                }
                break;
                
                default:
                {
                    lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
                }
                break;
            }
        }
        break;
        
        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
        {
            lReturn = TRUE;
        }
        break;
        
        case LM_SYSTRAYREADY:
        {
            if (m_pTrayService)
            {
                lReturn = (LRESULT)m_pTrayService->SendSystemTray();
            }
        }
        break;
        
        case LM_SAVEDATA:
        {
            WORD wIdent = HIWORD(wParam);
            WORD wLength = LOWORD(wParam);
            void *pvData = (void *)lParam;
            if ((pvData != NULL) && (wLength > 0))
            {
                if (m_pDataStoreManager == NULL)
                {
                    m_pDataStoreManager = new DataStore();
                }
                if (m_pDataStoreManager)
                {
                    lReturn = m_pDataStoreManager->StoreData(wIdent, pvData, wLength);
                }
            }
        }
        break;
        
        case LM_RESTOREDATA:
        {
            WORD wIdent = HIWORD(wParam);
            WORD wLength = LOWORD(wParam);
            void *pvData = (void *)lParam;
            if ((pvData != NULL) && (wLength > 0))
            {
                if (m_pDataStoreManager)
                {
                    lReturn = m_pDataStoreManager->ReleaseData(wIdent, pvData, wLength);
                    if (m_pDataStoreManager->Count() == 0)
                    {
                        delete m_pDataStoreManager;
                        m_pDataStoreManager = NULL;
                    }
                }
            }
        }
        break;
        
        case LM_GETLSOBJECT:
        case LM_WINDOWLIST:
        case LM_MESSAGEMANAGER:
        case LM_DATASTORE:
        {
            ; // Obsolete Message, return 0
        }
        break;
        
        case LM_ENUMREVIDS:
        {
            HRESULT hr = E_FAIL;
            
            if (m_pMessageManager)
            {
                hr = _EnumRevIDs((LSENUMREVIDSPROC)wParam, lParam);
            }
            
            return hr;
        }
        break;
        
        case LM_ENUMMODULES:
        {
            HRESULT hr = E_FAIL;
            
            if (m_pModuleManager)
            {
                hr = m_pModuleManager->EnumModules((LSENUMMODULESPROC)wParam,
                    lParam);
            }
            
            return hr;
        }
        break;
        
        case LM_RECYCLE:
        {
            switch (wParam)
            {
                case LR_RECYCLE:
                {
                    _Recycle();
                }
                break;
                
                case LR_LOGOFF:
                {
                    if (ExitWindowsEx(EWX_LOGOFF, 0))
                    {
                        PostQuitMessage(0);
                    }
                }
                break;
                
                case LR_QUIT:
                {
                    PostQuitMessage(0);
                }
                break;
                
                default:  // wParam == LR_MSSHUTDOWN
                {
                    FARPROC (__stdcall * MSWinShutdown)(HWND) = NULL;
                    MSWinShutdown = (FARPROC (__stdcall *)(HWND))GetProcAddress(GetModuleHandle("SHELL32.DLL"), (LPSTR)((long)0x3C));
                    if (MSWinShutdown)
                    {
                        MSWinShutdown(m_hMainWindow); // shouldn't this be NULL?
                        // Could also use the now-documented RestartDialog(Ex)
                    }
                }
                break;
            }
        }
        break;
        
        case LM_RELOADMODULE:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    // not sure if this feature is needed... if a module
                    // wants to reload it shouldn't need the core to do that
                    m_pModuleManager->ReloadModule((HINSTANCE)wParam);
                }
                else
                {
                    LPCSTR pszPath = (LPCSTR)wParam;
                    
                    if (IsValidStringPtr(pszPath))
                    {
                        m_pModuleManager->QuitModule(pszPath);
                        m_pModuleManager->LoadModule(pszPath, (DWORD)lParam);
                    }
                }
            }
        }
        break;
        
        case LM_UNLOADMODULE:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    m_pModuleManager->QuitModule((HINSTANCE)wParam);
                }
                else
                {
                    LPCSTR pszPath = (LPCSTR)wParam;
                    
                    if (IsValidStringPtr(pszPath))
                    {
                        m_pModuleManager->QuitModule(pszPath);
                    }
                }
            }
        }
        break;
        
        case LM_BANGCOMMAND:
        {
            PLMBANGCOMMAND plmbc = (PLMBANGCOMMAND)lParam;
            
            if (IsValidReadPtr(plmbc))
            {
                if (plmbc->cbSize == sizeof(LMBANGCOMMAND))
                {
                    lReturn = ParseBangCommand(plmbc->hWnd, plmbc->szCommand, plmbc->szArgs);
                }
            }
        }
        break;
        
        case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            
            switch (pcds->dwData)
            {
                case LM_BANGCOMMAND:
                {
                    lReturn = SendMessage(hWnd, LM_BANGCOMMAND, 0, (LPARAM)pcds->lpData);
                }
                break;
                
                default:
                break;
            }
        }
        break;
        
        case LM_REGISTERMESSAGE:     // Message Handler Message
        {
            if (m_pMessageManager)
            {
                m_pMessageManager->AddMessages((HWND)wParam, (UINT *)lParam);
            }
        }
        break;
        
        case LM_UNREGISTERMESSAGE:     // Message Handler Message
        {
            if (m_pMessageManager)
            {
                m_pMessageManager->RemoveMessages((HWND)wParam, (UINT *)lParam);
            }
        }
        break;
        
        default:
        {
            if (uMsg == WM_ShellHook)
            {
                HWND hWndMessage = (HWND)lParam;
                uMsg = (LOWORD(wParam) & 0x00FF) + 9500;
                lParam = (LOWORD(wParam) & 0xFF00);
                wParam = (WPARAM)hWndMessage;
                
                if (uMsg == LM_WINDOWACTIVATED)
                {
                    if (m_bAutoHideModules)
                    {
                        if ((lParam > 0) && (m_bAppIsFullScreen == false))
                        {
                            m_bAppIsFullScreen = true;
                            ParseBangCommand(m_hMainWindow, "!HIDEMODULES", NULL);
                        }
                        else if ((lParam <= 0) && (m_bAppIsFullScreen == true))
                        {
                            m_bAppIsFullScreen = false;
                            ParseBangCommand(m_hMainWindow, "!SHOWMODULES", NULL);
                        }
                    }
                }
            }
            
            if (m_pMessageManager && m_pMessageManager->HandlerExists(uMsg))
            {
                lReturn = m_pMessageManager->SendMessage(uMsg, wParam, lParam);
                break;
            }
            lReturn = DefWindowProc (hWnd, uMsg, wParam, lParam);
        }
        break;
    }
    
    return lReturn;
}


HRESULT CLiteStep::get_Window(/*[out, retval]*/ long *phWnd) const
{
    HRESULT hr;
    
    if (phWnd != NULL)
    {
        *phWnd = (LONG)m_hMainWindow;
        hr = S_OK;
    }
    else
    {
        hr = E_INVALIDARG;
    }
    
    return hr;
}


HRESULT CLiteStep::get_AppPath(/*[out, retval]*/ LPSTR pszPath, /*[in]*/ size_t cchPath) const
{
    HRESULT hr;
    
    if (IsValidStringPtr(pszPath, cchPath))
    {
        hr = StringCchCopy(pszPath, cchPath, m_sAppPath.c_str());
    }
    else
    {
        hr = E_INVALIDARG;
    }
    
    return hr;
}


//
// _InitServies()
//
HRESULT CLiteStep::_InitServices()
{
    IService* pService = NULL;
    
    //
    // DDE Service
    //
    if (GetRCBool("LSUseSystemDDE", TRUE))
    {
        // M$ DDE
        pService = new DDEStub();
    }
    else
    {
        // liteman
        pService = new DDEService();
    }
    
    if (pService)
    {
        m_Services.push_back(pService);
    }
    else
    {
        return E_OUTOFMEMORY;
    }
    
    //
    // Tray Service
    //
    if (GetRCBool("LSDisableTrayService", FALSE))
    {
        m_pTrayService = new TrayService();
        
        if (m_pTrayService)
        {
            m_Services.push_back(m_pTrayService);
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }
    
    return S_OK;
}


//
// _StartServices()
//
HRESULT CLiteStep::_StartServices()
{
    // use std::transform to add error checking to this
    for_each(m_Services.begin(), m_Services.end(), mem_fun(&IService::Start));
    return S_OK;
}


//
// _StopServices()
//
HRESULT CLiteStep::_StopServices()
{
    for_each(m_Services.begin(), m_Services.end(), mem_fun(&IService::Stop));
    return S_OK;
}


//
// _CleanupServices()
//
void CLiteStep::_CleanupServices()
{
    std::for_each(m_Services.begin(), m_Services.end(),
        std::mem_fun(&IService::Release));
    
    m_Services.clear();
}


//
// _InitManagers()
//
HRESULT CLiteStep::_InitManagers()
{
    HRESULT hr = S_OK;
    
    m_pMessageManager = new MessageManager();
    
    m_pModuleManager = new ModuleManager();
    
    // Note:
    // - The DataStore manager is dynamically initialized/started.
    // - The Bang manager is located in LSAPI, and
    //   is instantiated via LSAPIInit.
    
    return hr;
}


//
// _StartManagers
//
HRESULT CLiteStep::_StartManagers()
{
    HRESULT hr = S_OK;
    
    // Setup bang commands in lsapi
    SetupBangs();
    
    // Load modules
    m_pModuleManager->Start(this);
    
    // Note:
    // - The BangManger is continually running
    //   however, we must add our internal bang commands
    //   at startup.  We take responsibility for that
    //   here as it also needs to happen on Recycle.
    // - MessageManager has/needs no Start method.
    // - The DataStore manager is dynamically initialized/started.
    
    return hr;
}


//
// _StopManagers()
//
HRESULT CLiteStep::_StopManagers()
{
    HRESULT hr = S_OK;
    
    m_pModuleManager->Stop();
    
    m_pMessageManager->ClearMessages();
    ClearBangs();
    
    // Note:
    // - The DataStore manager is persistent.
    // - The Message manager can not be "stopped", just cleared.
    // - The Bang manager can not be "stopped", just cleraed.
    
    return hr;
}


//
// _CleanupManagers
//
void CLiteStep::_CleanupManagers()
{
    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = NULL;
    }
    
    if (m_pMessageManager)
    {
        delete m_pMessageManager;
        m_pMessageManager = NULL;
    }
    
    if (m_pDataStoreManager)
    {
        m_pDataStoreManager->Clear();
        delete m_pDataStoreManager;
        m_pDataStoreManager = NULL;
    }
}


//
// _Recycle
//
void CLiteStep::_Recycle()
{
    Block block(m_BlockRecycle);
    
    /* Do not allow recursive recycles.  This may happen if some
     * one is heavy fingered on their recycle hotkey, and multiple
     * LM_RECYCLE messages are posted to the queue. */
    if(block.IsBlocked())
    {
        return;
    }
    
    _StopManagers();
    
    DeleteSettingsManager();
    
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_ERROR6,
                        "Recycle has been paused, click OK to continue.", "LiteStep");
    }
    
    SetupSettingsManager(m_sAppPath.c_str(), m_sConfigFile.c_str());
    
    /* Read in our locally affected settings */
    m_bAutoHideModules = GetRCBool("LSAutoHideModules", TRUE) ? true : false;
    
    _StartManagers();
}


//
// _EnumRevIDs
//
HRESULT CLiteStep::_EnumRevIDs(LSENUMREVIDSPROC pfnCallback, LPARAM lParam) const
{
    HRESULT hr = E_FAIL;
    
    MessageManager::windowSetT setWindows;
    
    if (m_pMessageManager->GetWindowsForMessage(LM_GETREVID, setWindows))
    {
        hr = S_OK;
        
#if !defined(LS_NO_EXCEPTION)
        try
        {
#endif /* LS_NO_EXCEPTION */
            for (MessageManager::windowSetT::iterator iter = setWindows.begin();
                 iter != setWindows.end(); ++iter)
            {
                // Using MAX_LINE_LENGTH to be on the safe side. Modules
                // should assume a length of 64 or so.
                char szBuffer[MAX_LINE_LENGTH] = { 0 };
                
                if (SendMessage(*iter, LM_GETREVID, 0, (LPARAM)&szBuffer) > 0)
                {
                    if (!pfnCallback(szBuffer, lParam))
                    {
                        hr = S_FALSE;
                        break;
                    }
                }
            }
#if !defined(LS_NO_EXCEPTION)
        }
        catch (...)
        {
            hr = E_UNEXPECTED;
        }
#endif /* LS_NO_EXCEPTION */
    }
    
    return hr;
}
