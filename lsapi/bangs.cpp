//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2009  LiteStep Development Team
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

static void BangAbout(HWND hCaller, LPCSTR pszArgs);
static void BangAlert(HWND hCaller, LPCSTR pszArgs);
static void BangCascadeWindows(HWND hCaller, LPCSTR pszArgs);
static void BangConfirm(HWND hCaller, LPCSTR pszArgs);
static void BangExecute(HWND hCaller, LPCSTR pszArgs);
static void BangHideModules (HWND hCaller, LPCSTR pszArgs);
static void BangLogoff(HWND hCaller, LPCSTR pszArgs);
static void BangMinimizeWindows(HWND hCaller, LPCSTR pszArgs);
static void BangQuit(HWND caller, LPCSTR param);
static void BangRecycle (HWND hCaller, LPCSTR pszArgs);
static void BangRefresh(HWND hCaller, LPCSTR pszArgs);
static void BangReload(HWND hCaller, LPCSTR pszArgs);
static void BangReloadModule (HWND hCaller, LPCSTR pszArgs);
static void BangRestoreWindows(HWND hCaller, LPCSTR pszArgs);
static void BangRun (HWND hCaller, LPCSTR pszArgs);
static void BangShowModules (HWND hCaller, LPCSTR pszArgs);
static void BangShutdown(HWND hCaller, LPCSTR pszArgs);
static void BangSwitchUser(HWND hCaller, LPCSTR pszArgs);
static void BangTileWindowsH(HWND hCaller, LPCSTR pszArgs);
static void BangTileWindowsV(HWND hCaller, LPCSTR pszArgs);
static void BangToggleModules (HWND hCaller, LPCSTR pszArgs);
static void BangUnloadModule (HWND hCaller, LPCSTR pszArgs);


void SetupBangs(void)
{
    AddBangCommand("!About",            BangAbout);
    AddBangCommand("!Alert",            BangAlert);
    AddBangCommand("!CascadeWindows",   BangCascadeWindows);
    AddBangCommand("!Confirm",          BangConfirm);
    AddBangCommand("!Execute",          BangExecute);
    AddBangCommand("!HideModules",      BangHideModules);
    AddBangCommand("!Logoff",           BangLogoff);
    AddBangCommand("!MinimizeWindows",  BangMinimizeWindows);
    AddBangCommand("!Quit",             BangQuit);
    AddBangCommand("!Recycle",          BangRecycle);
    AddBangCommand("!Refresh",          BangRefresh);
    AddBangCommand("!Reload",           BangReload);
    AddBangCommand("!ReloadModule",     BangReloadModule);
    AddBangCommand("!RestoreWindows",   BangRestoreWindows);
    AddBangCommand("!Run",              BangRun);
    AddBangCommand("!ShowModules",      BangShowModules);
    AddBangCommand("!Shutdown",         BangShutdown);
    AddBangCommand("!SwitchUser",       BangSwitchUser);
    AddBangCommand("!TileWindowsH",     BangTileWindowsH);
    AddBangCommand("!TileWindowsV",     BangTileWindowsV);
    AddBangCommand("!ToggleModules",    BangToggleModules);
    AddBangCommand("!UnloadModule",     BangUnloadModule);
}


//
// BangAbout(HWND hCaller, LPCSTR pszArgs)
//
static void BangAbout(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    CloseHandle(LSCreateThread("AboutBox Thread", AboutBoxThread, NULL, NULL));
}


//
// BangAlert(HWND hCaller, LPCSTR pszArgs)
//
static void BangAlert(HWND hCaller, LPCSTR pszArgs)
{
    char szMessage[MAX_LINE_LENGTH] = { 0 };
    char szTitle[MAX_LINE_LENGTH] = { 0 };
    LPSTR aszTokens[] = { szMessage, szTitle };
    
    int nTokenCount = LCTokenize(pszArgs, aszTokens, 2, 0);
    
    if (nTokenCount >= 1)
    {
        if (nTokenCount == 1)
        {
            StringCchCopy(szTitle, MAX_PATH, "LiteStep !Alert");
        }
        
        MessageBox(hCaller, szMessage, szTitle, MB_OK | MB_TOPMOST);
    }
}


//
// BangCascadeWindows(HWND hCaller, LPCSTR pszArgs)
//
static void BangCascadeWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    CascadeWindows(NULL, MDITILE_SKIPDISABLED, NULL, 0, NULL);
}


//
// BangConfirm(HWND hCaller, LPCSTR pszArgs)
//
static void BangConfirm(HWND hCaller, LPCSTR pszArgs)
{
    char szFirst[MAX_LINE_LENGTH] = { 0 };
    char szSecond[MAX_LINE_LENGTH] = { 0 };
    char szThird[MAX_LINE_LENGTH] = { 0 };
    char szFourth[MAX_LINE_LENGTH] = { 0 };
    LPSTR aszTokens[] = { szFirst, szSecond, szThird, szFourth };
    
    int nTokenCount = CommandTokenize(pszArgs, aszTokens, 4, 0);
    
    if (nTokenCount >= 3)
    {
        if (nTokenCount == 3)
        {
            StringCchCopy(szFourth, MAX_LINE_LENGTH, "LiteStep !Confirm");
        }
    
        LPCSTR pszTitle = (nTokenCount == 3) ? szFourth : szSecond;
        
        INT idConfirm = MessageBox(hCaller,
            szFirst, pszTitle, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        
        if (idConfirm == IDYES)
        {
            LSExecute(hCaller,
                (nTokenCount == 3) ? szSecond : szThird, SW_SHOWNORMAL);
        }
        else
        {
            LSExecute(hCaller,
                (nTokenCount == 3) ? szThird : szFourth, SW_SHOWNORMAL);
        }
    }
}


//
// BangExecute(HWND hCaller, LPCSTR pszArgs)
//
static void BangExecute(HWND hCaller, LPCSTR pszArgs)
{
    LPCSTR pszNextToken = pszArgs;
    char szCommand[MAX_LINE_LENGTH];
    
    while (GetToken(pszNextToken, szCommand, &pszNextToken, TRUE))
    {
        LSExecute(hCaller, szCommand, SW_SHOWDEFAULT);
    }
}


//
// BangLogoff(HWND hCaller, LPCSTR pszArgs)
//
static void BangLogoff(HWND /* hCaller */, LPCSTR /* pszArgs */)
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
// BangMinimizeWindows(HWND hCaller, LPCSTR pszArgs)
//
static void BangMinimizeWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    EnumWindows(WindowsEnumProc, (LPARAM)SW_MINIMIZE);
}


//
// BangQuit(HWND hCaller, LPCSTR pszArgs)
//
static void BangQuit(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_QUIT, 0);
    }
}


//
// BangRecycle(HWND hCaller, LPCSTR pszArgs)
//
static void BangRecycle(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_RECYCLE, 0);
    }
}


//
// BangRefresh(HWND hCaller, LPCSTR pszArgs)
//
static void BangRefresh(HWND hCaller, LPCSTR pszArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    BangReload(hCaller, pszArgs);
    
    if (hLiteStep)
    {
        SendMessage(hLiteStep, LM_REFRESH, 0, 0);
    }
}


static void BangReload(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    LSAPIReloadSettings();
}


//
// BangReloadModule(HWND hCaller, LPCSTR pszArgs)
//
static void BangReloadModule(HWND /* hCaller */, LPCSTR pszArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        LPCSTR pszNextToken = pszArgs;
        char szModuleString[MAX_LINE_LENGTH] = { 0 };
        
        while (GetToken(pszNextToken, szModuleString, &pszNextToken, TRUE))
        {
            SendMessage(hLiteStep, LM_RELOADMODULE, (WPARAM)szModuleString, 0);
        }
    }
}


//
// BangRestoreWindows(HWND hCaller, LPCSTR pszArgs)
//
static void BangRestoreWindows(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    EnumWindows(WindowsEnumProc, (LPARAM)SW_RESTORE);
}


//
// BangRun(HWND hCaller, LPCSTR pszArgs)
//
static void BangRun(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    typedef VOID (WINAPI* RunDlgType)(HWND,HICON,LPCSTR,LPCSTR,LPCSTR,UINT);
    
    RunDlgType RunDlg = (RunDlgType)GetProcAddress(
        GetModuleHandle("SHELL32.DLL"), (LPCSTR)((long)0x003D));
    
    if (RunDlg)
    {
        RunDlg(NULL, NULL, NULL, NULL, NULL, 0);
    }
}


//
// BangShutdown(HWND hCaller, LPCSTR pszArgs)
//
static void BangShutdown(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        PostMessage(hLiteStep, LM_RECYCLE, LR_MSSHUTDOWN, 0);
    }
}


//
// BangSwitchUser(HWND hCaller, LPCSTR pszArgs)
//
static void BangSwitchUser(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    typedef BOOL (WINAPI* LockWorkStationType)(VOID);
    
    LockWorkStationType LockWorkStation = (LockWorkStationType)GetProcAddress(
        GetModuleHandle("USER32.DLL"), "LockWorkStation");
    
    if (LockWorkStation)
    {
        LockWorkStation();
    }
}


//
// BangTileWindowsH(HWND hCaller, LPCSTR pszArgs)
//
static void BangTileWindowsH(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    TileWindows(NULL, MDITILE_HORIZONTAL, NULL, 0, NULL);
}


//
// BangTileWindowsV(HWND hCaller, LPCSTR pszArgs)
//
static void BangTileWindowsV(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    TileWindows(NULL, MDITILE_VERTICAL, NULL, 0, NULL);
}


//
// BangUnloadModule(HWND hCaller, LPCSTR pszArgs)
//
static void BangUnloadModule(HWND /* hCaller */, LPCSTR pszArgs)
{
    HWND hLiteStep = GetLitestepWnd();
    
    if (hLiteStep)
    {
        LPCSTR pszNextToken = pszArgs;
        char szPath[MAX_LINE_LENGTH] = { 0 };
        
        while (GetToken(pszNextToken, szPath, &pszNextToken, TRUE))
        {
            SendMessage(hLiteStep, LM_UNLOADMODULE, (WPARAM)szPath, 0);
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
static void BangHideModules(HWND hCaller, LPCSTR /* pszArgs */)
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
// BangShowModules(HWND hCaller, LPCSTR pszArgs)
//
static void BangShowModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    EMPCONFIG EMPCfg = { 0 };
    
    EMPCfg.uMode = EMP_SHOW;
    EMPCfg.hDwp = BeginDeferWindowPos(10);
    EnumWindows(EnumModulesProc, (LPARAM)&EMPCfg);
    EndDeferWindowPos(EMPCfg.hDwp);
}


//
// BangToggleModules(HWND hCaller, LPCSTR pszArgs)
//
static void BangToggleModules(HWND /* hCaller */, LPCSTR /* pszArgs */)
{
    EMPCONFIG EMPCfg = { 0 };
    
    EMPCfg.uMode = EMP_TOGGLE;
    EMPCfg.hDwp = BeginDeferWindowPos(10);
    EnumWindows(EnumModulesProc, (LPARAM)&EMPCfg);
    EndDeferWindowPos(EMPCfg.hDwp);
}
