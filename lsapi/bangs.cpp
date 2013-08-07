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
#include "../utility/core.hpp"


extern DWORD WINAPI AboutBoxThread(LPVOID);

static void BangAbout(HWND hCaller, LPCWSTR pwzArgs);
static void BangAlert(HWND hCaller, LPCWSTR pwzArgs);
static void BangCascadeWindows(HWND hCaller, LPCWSTR pwzArgs);
static void BangConfirm(HWND hCaller, LPCWSTR pwzArgs);
static void BangExecute(HWND hCaller, LPCWSTR pwzArgs);
static void BangHideModules (HWND hCaller, LPCWSTR pwzArgs);
static void BangLogoff(HWND hCaller, LPCWSTR pwzArgs);
static void BangMinimizeWindows(HWND hCaller, LPCWSTR pwzArgs);
static void BangQuit(HWND hCaller, LPCWSTR param);
static void BangRecycle (HWND hCaller, LPCWSTR pwzArgs);
static void BangRefresh(HWND hCaller, LPCWSTR pwzArgs);
static void BangReload(HWND hCaller, LPCWSTR pwzArgs);
static void BangReloadModule (HWND hCaller, LPCWSTR pwzArgs);
static void BangRestoreWindows(HWND hCaller, LPCWSTR pwzArgs);
static void BangRun (HWND hCaller, LPCWSTR pwzArgs);
static void BangShowModules (HWND hCaller, LPCWSTR pwzArgs);
static void BangShutdown(HWND hCaller, LPCWSTR pwzArgs);
static void BangSwitchUser(HWND hCaller, LPCWSTR pwzArgs);
static void BangTileWindowsH(HWND hCaller, LPCWSTR pwzArgs);
static void BangTileWindowsV(HWND hCaller, LPCWSTR pwzArgs);
static void BangToggleModules (HWND hCaller, LPCWSTR pwzArgs);
static void BangUnloadModule (HWND hCaller, LPCWSTR pwzArgs);


void SetupBangs(void)
{
    AddBangCommandW(L"!About",            BangAbout);
    AddBangCommandW(L"!Alert",            BangAlert);
    AddBangCommandW(L"!CascadeWindows",   BangCascadeWindows);
    AddBangCommandW(L"!Confirm",          BangConfirm);
    AddBangCommandW(L"!Execute",          BangExecute);
    AddBangCommandW(L"!HideModules",      BangHideModules);
    AddBangCommandW(L"!Logoff",           BangLogoff);
    AddBangCommandW(L"!MinimizeWindows",  BangMinimizeWindows);
    AddBangCommandW(L"!Quit",             BangQuit);
    AddBangCommandW(L"!Recycle",          BangRecycle);
    AddBangCommandW(L"!Refresh",          BangRefresh);
    AddBangCommandW(L"!Reload",           BangReload);
    AddBangCommandW(L"!ReloadModule",     BangReloadModule);
    AddBangCommandW(L"!RestoreWindows",   BangRestoreWindows);
    AddBangCommandW(L"!Run",              BangRun);
    AddBangCommandW(L"!ShowModules",      BangShowModules);
    AddBangCommandW(L"!Shutdown",         BangShutdown);
    AddBangCommandW(L"!SwitchUser",       BangSwitchUser);
    AddBangCommandW(L"!TileWindowsH",     BangTileWindowsH);
    AddBangCommandW(L"!TileWindowsV",     BangTileWindowsV);
    AddBangCommandW(L"!ToggleModules",    BangToggleModules);
    AddBangCommandW(L"!UnloadModule",     BangUnloadModule);
}


//
// BangAbout(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangAbout(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    CloseHandle(LSCreateThread("AboutBox Thread", AboutBoxThread, NULL, NULL));
}


//
// BangAlert(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangAlert(HWND hCaller, LPCWSTR pwzArgs)
{
    wchar_t wzMessage[MAX_LINE_LENGTH] = { 0 };
    wchar_t wzTitle[MAX_LINE_LENGTH] = { 0 };
    LPWSTR awzTokens[] = { wzMessage, wzTitle };
    
    int nTokenCount = LCTokenizeW(pwzArgs, awzTokens, 2, 0);
    
    if (nTokenCount >= 1)
    {
        if (nTokenCount == 1)
        {
            StringCchCopyW(wzTitle, MAX_PATH, L"LiteStep !Alert");
        }
        
        MessageBoxW(hCaller, wzMessage, wzTitle, MB_OK | MB_TOPMOST);
    }
}


//
// BangCascadeWindows(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangCascadeWindows(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    CascadeWindows(NULL, MDITILE_SKIPDISABLED, NULL, 0, NULL);
}


//
// BangConfirm(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangConfirm(HWND hCaller, LPCWSTR pwzArgs)
{
    wchar_t wzFirst[MAX_LINE_LENGTH] = { 0 };
    wchar_t wzSecond[MAX_LINE_LENGTH] = { 0 };
    wchar_t wzThird[MAX_LINE_LENGTH] = { 0 };
    wchar_t wzFourth[MAX_LINE_LENGTH] = { 0 };
    LPWSTR awzTokens[] = { wzFirst, wzSecond, wzThird, wzFourth };
    
    int nTokenCount = CommandTokenizeW(pwzArgs, awzTokens, 4, 0);
    
    if (nTokenCount >= 3)
    {
        if (nTokenCount == 3)
        {
            StringCchCopyW(wzFourth, MAX_LINE_LENGTH, L"LiteStep !Confirm");
        }
    
        LPCWSTR pwzTitle = (nTokenCount == 3) ? wzFourth : wzSecond;
        
        INT idConfirm = MessageBoxW(hCaller,
            wzFirst, pwzTitle, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        
        if (idConfirm == IDYES)
        {
            LSExecuteW(hCaller,
                (nTokenCount == 3) ? wzSecond : wzThird, SW_SHOWNORMAL);
        }
        else
        {
            LSExecuteW(hCaller,
                (nTokenCount == 3) ? wzThird : wzFourth, SW_SHOWNORMAL);
        }
    }
}


//
// BangExecute(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangExecute(HWND hCaller, LPCWSTR pwzArgs)
{
    LPCWSTR pszNextToken = pwzArgs;
    wchar_t wzCommand[MAX_LINE_LENGTH];
    
    while (GetTokenW(pszNextToken, wzCommand, &pszNextToken, TRUE))
    {
        LSExecuteW(hCaller, wzCommand, SW_SHOWDEFAULT);
    }
}


//
// BangLogoff(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangLogoff(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_LOGOFF, 0);
    }
}


//
// CALLBACK WindowsEnumProc(HWND hWnd, LPARAM lParam)
//
BOOL CALLBACK WindowsEnumProc(HWND hWnd, LPARAM lParam)
{
    /* Based off of Jugg's task.dll */
    if (IsWindow(hWnd))
    {
        if ((GetWindowLongPtr(hWnd, GWLP_USERDATA) != magicDWord) &&
            (!(GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) &&
            (GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE)) &&
            (!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)))
        {
            ShowWindow(hWnd, (UINT)lParam);
        }
    }
    
    return TRUE;
}


//
// BangMinimizeWindows(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangMinimizeWindows(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    EnumWindows(WindowsEnumProc, (LPARAM)SW_MINIMIZE);
}


//
// BangQuit(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangQuit(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_QUIT, 0);
    }
}


//
// BangRecycle(HWND hCaller, LPCSTR pwzArgs)
//
static void BangRecycle(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_RECYCLE, 0);
    }
}


//
// BangRefresh(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangRefresh(HWND hCaller, LPCWSTR pwzArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    BangReload(hCaller, pwzArgs);
    
    if (hLiteStep)
    {
        SendMessage(hLiteStep, LM_REFRESH, 0, 0);
    }
}


static void BangReload(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    LSAPIReloadSettings();
}


//
// BangReloadModule(HWND hCaller, LPCWSTR pwzArgs)
//
static void BangReloadModule(HWND /* hCaller */, LPCWSTR pwzArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        LPCWSTR pwzNextToken = pwzArgs;
        wchar_t wzModuleString[MAX_LINE_LENGTH] = { 0 };
        
        while (GetTokenW(pwzNextToken, wzModuleString, &pwzNextToken, TRUE))
        {
            SendMessage(hLiteStep, LM_RELOADMODULEW, (WPARAM)wzModuleString, 0);
        }
    }
}


//
// BangRestoreWindows(HWND hCaller, LPCWSTR pszArgs)
//
static void BangRestoreWindows(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    EnumWindows(WindowsEnumProc, (LPARAM)SW_RESTORE);
}


//
// BangRun(HWND hCaller, LPCWSTR pszArgs)
//
static void BangRun(HWND /* hCaller */, LPCWSTR pwzArgs)
{
    // Flags
    //  1 -- Removes the browse button.
    //  2 -- No default text.
    //  4 -- Calculates the working directory from the filename.
    //  8 -- Hides the label to the left of the box.
    // 32 -- Removes the Separate Memory Space check box (Windows NT only)
    typedef VOID (WINAPI* RunDlgType)(HWND hwndOwner, HICON hIcon, LPCWSTR pwszDirectory,
        LPCWSTR pwszTitle, LPCWSTR pwszDescription, UINT uFlags);

    wchar_t wzX[MAX_LINE_LENGTH] = { 0 };
    wchar_t wzY[MAX_LINE_LENGTH] = { 0 };
    LPWSTR aszTokens[] = { wzX, wzY };
    
    int nTokenCount = CommandTokenizeW(pwzArgs, aszTokens, 2, NULL);
    int nX = nTokenCount == 2 ? _wtoi(wzX) : GetRCIntW(L"LSRunX", 0);
    int nY = nTokenCount == 2 ? _wtoi(wzY) : GetRCIntW(L"LSRunY", 0);

    RunDlgType RunDlg = (RunDlgType)GetProcAddress(
        GetModuleHandleW(L"SHELL32.DLL"), (LPCSTR)((long)0x003D));

    if (RunDlg)
    {
        HWND hParent = CreateWindowExW(WS_EX_TOOLWINDOW, L"Static", L"",
            WS_POPUP, nX, nY, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
        if (hParent)
        {
            SetWindowLongPtr(hParent, GWLP_USERDATA, 0);
            SetWindowLongPtr(hParent, GWLP_WNDPROC, (LONG_PTR)(WNDPROC) [] (HWND window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
            {
                if (message == WM_TIMER)
                {
                    HWND hDialog = GetWindow(window, GW_ENABLEDPOPUP);
                    if (hDialog != nullptr)
                    {
                        KillTimer(window, 1);

                        if (GetWindowLongPtr(window, GWLP_USERDATA) == 0)
                        {
                            SetWindowLongPtr(hDialog, GWLP_USERDATA, GetWindowLongPtr(hDialog, GWLP_WNDPROC));
                            SetWindowLongPtr(hDialog, GWLP_WNDPROC, (LONG_PTR)(WNDPROC) [] (HWND window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
                            {
                                if (message == WM_COMMAND && LOWORD(wParam) == 1 && HIWORD(wParam) == 0)
                                {
                                    // Pressed OK, handle it ourselves and forward a cancel instead.
                                    HWND hEditBox = GetDlgItem(window, 0x300A);
                                    if (hEditBox != nullptr)
                                    {
                                        int nTextLength = GetWindowTextLengthW(hEditBox) + 1;
                                        LPWSTR pwzText = (LPWSTR)malloc(nTextLength*sizeof(wchar_t));
                                        GetWindowTextW(hEditBox, pwzText, nTextLength);

                                        // Only handle bang commands ourselfs, as anything else will prevent the RunMRU from populating properly.
                                        if (pwzText[0] == L'!')
                                        {
                                            WCHAR wzCommand[MAX_BANGCOMMAND];
                                            LPCWSTR pwzArgs = nullptr;
                                            GetTokenW(pwzText, wzCommand, &pwzArgs, TRUE);
                                            LSExecuteExW(nullptr, nullptr, wzCommand, pwzArgs, nullptr, 0);
                                            wParam = MAKEWPARAM(2, 0); // Click on cancel
                                        }
                                        free(pwzText);
                                    }
                                }
                                return WNDPROC(GetWindowLongPtr(window, GWLP_USERDATA))(window, message, wParam, lParam);
                            });
                            SetWindowLongPtr(window, GWLP_USERDATA, 1);
                        }
                    }
                }
                return DefWindowProc(window, message, wParam, lParam);
            });
            SetTimer(hParent, 1, 10, nullptr);
        }

        RunDlg(hParent, NULL, NULL, NULL, NULL, 0);

        if (hParent)
        {
            DestroyWindow(hParent);
        }
    }
}


//
// BangShutdown(HWND hCaller, LPCWSTR pszArgs)
//
static void BangShutdown(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_MSSHUTDOWN, 0);
    }
}


//
// BangSwitchUser(HWND hCaller, LPCWSTR pszArgs)
//
static void BangSwitchUser(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    typedef BOOL (WINAPI* LockWorkStationType)(VOID);
    
    LockWorkStationType LockWorkStation = (LockWorkStationType)GetProcAddress(
        GetModuleHandleW(L"USER32.DLL"), "LockWorkStation");
    
    if (LockWorkStation)
    {
        LockWorkStation();
    }
}


//
// BangTileWindowsH(HWND hCaller, LPCWSTR pszArgs)
//
static void BangTileWindowsH(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    TileWindows(NULL, MDITILE_HORIZONTAL, NULL, 0, NULL);
}


//
// BangTileWindowsV(HWND hCaller, LPCWSTR pszArgs)
//
static void BangTileWindowsV(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    TileWindows(NULL, MDITILE_VERTICAL, NULL, 0, NULL);
}


//
// BangUnloadModule(HWND hCaller, LPCWSTR pszArgs)
//
static void BangUnloadModule(HWND /* hCaller */, LPCWSTR pwzArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        LPCWSTR pwzNextToken = pwzArgs;
        wchar_t wzPath[MAX_LINE_LENGTH] = { 0 };
        
        while (GetTokenW(pwzNextToken, wzPath, &pwzNextToken, TRUE))
        {
            SendMessage(hLiteStep, LM_UNLOADMODULEW, (WPARAM)wzPath, 0);
        }
    }
}


#define EMP_HIDE   0
#define EMP_SHOW   1
#define EMP_TOGGLE 2

typedef struct _EMPCONFIG
{
    UINT uMode;
    HDWP hDwp;
    HMONITOR hMon;
} EMPCONFIG, *PEMPCONFIG, FAR *LPEMPCONFIG;

static void EMPHandler(HWND hWnd, PEMPCONFIG pEMPCfg)
{
// note: must NOT use SWP_NOSENDCHANGING, as it will not show/hide then.
#define DEFAULT_FLAGS (SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE)
    if (IsWindow(hWnd))
    {
        LONG_PTR lUserData = GetWindowLongPtr(hWnd, GWLP_USERDATA);
        
        if ((lUserData == magicDWord) && IsWindowVisible(hWnd) &&
            (pEMPCfg->uMode == EMP_HIDE || pEMPCfg->uMode == EMP_TOGGLE))
        {
            if (NULL == pEMPCfg->hMon || MonitorFromWindow(
                hWnd, MONITOR_DEFAULTTONULL) == pEMPCfg->hMon)
            {
                SetWindowLongPtr(hWnd, GWLP_USERDATA, HIDEmagicDWord);
                pEMPCfg->hDwp = DeferWindowPos(
                     pEMPCfg->hDwp
                    ,hWnd
                    ,NULL
                    ,0 ,0
                    ,0 ,0
                    ,SWP_HIDEWINDOW | DEFAULT_FLAGS
                );
            }
        }
        else if ((lUserData == HIDEmagicDWord) &&
                 (pEMPCfg->uMode == EMP_SHOW || pEMPCfg->uMode == EMP_TOGGLE))
        {
            if (NULL == pEMPCfg->hMon || MonitorFromWindow(
                hWnd, MONITOR_DEFAULTTONULL) == pEMPCfg->hMon)
            {
                SetWindowLongPtr(hWnd, GWLP_USERDATA, magicDWord);
                pEMPCfg->hDwp = DeferWindowPos(
                     pEMPCfg->hDwp
                    ,hWnd
                    ,NULL
                    ,0 ,0
                    ,0 ,0
                    ,SWP_SHOWWINDOW | DEFAULT_FLAGS
                );
            }
        }
    }
}


//
// CALLBACK EnumModulesProc(HWND hWnd, LPARAM lParam)
//
static BOOL CALLBACK EnumModulesProc(HWND hWnd, LPARAM lParam)
{
    EMPHandler(hWnd, (PEMPCONFIG)lParam);
    return TRUE;
}


//
// BangHideModules(HWND hCaller, LPCSTR pszArgs)
//
static void BangHideModules(HWND hCaller, LPCWSTR /* pwzArgs */)
{
    EMPCONFIG EMPCfg = { 0 };
    
    // A hack to support hiding modules on a specific monitor.
    if (NULL != hCaller && !IsWindow(hCaller))
    {
        // Check to see if this is a valid monitor handle
        MONITORINFO mi;
        mi.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo((HMONITOR)hCaller, &mi))
        {
            EMPCfg.hMon = (HMONITOR)hCaller;
        }
    }
    
    EMPCfg.uMode = EMP_HIDE;
    EMPCfg.hDwp = BeginDeferWindowPos(10);
    EnumWindows(EnumModulesProc, (LPARAM)&EMPCfg);
    EndDeferWindowPos(EMPCfg.hDwp);
}


//
// BangShowModules(HWND hCaller, LPCWSTR pszArgs)
//
static void BangShowModules(HWND hCaller, LPCWSTR /* pwzArgs */)
{
    EMPCONFIG EMPCfg = { 0 };
    
    // A hack to support showing modules on a specific monitor.
    if (NULL != hCaller && !IsWindow(hCaller))
    {
        // Check to see if this is a valid monitor handle
        MONITORINFO mi;
        mi.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo((HMONITOR)hCaller, &mi))
        {
            EMPCfg.hMon = (HMONITOR)hCaller;
        }
    }

    EMPCfg.uMode = EMP_SHOW;
    EMPCfg.hDwp = BeginDeferWindowPos(10);
    EnumWindows(EnumModulesProc, (LPARAM)&EMPCfg);
    EndDeferWindowPos(EMPCfg.hDwp);
}


//
// BangToggleModules(HWND hCaller, LPCWSTR pszArgs)
//
static void BangToggleModules(HWND /* hCaller */, LPCWSTR /* pwzArgs */)
{
    EMPCONFIG EMPCfg = { 0 };
    
    EMPCfg.uMode = EMP_TOGGLE;
    EMPCfg.hDwp = BeginDeferWindowPos(10);
    EnumWindows(EnumModulesProc, (LPARAM)&EMPCfg);
    EndDeferWindowPos(EMPCfg.hDwp);
}
