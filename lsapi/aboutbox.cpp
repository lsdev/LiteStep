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
#include "../litestep/resource.h"
#include "../utility/core.hpp"
#include <CommCtrl.h>
#include <WindowsX.h>
#include <math.h>


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Function declarations
//

// Message handlers
static BOOL    OnInitDialog(HWND hwndDialog, HWND hwndFocus, LPARAM lParam);
static HBRUSH  OnCtlColor(HWND hwndDialog, HDC hdc, HWND hwndChild, int nType);
static INT_PTR OnCommand(HWND hwndDialog, int idCtl, HWND hwndCtl, UINT uCodeNotify);

// ListView Callbacks
typedef void (*AboutFunction)(HWND);

static void AboutBangs(HWND hListView);
static void AboutDevTeam(HWND hListView);
static void AboutLicense(HWND hEdit);
static void AboutModules(HWND hListView);
static void AboutRevIDs(HWND hListView);
static void AboutSysInfo(HWND hListView);
static void AboutPerformance(HWND hListView);

// Utility
static HFONT CreateSimpleFont(LPCSTR pszName, int nSizeInPoints, bool bBold);
static int GetClientWidth(HWND hWnd);
static void FormatBytes(DWORDLONG stBytes, LPSTR pszBuffer, size_t cchBuffer);


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ComboBox options
// The enum and struct need to be kept in sync
//
enum
{
     ABOUT_BANGS = 0
    ,ABOUT_DEVTEAM
    ,ABOUT_LICENSE
    ,ABOUT_MODULES
    ,ABOUT_PERFORMANCE
    ,ABOUT_REVIDS
    ,ABOUT_SYSINFO
};

struct AboutOptions
{
    const char* option;
    AboutFunction function;
}
static const g_aboutOptions[] = \
{
     { "Bang Commands",      AboutBangs       }
    ,{ "Development Team",   AboutDevTeam     }
    ,{ "License",            AboutLicense     }
    ,{ "Loaded Modules",     AboutModules     }
    ,{ "Performance",        AboutPerformance }
    ,{ "Revision IDs",       AboutRevIDs      }
    ,{ "System Information", AboutSysInfo     }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LiteStep Development Team
//
struct TheDevTeam
{
    const char *nick;
    const char *realName;
}
static const g_theDevTeam[] = \
{
     { "Acidfire",      "Alexander Vermaat" }
    ,{ "alur",          "Erik Welander"     }
    ,{ "ilmcuts",       "Simon"             }
    ,{ "jugg",          "Chris Rempel"      }
    ,{ "Maduin",        "Kevin Schaffer"    }
    ,{ "RabidCow",      "Joshua Seagoe"     }
    ,{ "Tobbe",         "Tobbe Lundberg"    }
    ,{ "Xjill",         "Vidar T. Fauske"   }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LiteStep License Notice
//
static const CHAR g_szLicense[] = \
 "LiteStep is a replacement shell for the standard Windows® Explorer shell.\r\n"
 "\r\n"
 "Copyright (C) 1997-1998  Francis Gastellu\r\n"
 "Copyright (C) 1998-2012  LiteStep Development Team\r\n"
 "\r\n"
 "This program is free software; you can redistribute it and/or modify it "
 "under the terms of the GNU General Public License as published by the Free "
 "Software Foundation; either version 2 of the License, or (at your option) "
 "any later version.\r\n"
 "\r\n"
 "This program is distributed in the hope that it will be useful, but WITHOUT "
 "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
 "FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for "
 "more details.\r\n"
 "\r\n"
 "You should have received a copy of the GNU General Public License along with "
 "this program; if not, write to the Free Software Foundation, Inc., 51 "
 "Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\r\n"
 "\r\n"
 "http://www.lsdev.org/";


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Other global constants or variables
//
struct CallbackInfo
{
    HWND hListView;
    int nItem;
};

// Global handle to the running AboutBox instance (if any)
static HWND g_hAboutbox = NULL;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutBox Dialog Procedure
//
static INT_PTR CALLBACK AboutBoxProcedure(
    HWND hwndDialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    HANDLE_MSG(hwndDialog, WM_COMMAND,        OnCommand);
    HANDLE_MSG(hwndDialog, WM_CTLCOLOREDIT,   OnCtlColor);
    HANDLE_MSG(hwndDialog, WM_CTLCOLORSTATIC, OnCtlColor);
    HANDLE_MSG(hwndDialog, WM_INITDIALOG,     OnInitDialog);
    default:
        break;
    }
    
    return FALSE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// OnInitDialog
//
static BOOL OnInitDialog(HWND hwndDialog, HWND hwndFocus, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(hwndFocus);
    UNREFERENCED_PARAMETER(lParam);
    
    // save global handle
    g_hAboutbox = hwndDialog;
    
    //
    // Initialize ListView
    //
    HWND hListView = GetDlgItem(hwndDialog, IDC_LISTVIEW);
    
    // Add custom extended styles
    DWORD dwStyles = LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP;
    ListView_SetExtendedListViewStyleEx(hListView, dwStyles, dwStyles);
    
    //
    // Initialize title
    //
    HWND hwndTitle = GetDlgItem(hwndDialog, IDC_TITLE);
    
    HFONT hTitleFont = CreateSimpleFont("Verdana", 14, false);
    SetWindowFont(hwndTitle, hTitleFont, FALSE);
    
    Static_SetText(hwndTitle, "LiteStep 0.25.0 Alpha");
    
    //
    // Initialize theme info
    //
    char themeAuthor[16] = { 0 };
    char themeName[21] = { 0 };
    char themeOut[MAX_LINE_LENGTH] = { 0 };
    
    GetRCString("ThemeAuthor", themeAuthor, "(unknown)", COUNTOF(themeAuthor));
    GetRCString("ThemeName", themeName, "(unknown", COUNTOF(themeName));
    
    if (SUCCEEDED(StringCchPrintf(themeOut, COUNTOF(themeOut),
        "Theme: %s by %s", themeName, themeAuthor)))
    {
        SetDlgItemText(hwndDialog, IDC_THEME_INFO, themeOut);
    }
    
    //
    // Initialize compile time
    //
    char compileTime[42] = { 0 };
    LSGetVariableEx("CompileDate", compileTime, 42);
    SetDlgItemText(hwndDialog, IDC_COMPILETIME, compileTime);
    
    //
    // Initialize edit control (license notice)
    //
    SetDlgItemText(hwndDialog, IDC_EDIT, g_szLicense);
    
    //
    // Initialize ComboBox
    //
    HWND hComboBox = GetDlgItem(hwndDialog, IDC_COMBOBOX);
    
    for (unsigned int i = 0; i < COUNTOF(g_aboutOptions); ++i)
    {
        ComboBox_AddString(hComboBox, g_aboutOptions[i].option);
    }
    
    // Default to license notice
    ComboBox_SetCurSel(hComboBox, ABOUT_LICENSE);
    
    // SetCurSel doesn't notify us via WM_COMMAND, so force it
    FORWARD_WM_COMMAND(
        hwndDialog, IDC_COMBOBOX, hComboBox, CBN_SELCHANGE, SendMessage);
    
    //
    // Finishing touches: Center dialog, switch focus
    //
    RECT rc;
    GetWindowRect(hwndDialog, &rc);
    
    SetWindowPos(hwndDialog, HWND_TOP,
        (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
        0, 0, SWP_NOSIZE);
    
    SwitchToThisWindow(hwndDialog, TRUE);
    
    // Set focus to the combo box
    SendMessage(hwndDialog, WM_NEXTDLGCTL, (WPARAM)hComboBox, TRUE);
    
    return FALSE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// OnCtlColor
//
static HBRUSH OnCtlColor(HWND hwndDialog, HDC hdc, HWND hwndChild, int nType)
{
    UNREFERENCED_PARAMETER(hwndDialog);
    UNREFERENCED_PARAMETER(nType);
    
    HBRUSH hbReturn = NULL;
    
    // The header and title need a white (COLOR_WINDOW) background
    int idCtl = GetDlgCtrlID(hwndChild);
    
    if (idCtl == IDC_TITLE || idCtl == IDC_THEME_INFO || idCtl == IDC_EDIT)
    {
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        
        hbReturn = GetSysColorBrush(COLOR_WINDOW);
    }
    else if (idCtl == IDC_HEADER || idCtl == IDC_LOGO)
    {
        hbReturn = GetSysColorBrush(COLOR_WINDOW);
    }
    else
    {
        // special return value to tell the system
        // to perform default message processing
        hbReturn = FALSE;
    }
    
    return hbReturn;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// OnCommand
//
static INT_PTR OnCommand(
    HWND hwndDialog, int idCtl, HWND hwndCtl, UINT uCodeNotify)
{
    BOOL bHandled = FALSE;
    
    if (idCtl == IDC_COMBOBOX && uCodeNotify == CBN_SELCHANGE)
    {
        HWND hwndEdit = GetDlgItem(hwndDialog, IDC_EDIT);
        HWND hListView = GetDlgItem(hwndDialog, IDC_LISTVIEW);
        
        // Delete listview items
        ListView_DeleteAllItems(hListView);
        
        // Delete listview columns
        for (int nCol = 3; nCol >= 0; --nCol)
        {
            ListView_DeleteColumn(hListView, nCol);
        }
        
        // get new selection
        int i = ComboBox_GetCurSel(hwndCtl);
        
        switch (i)
        {
        default:
            // default to revision IDs
            i = ABOUT_REVIDS;
            ComboBox_SetCurSel(hwndCtl, ABOUT_REVIDS);
            
            // FALL THROUGH
            
        case ABOUT_REVIDS:
        case ABOUT_BANGS:
        case ABOUT_DEVTEAM:
        case ABOUT_MODULES:
        case ABOUT_PERFORMANCE:
        case ABOUT_SYSINFO:
            // set the current display to the list view
            g_aboutOptions[i].function(hListView);
            
            ShowWindow(hListView, SW_SHOWNA);
            ShowWindow(hwndEdit, SW_HIDE);
            break;
            
        case ABOUT_LICENSE:
            // set the current display to the edit box
            g_aboutOptions[i].function(hwndEdit);
            
            ShowWindow(hwndEdit, SW_SHOWNA);
            ShowWindow(hListView, SW_HIDE);
            break;
        }
        
        bHandled = TRUE;
    }
    else if (idCtl == IDOK || idCtl == IDCANCEL)
    {
        HFONT hTitleFont = (HFONT)SendDlgItemMessage(
            hwndDialog, IDC_TITLE, WM_GETFONT, 0, 0);
        
        // close the dialog box
        EndDialog(hwndDialog, IDOK);
        
        // release title font
        DeleteObject(hTitleFont);
        
        bHandled = TRUE;
    }
    /* This isn't necessary as we have the edit control set to read only.
    * It just ensures our text doesn't get changed somehow */
    else if (idCtl == IDC_EDIT && uCodeNotify == EN_UPDATE)
    {
        DWORD dwStart = 0;
        SendMessage(hwndCtl, EM_GETSEL, (WPARAM)&dwStart, 0);
        
        Edit_SetText(hwndCtl, g_szLicense);
        Edit_SetSel(hwndCtl, dwStart-1, dwStart-1);
        
        bHandled = TRUE;
    }
    
    SetWindowLongPtr(hwndDialog, DWLP_MSGRESULT, bHandled);
    return bHandled;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutBox Thread Procedure
//
DWORD WINAPI AboutBoxThread(LPVOID /* lpParameter */)
{
    if (!g_hAboutbox)
    {
        DialogBox(GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, AboutBoxProcedure);
        g_hAboutbox = NULL;
    }
    else
    {
        SetForegroundWindow(g_hAboutbox);
    }
    
    return 0;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// BangCallback
// Used by AboutBangs
//
static BOOL CALLBACK BangCallback(
    HMODULE hModule, LPCSTR pszName, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem;
    itemInfo.pszText = (char*)pszName;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    CHAR szModule[MAX_PATH] = { 0 };
    
    if (LSGetModuleFileName(hModule, szModule, COUNTOF(szModule)))
    {
        PathStripPath(szModule);
        ListView_SetItemText(pCi->hListView, pCi->nItem, 1, szModule);
    }
    
    ++pCi->nItem;
    return TRUE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutBangs
//
static void AboutBangs(HWND hListView)
{
    LVCOLUMN columnInfo;
    char text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    strcpy(text, "Bang Command");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 2;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    strcpy(text, "Module");
    columnInfo.cx = width - columnInfo.cx;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSData(ELD_BANGS_V2, (FARPROC)BangCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutDevTeam
//
static void AboutDevTeam(HWND hListView)
{
    LVCOLUMN columnInfo;
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    char text[32];
    
    strcpy(text, "Nick");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 3;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    strcpy(text, "Real Name");
    columnInfo.cx = (2 * width) / 3;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    for (unsigned int i = 0; i < COUNTOF(g_theDevTeam); ++i)
    {
        LVITEM itemInfo;
        
        itemInfo.mask = LVIF_TEXT;
        itemInfo.iItem = i;
        itemInfo.pszText = (char*)g_theDevTeam[i].nick;
        itemInfo.iSubItem = 0;
        
        ListView_InsertItem(hListView, &itemInfo);
        ListView_SetItemText(hListView, i, 1, (char*)g_theDevTeam[i].realName);
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutLicense
// Show License Notice... Nothing to do
//
static void AboutLicense(HWND /* hEdit */)
{
    // do nothing
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// ModulesCallback
// Used by AboutModules
//
static BOOL CALLBACK ModulesCallback(LPCSTR pszPath, DWORD /* dwFlags */, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem++;
    itemInfo.pszText = (char*)pszPath;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    return TRUE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutModules
//
static void AboutModules(HWND hListView)
{
    LVCOLUMN columnInfo;
    char text[32];
    
    strcpy(text, "Module");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSData(ELD_MODULES, (FARPROC)ModulesCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// PerformanceCallback
// Used by AboutPerformance
//
static BOOL CALLBACK PerformanceCallback(LPCSTR pszPath, DWORD dwLoadTime, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    CHAR szModule[MAX_PATH] = { 0 };
    strcpy(szModule, pszPath);
    PathStripPath(szModule);
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem;
    itemInfo.pszText = szModule;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    sprintf(szModule, "%dms", dwLoadTime);
    ListView_SetItemText(pCi->hListView, pCi->nItem, 1, szModule);
    
    ++pCi->nItem;
    return TRUE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutPerformance
//
static void AboutPerformance(HWND hListView)
{
    LVCOLUMN columnInfo;
    char text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    strcpy(text, "Module");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 2;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    strcpy(text, "Load Time");
    columnInfo.cx = width - columnInfo.cx;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSData(ELD_PERFORMANCE, (FARPROC)PerformanceCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// RevIDCallback
// Used by AboutRevIDs
//
static BOOL CALLBACK RevIDCallback(LPCSTR pszRevID, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem++;
    itemInfo.pszText = (char*)pszRevID;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    return TRUE;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutRevIDs
//
static void AboutRevIDs(HWND hListView)
{
    LVCOLUMN columnInfo;
    char text[32];
    
    strcpy(text, "Revision ID");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    ci.nItem = 0;
    
    EnumLSData(ELD_REVIDS, (FARPROC)RevIDCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetWinVerString
// Helper for AboutSysInfo
//
static HRESULT GetWinVerString(LPTSTR pszVersion, DWORD cchVersion)
{
    ASSERT(pszVersion != NULL);
    
    UINT uVersion = GetWindowsVersion();
    
    LPCSTR pszTemp = NULL;
    
    switch (uVersion)
    {
    case WINVER_WIN95:   pszTemp = _T("Windows 95");            break;
    case WINVER_WIN98:   pszTemp = _T("Windows 98");            break;
    case WINVER_WINME:   pszTemp = _T("Windows ME");            break;
    case WINVER_WINNT4:  pszTemp = _T("Windows NT 4.0");        break;
    case WINVER_WIN2000: pszTemp = _T("Windows 2000");          break;
    case WINVER_WINXP:   pszTemp = _T("Windows XP");            break;
    case WINVER_VISTA:   pszTemp = _T("Windows Vista");         break;
    case WINVER_WIN7:    pszTemp = _T("Windows 7");             break;
    case WINVER_WIN8:    pszTemp = _T("Windows 8");             break;
    case WINVER_WIN2003:
        if (GetSystemMetrics(SM_SERVERR2))
        {
            pszTemp = _T("Windows Server 2003 R2");
        }
        else
        {
            pszTemp = _T("Windows Server 2003");
        }
        break;
    case WINVER_WHS:     pszTemp = _T("Windows Home Server");   break;
    case WINVER_WIN2008: pszTemp = _T("Windows Server 2008");   break;
    case WINVER_WIN2012: pszTemp = _T("Windows Server 2012");   break;
    default:             pszTemp = _T("<Unknown Version>");     break;
    }
    
    HRESULT hr = StringCchCopy(pszVersion, cchVersion, pszTemp);
    
    if (SUCCEEDED(hr))
    {
#if !defined(WIN64)
        if (IsOS(OS_WOW6432))
#endif
        {
            StringCchCat(pszVersion, cchVersion, _T(" (64-Bit)"));
        }
    }
    
    return hr;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutSysInfo
//
static void AboutSysInfo(HWND hListView)
{
    LVCOLUMN columnInfo;
    LVITEM itemInfo;
    int i = 0;
    char buffer[MAX_PATH];
    char text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    strcpy(text, "Name");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 3 + width / 8;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    /* Using this odd size, keeps the columns aligned with
     * the other list views, and also gives the text a little
     * more room to keep from being truncated. */
    strcpy(text, "Value");
    columnInfo.cx = (2 * width) / 3 - width / 8;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    // operating system and version
    strcpy(text, "Operating System");
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    GetWinVerString(buffer, COUNTOF(buffer));
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    // memory information
    DWORD dwMemoryLoad;
    DWORDLONG dwTotalPhys, dwAvailPhys;
    DWORDLONG dwTotalPageFile, dwAvailPageFile;
    
    typedef BOOL (WINAPI *GMSExFunctionType)(LPMEMORYSTATUSEX);
    GMSExFunctionType fpGlobalMemoryStatusEx = (GMSExFunctionType)GetProcAddress(
        GetModuleHandle("KERNEL32.DLL"), "GlobalMemoryStatusEx");
    
    if (fpGlobalMemoryStatusEx)
    {
        MEMORYSTATUSEX ms;
        ms.dwLength = sizeof(MEMORYSTATUSEX);
        fpGlobalMemoryStatusEx(&ms);
        
        dwMemoryLoad = (DWORD)ms.dwMemoryLoad;
        dwTotalPhys = (DWORDLONG)ms.ullTotalPhys;
        dwAvailPhys = (DWORDLONG)ms.ullAvailPhys;
        dwTotalPageFile = (DWORDLONG)ms.ullTotalPageFile;
        dwAvailPageFile = (DWORDLONG)ms.ullAvailPageFile;
    }
    else
    {
        MEMORYSTATUS ms;
        ms.dwLength = sizeof(MEMORYSTATUS);
        GlobalMemoryStatus(&ms);
        
        dwMemoryLoad = (DWORD)ms.dwMemoryLoad;
        dwTotalPhys = (DWORDLONG)ms.dwTotalPhys;
        dwAvailPhys = (DWORDLONG)ms.dwAvailPhys;
        dwTotalPageFile = (DWORDLONG)ms.dwTotalPageFile;
        dwAvailPageFile = (DWORDLONG)ms.dwAvailPageFile;
    }
    
    strcpy(text, "Memory Load");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    StringCchPrintf(buffer, MAX_PATH, "%d%%", dwMemoryLoad);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    strcpy(text, "Physical Memory Total");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwTotalPhys, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    strcpy(text, "Physical Memory Available");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwAvailPhys, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    strcpy(text, "Swap Space Total");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwTotalPageFile, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    strcpy(text, "Swap Space Available");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwAvailPageFile, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// CreateSimpleFont
// Simplified version of CreateFont
//
static HFONT CreateSimpleFont(LPCSTR pszName, int nSizeInPoints, bool bBold)
{
    ASSERT(NULL != pszName); ASSERT(nSizeInPoints > 0);
    
    // convert size from points to pixels
    HDC hDC = GetDC(NULL);
    int sizeInPixels = -MulDiv(nSizeInPoints,
        GetDeviceCaps(hDC, LOGPIXELSY), 72);
    
    ReleaseDC(NULL, hDC);
    
    // fill in LOGFONT structure
    LOGFONT lf = { 0 };
    lf.lfHeight = sizeInPixels;
    lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
    StringCchCopy(lf.lfFaceName, LF_FACESIZE, pszName);
    
    // create it
    return CreateFontIndirect(&lf);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetClientWidth
// Return the width of the window's client area
//
static int GetClientWidth(HWND hWnd)
{
    RECT r;
    GetClientRect(hWnd, &r);
    return r.right - r.left;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// FormatBytes
// Formats a byte count into a string suitable for display to the user
//
// Note: Max value of stBytes is 16 EB, so no need to be concerned of
//       overrunning units[] index.
//
static LPCSTR units[] = { "bytes", "KB", "MB", "GB", "TB", "PB", "EB" };

static void FormatBytes(DWORDLONG stBytes, LPSTR pszBuffer, size_t cchBuffer)
{
    double dValue = (double)stBytes;
    unsigned int uUnit = 0;
    
    while ((dValue >= 1024) && (uUnit < COUNTOF(units) - 1))
    {
        dValue /= 1024;
        ++uUnit;
    }
    
    if (uUnit >= 3)
    {
        StringCchPrintf(
            pszBuffer, cchBuffer,
            "%.2f %s",
            dValue, units[uUnit]);
    }
    else
    {
        StringCchPrintf(
            pszBuffer, cchBuffer,
            "%d %s",
            (int)floor(dValue + 0.5), units[uUnit]);
    }
}
