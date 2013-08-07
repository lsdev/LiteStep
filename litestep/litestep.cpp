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
#include "litestep.h"

// Services
#include "DDEService.h"
#include "DDEStub.h"
#include "RecoveryMenu.h"
#include "TrayService.h"
#include "ExplorerService.h"
#include "FullscreenMonitor.h"
#include "COMService.h"

// Managers
#include "MessageManager.h"
#include "ModuleManager.h"

// Other
#include "DataStore.h"
#include "StartupRunner.h"
#include "../lsapi/lsapiInit.h"
#include "../lsapi/ThreadedBangCommand.h"
#include "../utility/macros.h"
#include "../utility/core.hpp"
#include <algorithm>
#include <WtsApi32.h>
#include <functional>
#include <Psapi.h>


// namespace stuff
using std::for_each;
using std::mem_fun;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetAppPath
//
static HRESULT GetAppPath(LPTSTR pszAppPath, DWORD cchAppPath)
{
    HRESULT hr = E_FAIL;
    
#if defined(_DEBUG)
    typedef BOOL (WINAPI* IsDebuggerPresentProc)();
    
    IsDebuggerPresentProc fnIsDebuggerPresent = (IsDebuggerPresentProc)
        GetProcAddress(GetModuleHandle(_T("kernel32")), "IsDebuggerPresent");
    
    // If a debugger is attached use the current directory as base path
    if (fnIsDebuggerPresent && fnIsDebuggerPresent())
    {
        if (GetCurrentDirectory(cchAppPath, pszAppPath))
        {
            hr = S_OK;
        }
    }
    else
#endif
    // Otherwise use litestep.exe's location as base path
    if (LSGetModuleFileName(NULL, pszAppPath, cchAppPath))
    {
        PathRemoveFileSpec(pszAppPath);
        hr = S_OK;
    }
    else
    {
        hr = HrGetLastError();
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// SetWelcomeScreenEvent
//
void SetWelcomeScreenEvent()
{
    HANDLE hSwitchEvent = NULL;
    
    if (IsVistaOrAbove())
    {
        hSwitchEvent = OpenEvent(
            EVENT_MODIFY_STATE, FALSE, _T("ShellDesktopSwitchEvent"));
    }
    else
    {
        hSwitchEvent = OpenEvent(
            EVENT_MODIFY_STATE, FALSE, _T("msgina: ShellReadyEvent"));
    }
    
    if (hSwitchEvent)
    {
        VERIFY(SetEvent(hSwitchEvent));
        VERIFY(CloseHandle(hSwitchEvent));
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// SetDialogClassIcon
//
void SetDialogClassIcon(HINSTANCE hInstance, HICON hIcon)
{
    ASSERT(hIcon != NULL);
    ASSERT(hInstance != NULL);
    
    // SetClassLongPtr requires a window handle, so create a temporary window
    HWND hwndTemp = CreateWindow(WC_DIALOG, NULL, 0,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    
    if (hwndTemp != NULL)
    {
        SetClassLongPtr(hwndTemp, GCLP_HICON, (LONG_PTR)hIcon);
        DestroyWindow(hwndTemp);
    }
}


//
//
//
int StartLitestep(HINSTANCE hInst, WORD wStartFlags, LPCTSTR pszAltConfigFile)
{
    DbgSetCurrentThreadName("LS Main Thread");
    
    TCHAR szAppPath[MAX_PATH] = { 0 };
    TCHAR szRcPath[MAX_PATH] = { 0 };
    
    if (FAILED(GetAppPath(szAppPath, COUNTOF(szAppPath))))
    {
        // something really crappy is going on.
        return LRV_NO_APP_PATH;
    }
    
    if (wStartFlags & LSF_ALTERNATE_CONFIG)
    {
        StringCchCopy(szRcPath, COUNTOF(szRcPath), pszAltConfigFile);
    }
    else
    {
        PathCombine(szRcPath, szAppPath, L"step.rc");
    }
    
    if (IsVistaOrAbove())
    {
        // Starting with Vista (or Aero?) the shell needs to set the wallpaper.
        // Otherwise the background remains black and SPI_GETDESKWALLPAPER
        // returns an empty string. SETWALLPAPER_DEFAULT apparently causes
        // SystemParametersInfo to look up the wallpaper in the registry.
        VERIFY(SystemParametersInfo(
            SPI_SETDESKWALLPAPER, 0, SETWALLPAPER_DEFAULT, 0));
    }
    
    //
    // Close the welcome screen (if required)
    // This has to be done before the first MessageBox call, otherwise
    // the box would pop up under that screen
    //
    SetWelcomeScreenEvent();
    
    // Change the icon of all dialogs to the LS icon
    SetDialogClassIcon(hInst, LoadIcon(hInst, MAKEINTRESOURCE(IDI_LS)));
    
    // Make sure we don't quit if there's an app blocking shutdown.
    // Otherwise the user might be left without a shell if he chooses to abort
    // shutdown entirely.
    // 0x00FF is just below the application threshold
    SetProcessShutdownParameters(0x00FF, SHUTDOWN_NORETRY);
    
    // If we can't find "step.rc", there's no point in proceeding
    if (!PathFileExists(szRcPath))
    {
        RESOURCE_STREX(
            hInst, IDS_LITESTEP_ERROR2, resourceTextBuffer, MAX_LINE_LENGTH,
            L"Unable to find the file \"%ls\".\n"
            L"Please verify the location of the file, and try again.", szRcPath);
        
        RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);
        
        return LRV_NO_STEP;
    }
    
    // Initialize the LSAPI.  Note: The LSAPI controls the bang and settings
    // managers so they will be started at this point.
    if (!LSAPIInitialize(szAppPath, szRcPath))
    {
        RESOURCE_MSGBOX(hInst, IDS_LSAPI_INIT_ERROR,
            L"Failed to initialize the LiteStep API.", L"LiteStep");
        
        return LRV_LSAPI_FAIL;
    }
    
    // All child processes get this variable
    VERIFY(SetEnvironmentVariable(_T("LitestepDir"), szAppPath));
    
    int nReturn = 0;

    CLiteStep liteStep;
    HRESULT hr = liteStep.Start(hInst, wStartFlags);
    
    if (SUCCEEDED(hr))
    {
        // if hr == S_FALSE the user aborted during startup
        if (hr == S_OK)
        {
            nReturn = liteStep.Run();
            hr = liteStep.Stop();
        }
    }
    else
    {
        RESOURCE_STREX(hInst, IDS_LITESTEP_INIT_ERROR,
            resourceTextBuffer, MAX_LINE_LENGTH,
            L"Failed to initialize LiteStep.\n"
            L"Please contact the LiteStep Development Team.\n\n"
            L"Error code: 0x%.8X", hr);
        
        RESOURCE_MSGBOX_F(L"LiteStep", MB_ICONERROR);
    }
    
    if (FAILED(hr))
    {
        nReturn = HRESULT_CODE(hr);
    }
    
    return nReturn;
}


//
// CLiteStep()
//
CLiteStep::CLiteStep()
: m_pRecoveryMenu(nullptr),
  m_pRegisterShellHook(nullptr),
  m_hWtsDll(nullptr)
{
    m_hInstance = nullptr;
    m_hMainWindow = nullptr;
    WM_ShellHook = 0;
    m_pModuleManager = nullptr;
    m_pDataStoreManager = nullptr;
    m_pMessageManager = nullptr;
    m_bSignalExit = false;
    m_pTrayService = nullptr;
    m_pFullscreenMonitor = nullptr;
    m_BlockRecycle = 0;
}


//
//
//
CLiteStep::~CLiteStep()
{
    // do nothing
}


//
// Start
//
HRESULT CLiteStep::Start(HINSTANCE hInstance, WORD wStartFlags)
{
    HRESULT hr = E_FAIL;
    
    m_hInstance = hInstance;
    m_pRecoveryMenu = new RecoveryMenu(hInstance);
    
    // Before anything else, start the recovery menu
    if (m_pRecoveryMenu)
    {
        hr = m_pRecoveryMenu->Start();
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    
    if (FAILED(hr))
    {
        delete m_pRecoveryMenu;
        m_pRecoveryMenu = NULL;
        
        return hr;
    }
    
    // Initialize OLE/COM
    hr = OleInitialize(NULL);
    
    // Order of precedence: 1) shift key, 2) command line flags, 3) step.rc
    if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
        (GetRCBoolW(L"LSNoStartup", TRUE) &&
        !(wStartFlags & LSF_FORCE_STARTUPAPPS)))
    {
        wStartFlags &= ~LSF_RUN_STARTUPAPPS;
    }
    
    bool bUnderExplorer = false;
    
    //
    // Check for another shell
    //
    if (FindWindow(L"Shell_TrayWnd", NULL) != NULL)
    {
        if (GetRCBoolW(L"LSCloseExplorer", TRUE))
        {
            HWND hTrayWindow = FindWindow(L"Shell_TrayWnd", NULL);

            // Determine the name of the current Shell
            DWORD dwProcessID;
            GetWindowThreadProcessId(hTrayWindow, &dwProcessID);
            HANDLE hShellProc = OpenProcess(
                PROCESS_QUERY_INFORMATION | SYNCHRONIZE,
                FALSE,
                dwProcessID
                );

            if (hShellProc != NULL)
            {
                TCHAR szProcessPath[MAX_PATH];
                LSGetProcessImageFileName(hShellProc, szProcessPath, _countof(szProcessPath));
                LPCTSTR pszPathName = PathFindFileName(szProcessPath);

                if (_tcsicmp(pszPathName, _T("explorer.exe")) == 0)
                {
                    if (IsVistaOrAbove())
                    {
                        PostMessage(hTrayWindow, 0x5B4, 0, 0);
                    }
                    else if (IsOS(OS_XPORGREATER))
                    {
                        HWND hProgman = FindWindow(_T("Progman"), NULL);
                        PostMessage(hProgman, WM_QUIT, 0, TRUE);
                        PostMessage(hTrayWindow, WM_QUIT, 0, 0); 
                    }
                    else
                    {
                        // Not supported
                    }

                    // Wait for the process to exit
                    if (WaitForSingleObject(hShellProc, 3000) != WAIT_OBJECT_0) // Wait for at most 3 seconds.
                    {
                        // At this point, forcibly terminate the proc
                        TerminateProcess(hShellProc, 0);

                        // Wait another 3 seconds, max
                        WaitForSingleObject(hShellProc, 3000);
                    }
                }

                CloseHandle(hShellProc);
            }
        }
    }
    
    if (FindWindow(L"Shell_TrayWnd", NULL) != NULL)
    {
        if (GetRCBoolW(L"LSNoShellWarning", FALSE))
        {
            RESOURCE_STR(hInstance, IDS_LITESTEP_ERROR3,
                L"LiteStep is not able to load as the system shell.\n"
                L"Another shell is already active.\n"
                L"\n"
                L"Continuing to load LiteStep will disable specific system\n"
                L"shell functions of LiteStep and some features will not\n"
                L"function properly such as icon notifications (systray),\n"
                L"the desktop and some task managers.\n"
                L"\n"
                L"To disable this message, place 'LSNoShellWarning' in\n"
                L"your step.rc.\n"
                L"\n"
                L"Continue to load LiteStep?\n");
            
            RESOURCE_TITLE(hInstance, IDS_LITESTEP_TITLE_WARNING, L"Warning");
            
            if (RESOURCE_MSGBOX_F(
                resourceTitleBuffer, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
            {
                return S_FALSE;
            }
        }
        
        bUnderExplorer = true;
    }
    
    if (SUCCEEDED(hr))
    {   
        hr = CreateMainWindow();
    }
    
    //
    // Start up everything
    //
    if (SUCCEEDED(hr))
    {
        hr = _InitServices(!bUnderExplorer && GetRCBoolW(L"LSSetAsShell", TRUE));
        
        if (SUCCEEDED(hr))
        {
            hr = _StartServices();
            
            if (SUCCEEDED(hr))
            {
                hr = _InitManagers();
                
                if (SUCCEEDED(hr))
                {
                    hr = _StartManagers();
                }
            }
        }
        
        if (SUCCEEDED(hr))
        {
            // Run startup items
            if (wStartFlags & LSF_RUN_STARTUPAPPS)
            {
                BOOL bForceStartup = (wStartFlags & LSF_FORCE_STARTUPAPPS);
                
                StartupRunner::Run(bForceStartup);
            }
            
            // On Vista, the shell is responsible for playing the startup sound
            if (IsVistaOrAbove() && StartupRunner::IsFirstRunThisSession(
                _T("LogonSoundHasBeenPlayed")))
            {
                LSPlaySystemSound(L"WindowsLogon");
            }
            
            // Undocumented call: Shell Loading Finished
            SendMessage(GetDesktopWindow(), WM_USER, 0, 0);
        }
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Stop
//
HRESULT CLiteStep::Stop()
{
    _StopManagers();
    _CleanupManagers();
    
    _StopServices();
    _CleanupServices();
    
    if (m_hMainWindow)
    {
        DestroyMainWindow();
    }
    
    OleUninitialize();
    
    if (m_pRecoveryMenu)
    {
        m_pRecoveryMenu->Stop();
        
        delete m_pRecoveryMenu;
        m_pRecoveryMenu = NULL;
    }
    
    return S_OK;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Run
// Main message pump
//
int CLiteStep::Run()
{
    TRACE("Entering main message loop.");
    
    int nReturn = 0;
    MSG message = { 0 };
    
    //
    // Note: check m_bSignalExit first, so that if MessageHandler()
    // was called externally from a response to PeekMessage() we
    // know right away if there was a WM_QUIT in the queue, and
    // subsequently do not incorrectly call GetMessage() again.
    //
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        MessageHandler(message);
        
        // This only occurs in PeekAllMsgs()
        if (m_bSignalExit)
        {
            m_bSignalExit = FALSE;
            PostQuitMessage(m_nQuitMsg);
        }
    }
    
    TRACE("Left main message loop. Last message: 0x%.4X (%p, %p)",
        message.message, message.wParam, message.lParam);
    
    if (message.message == WM_QUIT)
    {
        nReturn = (int)message.wParam;
    }
    
    return nReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// CreateMainWindow
//
HRESULT CLiteStep::CreateMainWindow()
{
    HRESULT hr = E_FAIL;
    
    //
    // Register window class
    //
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = CLiteStep::ExternalWndProc;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_LS));
    wc.lpszClassName = szMainWindowClass;
    
    if (!RegisterClassEx(&wc))
    {
        hr = HrGetLastError();
        return hr;
    }
    
    //
    // Create main window
    //
    m_hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
        szMainWindowClass, szMainWindowTitle,
        0, 0, 0, 0,
        0, NULL, NULL,
        m_hInstance,
        (void*)this);
    
    if (m_hMainWindow)
    {
        // Set magic DWORD to prevent VWM from seeing main window
        SetWindowLongPtr(m_hMainWindow, GWLP_USERDATA, magicDWord);
        
        // Set our window in LSAPI
        LSAPISetLitestepWindow(m_hMainWindow);
        
        _RegisterShellNotifications(m_hMainWindow);
        
        hr = S_OK;
    }
    else
    {
        hr = HrGetLastError();
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DestroyMainWindow
//
HRESULT CLiteStep::DestroyMainWindow()
{
    ASSERT(m_hMainWindow != NULL);
    
    _UnregisterShellNotifications(m_hMainWindow);
    LSAPISetLitestepWindow(NULL);
    
    VERIFY(DestroyWindow(m_hMainWindow));
    m_hMainWindow = NULL;
    
    UnregisterClass(szMainWindowClass, m_hInstance);
    return S_OK;
}


//
//
//
void CLiteStep::PeekAllMsgs()
{
    MSG message;
    
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        if (WM_QUIT == message.message)
        {
            m_bSignalExit = true;
            m_nQuitMsg = (int)message.wParam;
            return;
        }
        
        MessageHandler(message);
    }
}


//
//
//
void CLiteStep::MessageHandler(MSG &message)
{
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
            {
                // do nothing
            }
            break;
        }
    }
    else
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}


//
// _RegisterShellNotifications
//
void CLiteStep::_RegisterShellNotifications(HWND hWnd)
{
    //
    // Configure the Win32 window manager to hide minimized windows
    // This is necessary to enable WH_SHELL-style hooks,
    // including RegisterShellHook
    //
    MINIMIZEDMETRICS mm = { 0 };
    mm.cbSize = sizeof(MINIMIZEDMETRICS);
    
    VERIFY(SystemParametersInfo(SPI_GETMINIMIZEDMETRICS, mm.cbSize, &mm, 0));
    
    if (!(mm.iArrange & ARW_HIDE))
    {
        mm.iArrange |= ARW_HIDE;
        VERIFY(SystemParametersInfo(
            SPI_SETMINIMIZEDMETRICS, mm.cbSize, &mm, 0));
    }
    
    //
    // Register for shell hook notifications
    //
    WM_ShellHook = RegisterWindowMessage(L"SHELLHOOK");
    
    m_pRegisterShellHook = (RSHPROC)GetProcAddress(
        GetModuleHandle(_T("SHELL32.DLL")), (LPCSTR)((long)0x00B5));
    
    if (m_pRegisterShellHook)
    {
        m_pRegisterShellHook(NULL, RSH_REGISTER);
        
        if (IsOS(OS_WINDOWS))
        {
            // c0atzin's fix for 9x
            m_pRegisterShellHook(hWnd, RSH_REGISTER);
        }
        else
        {
            m_pRegisterShellHook(hWnd, RSH_TASKMAN);
        }
    }
    
    //
    // Register for session change notifications
    //
    if (IsOS(OS_XPORGREATER))
    {
        ASSERT(m_hWtsDll == NULL);
        m_hWtsDll = LoadLibrary(_T("WtsApi32.dll"));
        
        if (m_hWtsDll)
        {
            typedef BOOL (WINAPI* WTSRSNPROC)(HWND, DWORD);
            
            WTSRSNPROC pWTSRegisterSessionNotification = (WTSRSNPROC)
                GetProcAddress(m_hWtsDll, "WTSRegisterSessionNotification");
            
            if (pWTSRegisterSessionNotification)
            {
                // This needs to be fixed: We should wait for
                // Global\TermSrvReadyEvent before calling this.
                VERIFY(pWTSRegisterSessionNotification(
                    hWnd, NOTIFY_FOR_THIS_SESSION));
            }
        }
        else
        {
            TRACE("Failed to load WtsApi32.dll");
        }
    }
}


//
// _UnregisterShellNotifications
//
void CLiteStep::_UnregisterShellNotifications(HWND hWnd)
{
    ASSERT(IsWindow(hWnd));
    
    if (m_hWtsDll)
    {
        typedef BOOL (WINAPI* WTSURSNPROC)(HWND);
        
        WTSURSNPROC pWTSUnRegisterSessionNotification = (WTSURSNPROC)
            GetProcAddress(m_hWtsDll, "WTSUnRegisterSessionNotification");
        
        if (pWTSUnRegisterSessionNotification)
        {
            VERIFY(pWTSUnRegisterSessionNotification(hWnd));
        }
        
        VERIFY(FreeLibrary(m_hWtsDll));
        m_hWtsDll = NULL;
    }
    
    if (m_pRegisterShellHook)
    {
        m_pRegisterShellHook(hWnd, RSH_UNREGISTER);
    }
}


//
//
//
LRESULT CALLBACK CLiteStep::ExternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static CLiteStep* pLiteStep = NULL;
    
    if (uMsg == WM_NCCREATE)
    {
        pLiteStep = static_cast<CLiteStep*>(
            reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        
        ASSERT(NULL != pLiteStep);
    }
    
    if (pLiteStep)
    {
        return pLiteStep->InternalWndProc(hWnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//
//
//
LRESULT CLiteStep::InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lReturn = 0;
    
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
                    ParseBangCommandW(hWnd, L"!ShutDown", NULL);
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
                    lReturn = m_pDataStoreManager->StoreData(
                        wIdent, pvData, wLength);
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
                    lReturn = m_pDataStoreManager->ReleaseData(
                        wIdent, pvData, wLength);
                    
                    if (m_pDataStoreManager->Count() == 0)
                    {
                        delete m_pDataStoreManager;
                        m_pDataStoreManager = NULL;
                    }
                }
            }
        }
        break;
        
    case LM_ENUMREVIDS:
        {
            HRESULT hr = E_FAIL;
            
            if (m_pMessageManager)
            {
                hr = _EnumRevIDs((LSENUMREVIDSPROCW)wParam, lParam);
            }
            
            return hr;
        }
        break;
        
    case LM_ENUMMODULES:
        {
            HRESULT hr = E_FAIL;
            
            if (m_pModuleManager)
            {
                hr = m_pModuleManager->EnumModules((LSENUMMODULESPROCW)wParam,
                    lParam);
            }
            
            return hr;
        }
        break;
        
	case LM_ENUMPERFORMANCE:
        {
            HRESULT hr = E_FAIL;
            
            if (m_pModuleManager)
            {
                hr = m_pModuleManager->EnumPerformance((LSENUMPERFORMANCEPROCW)wParam,
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

            case LR_EXPLORER:
                {
                    PostQuitMessage(4);
                }
                break;
                
            default:  // wParam == LR_MSSHUTDOWN
                {
                    LSShutdownDialog(m_hMainWindow);
                }
                break;
            }
        }
        break;
        
    case LM_RELOADMODULEA:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    // not sure if this feature is needed... if a module
                    // wants to reload it shouldn't need the core to do that
                    m_pModuleManager->ReloadModule((HINSTANCE)wParam);
                }
                else  // (lParam & LMM_PATH)
                {
                    LPCSTR pszPath = (LPCSTR)wParam;
                    
                    if (pszPath != nullptr)
                    {
                        std::unique_ptr<wchar_t> wzPath(WCSFromMBS(pszPath));
                        m_pModuleManager->QuitModule(wzPath.get());
                        m_pModuleManager->LoadModule(wzPath.get(), (DWORD)lParam);
                    }
                }
            }
        }
        break;
        
    case LM_RELOADMODULEW:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    // not sure if this feature is needed... if a module
                    // wants to reload it shouldn't need the core to do that
                    m_pModuleManager->ReloadModule((HINSTANCE)wParam);
                }
                else  // (lParam & LMM_PATH)
                {
                    LPCWSTR pwzPath = (LPCWSTR)wParam;
                    
                    if (pwzPath != nullptr)
                    {
                        m_pModuleManager->QuitModule(pwzPath);
                        m_pModuleManager->LoadModule(pwzPath, (DWORD)lParam);
                    }
                }
            }
        }
        break;
        
    case LM_UNLOADMODULEA:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    m_pModuleManager->QuitModule((HINSTANCE)wParam);
                }
                else // (lParam & LMM_PATH)
                {
                    LPCSTR pszPath = (LPCSTR)wParam;
                    
                    if (pszPath != nullptr)
                    {
                        m_pModuleManager->QuitModule(MBSTOWCS(pszPath));
                    }
                }
            }
        }
        break;
        
    case LM_UNLOADMODULEW:
        {
            if (m_pModuleManager)
            {
                if (lParam & LMM_HINSTANCE)
                {
                    m_pModuleManager->QuitModule((HINSTANCE)wParam);
                }
                else // (lParam & LMM_PATH)
                {
                    LPCWSTR pwzPath = (LPCWSTR)wParam;
                    
                    if (pwzPath != nullptr)
                    {
                        m_pModuleManager->QuitModule(pwzPath);
                    }
                }
            }
        }
        break;
        
    case LM_BANGCOMMANDA:
        {
            PLMBANGCOMMANDA plmbc = (PLMBANGCOMMANDA)lParam;
            
            if (plmbc != nullptr)
            {
                if (plmbc->cbSize == sizeof(LMBANGCOMMANDA))
                {
                    lReturn = ParseBangCommandA(plmbc->hWnd,
                        plmbc->szCommand, plmbc->szArgs);
                }
            }
        }
        break;
        
    case LM_BANGCOMMANDW:
        {
            PLMBANGCOMMANDW plmbc = (PLMBANGCOMMANDW)lParam;
            
            if (plmbc != nullptr)
            {
                if (plmbc->cbSize == sizeof(LMBANGCOMMANDW))
                {
                    lReturn = ParseBangCommandW(plmbc->hWnd,
                        plmbc->wzCommand, plmbc->wzArgs);
                }
            }
        }
        break;
        
    case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            
            switch (pcds->dwData)
            {
            case LM_BANGCOMMANDA:
                {
                    lReturn = SendMessage(hWnd, LM_BANGCOMMANDA,
                        0, (LPARAM)pcds->lpData);
                }
                break;

            case LM_BANGCOMMANDW:
                {
                    lReturn = SendMessage(hWnd, LM_BANGCOMMANDW,
                        0, (LPARAM)pcds->lpData);
                }
                break;
                
            default:
                {
                    // do nothing
                }
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
        
    case WM_WTSSESSION_CHANGE:
        {
            lReturn = _HandleSessionChange((DWORD)wParam, (DWORD)lParam);
        }
        break;
        
    default:
        {
            // Translate Shell Hook messages to LM_SHELLHOOK (ShellProc format).
            if (uMsg == WM_ShellHook)
            {
                WORD wHookCode  = (LOWORD(wParam) & 0x00FF);
                WORD wExtraBits = (LOWORD(wParam) & 0xFF00);
                
                // Convert to an LM_SHELLHOOK message
                uMsg = LM_SHELLHOOK + wHookCode;

                if (uMsg == LM_APPCOMMAND)
                {
                    wParam = NULL;
                    lParam = lParam; // no change
                }
                else if (uMsg == LM_GETMINRECT)
                {
                    SHELLHOOKINFO* pshi = (SHELLHOOKINFO*)lParam;
                    
                    wParam = (WPARAM)pshi->hwnd;
                    lParam = (LPARAM)&pshi->rc;
                }
                else
                {
                    wParam = (WPARAM)lParam;
                    lParam = (LPARAM)wExtraBits;
                }
            }

            // WM_APP, LM_XYZ, and registered messages are all >= WM_USER
            if (uMsg >= WM_USER)
            {
                if (m_pMessageManager &&
                    m_pMessageManager->HandlerExists(uMsg))
                {
                    lReturn = \
                        m_pMessageManager->SendMessage(uMsg, wParam, lParam);
                    break;
                }
            }
            lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        break;
    }
    
    return lReturn;
}


//
// _HandleSessionChange
// Handler for WM_WTSSESSION_CHANGE messages
//
LRESULT CLiteStep::_HandleSessionChange(DWORD dwCode, DWORD /* dwSession */)
{
    if (dwCode == WTS_SESSION_LOCK)
    {
        LSPlaySystemSound(L"WindowsLogoff");
    }
    else if (dwCode == WTS_SESSION_UNLOCK)
    {
        LSPlaySystemSound(L"WindowsLogon");
    }
    
    return 0;
}


//
// _InitServies()
//
HRESULT CLiteStep::_InitServices(bool bSetAsShell)
{
    IService* pService = nullptr;
    
    //
    // DDE Service
    //
    if (GetRCBoolW(L"LSUseSystemDDE", TRUE))
    {
        // M$ DDE
        pService = new (std::nothrow) DDEStub();
    }
    else
    {
        // liteman
        pService = new (std::nothrow) DDEService();
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
    if (GetRCBoolW(L"LSDisableTrayService", FALSE))
    {
        m_pTrayService = new (std::nothrow) TrayService();
        
        if (m_pTrayService)
        {
            m_Services.push_back(m_pTrayService);
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }

    //
    // Explorer service
    //
    if (bSetAsShell)
    {
#if defined(LS_USE_EXPLORER_SERVICE)
        pService = new (std::nothrow) ExplorerService();

        if (pService)
        {
            m_Services.push_back(pService);
        }
        else
        {
            return E_OUTOFMEMORY;
        }
#else
        _SetShellWindow(m_hMainWindow);
#endif
    }

    //
    // COM Service
    //
    pService = new (std::nothrow) COMService();

    if (pService)
    {
        m_Services.push_back(pService);
    }
    else
    {
        return E_OUTOFMEMORY;
    }

    //
    // FullscreenMonitor service
    //
    m_pFullscreenMonitor = new (std::nothrow) FullscreenMonitor();

    if (m_pFullscreenMonitor)
    {
        m_Services.push_back(m_pFullscreenMonitor);
    }
    else
    {
        return E_OUTOFMEMORY;
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
    // - The Bang and Settings managers are located in LSAPI, and
    //   are instantiated via LSAPIInit.
    
    return hr;
}


//
// _StartManagers
//
HRESULT CLiteStep::_StartManagers()
{
    HRESULT hr = S_OK;

    // Load modules
    m_pModuleManager->Start(this);
    
    // Note:
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
    
    // Clean up as modules might not have
    m_pMessageManager->ClearMessages();

    // Note:
    // - The DataStore manager is persistent.
    // - The Message manager can not be "stopped", just cleared.
    
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
    if (block.IsBlocked())
    {
        return;
    }
    _StopManagers();
    
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_ERROR6,
            L"Recycle has been paused, click OK to continue.",
            L"LiteStep");
    }
    
    // Re-initialize the bang and settings manager in LSAPI
    LSAPIReloadBangs();
    LSAPIReloadSettings();

    // Call service's Recycle function
    for_each(m_Services.begin(), m_Services.end(), mem_fun(&IService::Recycle));

    _StartManagers();
}


//
// _EnumRevIDs
//
HRESULT CLiteStep::_EnumRevIDs(LSENUMREVIDSPROCW pfnCallback, LPARAM lParam) const
{
    HRESULT hr = E_FAIL;
    
    MessageManager::windowSetT setWindowsW, setWindowsA;
    
    m_pMessageManager->GetWindowsForMessage(LM_GETREVIDA, setWindowsA);
    m_pMessageManager->GetWindowsForMessage(LM_GETREVIDW, setWindowsW);
    
    if (!setWindowsA.empty() || !setWindowsW.empty())
    {
        hr = S_OK;
        
        for (MessageManager::windowSetT::iterator iter = setWindowsW.begin();
            iter != setWindowsW.end(); ++iter)
        {
            // Using MAX_LINE_LENGTH to be on the safe side. Modules
            // should assume a length of 64 or so.
            wchar_t wzBuffer[MAX_LINE_LENGTH] = { 0 };
            
            if (SendMessage(*iter, LM_GETREVIDW, 0, (LPARAM)&wzBuffer) > 0)
            {
                if (!pfnCallback(wzBuffer, lParam))
                {
                    hr = S_FALSE;
                    break;
                }
            }
        }

        for (MessageManager::windowSetT::iterator iter = setWindowsA.begin();
            iter != setWindowsA.end(); ++iter)
        {
            // Using MAX_LINE_LENGTH to be on the safe side. Modules
            // should assume a length of 64 or so.
            char szBuffer[MAX_LINE_LENGTH] = { 0 };
            
            if (SendMessage(*iter, LM_GETREVIDA, 0, (LPARAM)&szBuffer) > 0)
            {
                if (!pfnCallback(MBSTOWCS(szBuffer), lParam))
                {
                    hr = S_FALSE;
                    break;
                }
            }
        }
    }
    
    return hr;
}


//
// _SetShellWindow
//
BOOL CLiteStep::_SetShellWindow(HWND hWnd) {
    typedef BOOL (WINAPI* SETSHELLWINDOWPROC)(HWND);
            
    SETSHELLWINDOWPROC fnSetShellWindow =
        (SETSHELLWINDOWPROC)GetProcAddress(
        GetModuleHandle(_T("USER32.DLL")), "SetShellWindow");
    
    BOOL bRet = FALSE;

    if (fnSetShellWindow)
    {
        bRet = fnSetShellWindow(hWnd);
    }
    else
    {
        TRACE("SetShellWindow() not found");
    }

    return bRet;
}