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
#include "../litestep/resource.h"
#include "../utility/core.hpp"
#include <CommCtrl.h>
#include <WindowsX.h>
#include <math.h>
#include <strsafe.h>


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
static HFONT CreateSimpleFont(LPCWSTR pszName, int nSizeInPoints, bool bBold);
static int GetClientWidth(HWND hWnd);
static void FormatBytes(DWORDLONG stBytes, LPWSTR pszBuffer, size_t cchBuffer);


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
    const wchar_t *option;
    AboutFunction function;
}
static const g_aboutOptions[] = \
{
     { L"Bang Commands",      AboutBangs       }
    ,{ L"Development Team",   AboutDevTeam     }
    ,{ L"License",            AboutLicense     }
    ,{ L"Loaded Modules",     AboutModules     }
    ,{ L"Performance",        AboutPerformance }
    ,{ L"Revision IDs",       AboutRevIDs      }
    ,{ L"System Information", AboutSysInfo     }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LiteStep Development Team
//
struct TheDevTeam
{
    const wchar_t *nick;
    const wchar_t *realName;
}
static const g_theDevTeam[] = \
{
     { L"Acidfire",      L"Alexander Vermaat" }
    ,{ L"alur",          L"Erik Welander"     }
    ,{ L"ilmcuts",       L"Simon"             }
    ,{ L"jugg",          L"Chris Rempel"      }
    ,{ L"Maduin",        L"Kevin Schaffer"    }
    ,{ L"RabidCow",      L"Joshua Seagoe"     }
    ,{ L"Tobbe",         L"Tobbe Lundberg"    }
    ,{ L"Xjill",         L"Vidar T. Fauske"   }
};


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// LiteStep License Notice
//
static const WCHAR g_wzLicense[] = \
 L"LiteStep is a replacement shell for the standard Windows® Explorer shell.\r\n"
 L"\r\n"
 L"Copyright (C) 1997-1998  Francis Gastellu\r\n"
 L"Copyright (C) 1998-2013  LiteStep Development Team\r\n"
 L"\r\n"
 L"This program is free software; you can redistribute it and/or modify it "
 L"under the terms of the GNU General Public License as published by the Free "
 L"Software Foundation; either version 2 of the License, or (at your option) "
 L"any later version.\r\n"
 L"\r\n"
 L"This program is distributed in the hope that it will be useful, but WITHOUT "
 L"ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
 L"FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for "
 L"more details.\r\n"
 L"\r\n"
 L"You should have received a copy of the GNU General Public License along with "
 L"this program; if not, write to the Free Software Foundation, Inc., 51 "
 L"Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\r\n"
 L"\r\n"
 L"http://www.lsdev.org/";


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
static HWND g_hAboutbox = nullptr;


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
    
    HFONT hTitleFont = CreateSimpleFont(L"Verdana", 14, false);
    SetWindowFont(hwndTitle, hTitleFont, FALSE);
    
    SetWindowTextW(hwndTitle, L"LiteStep 0.25.0 Alpha");
    
    //
    // Initialize theme info
    //
    wchar_t themeAuthor[16] = { 0 };
    wchar_t themeName[21] = { 0 };
    wchar_t themeOut[MAX_LINE_LENGTH] = { 0 };
    
    GetRCStringW(L"ThemeAuthor", themeAuthor, L"(unknown)", COUNTOF(themeAuthor));
    GetRCStringW(L"ThemeName", themeName, L"(unknown)", COUNTOF(themeName));
    
    if (SUCCEEDED(StringCchPrintfW(themeOut, COUNTOF(themeOut),
        L"Theme: %ls by %ls", themeName, themeAuthor)))
    {
        SetDlgItemText(hwndDialog, IDC_THEME_INFO, themeOut);
    }
    
    //
    // Initialize compile time
    //
    wchar_t compileTime[42] = { 0 };
    LSGetVariableExW(L"CompileDate", compileTime, 42);
    SetDlgItemText(hwndDialog, IDC_COMPILETIME, compileTime);
    
    //
    // Initialize edit control (license notice)
    //
    SetDlgItemTextW(hwndDialog, IDC_EDIT, g_wzLicense);
    
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
        
        SetWindowTextW(hwndCtl, g_wzLicense);
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
    HMODULE hModule, LPCWSTR pszName, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem;
    itemInfo.pszText = (wchar_t*)pszName;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    wchar_t szModule[MAX_PATH] = { 0 };
    
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
    wchar_t text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    StringCchCopy(text, _countof(text), L"Bang Command");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 2;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    StringCchCopy(text, _countof(text), L"Module");
    columnInfo.cx = width - columnInfo.cx;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSDataW(ELD_BANGS_V2, (FARPROC)BangCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// AboutDevTeam
//
static void AboutDevTeam(HWND hListView)
{
    LVCOLUMN columnInfo;
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    wchar_t text[32];
    
    StringCchCopy(text, _countof(text), L"Nick");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 3;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    StringCchCopy(text, _countof(text), L"Real Name");
    columnInfo.cx = (2 * width) / 3;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    for (unsigned int i = 0; i < COUNTOF(g_theDevTeam); ++i)
    {
        LVITEM itemInfo;
        
        itemInfo.mask = LVIF_TEXT;
        itemInfo.iItem = i;
        itemInfo.pszText = (wchar_t*)g_theDevTeam[i].nick;
        itemInfo.iSubItem = 0;
        
        ListView_InsertItem(hListView, &itemInfo);
        ListView_SetItemText(hListView, i, 1, (wchar_t*)g_theDevTeam[i].realName);
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
static BOOL CALLBACK ModulesCallback(LPCWSTR pszPath, DWORD /* dwFlags */, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem++;
    itemInfo.pszText = (wchar_t*)pszPath;
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
    wchar_t text[32];
    
    StringCchCopy(text, _countof(text), L"Module");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSDataW(ELD_MODULES, (FARPROC)ModulesCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// PerformanceCallback
// Used by AboutPerformance
//
static BOOL CALLBACK PerformanceCallback(LPCWSTR pszPath, DWORD dwLoadTime, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    wchar_t szModule[MAX_PATH] = { 0 };
    StringCchCopy(szModule, _countof(szModule), pszPath);
    PathStripPath(szModule);
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem;
    itemInfo.pszText = szModule;
    itemInfo.iSubItem = 0;
    
    ListView_InsertItem(pCi->hListView, &itemInfo);
    
    StringCchPrintf(szModule, _countof(szModule), L"%dms", dwLoadTime);
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
    wchar_t text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    StringCchCopy(text, _countof(text), L"Module");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 2;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    StringCchCopy(text, _countof(text), L"Load Time");
    columnInfo.cx = width - columnInfo.cx;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSDataW(ELD_PERFORMANCE, (FARPROC)PerformanceCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// RevIDCallback
// Used by AboutRevIDs
//
static BOOL CALLBACK RevIDCallback(LPCWSTR pszRevID, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;
    
    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem++;
    itemInfo.pszText = (wchar_t*)pszRevID;
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
    wchar_t text[32];
    
    StringCchCopy(text, _countof(text), L"Revision ID");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    ci.nItem = 0;
    
    EnumLSDataW(ELD_REVIDS, (FARPROC)RevIDCallback, (LPARAM)&ci);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// GetWinVerString
// Helper for AboutSysInfo
//
static HRESULT GetWinVerString(LPWSTR pwzVersion, DWORD cchVersion)
{
    ASSERT(pwzVersion != nullptr);
    
    UINT uVersion = GetWindowsVersion();
    
    LPCWSTR pwzTemp = nullptr;
    
    switch (uVersion)
    {
    case WINVER_WIN95:   pwzTemp = L"Windows 95";            break;
    case WINVER_WIN98:   pwzTemp = L"Windows 98";            break;
    case WINVER_WINME:   pwzTemp = L"Windows ME";            break;
    case WINVER_WINNT4:  pwzTemp = L"Windows NT 4.0";        break;
    case WINVER_WIN2000: pwzTemp = L"Windows 2000";          break;
    case WINVER_WINXP:   pwzTemp = L"Windows XP";            break;
    case WINVER_VISTA:   pwzTemp = L"Windows Vista";         break;
    case WINVER_WIN7:    pwzTemp = L"Windows 7";             break;
    case WINVER_WIN8:    pwzTemp = L"Windows 8";             break;
    case WINVER_WIN81:   pwzTemp = L"Windows 8.1";           break;
    case WINVER_WIN2003:
        if (GetSystemMetrics(SM_SERVERR2))
        {
            pwzTemp = L"Windows Server 2003 R2";
        }
        else
        {
            pwzTemp = L"Windows Server 2003";
        }
        break;
    case WINVER_WHS:       pwzTemp = L"Windows Home Server";     break;
    case WINVER_WIN2008:   pwzTemp = L"Windows Server 2008";     break;
    case WINVER_WIN2008R2: pwzTemp = L"Windows Server 2008 R2";  break;
    case WINVER_WIN2012:   pwzTemp = L"Windows Server 2012";     break;
    case WINVER_WIN2012R2: pwzTemp = L"Windows Server 2012 R2";  break;
    default:               pwzTemp = L"<Unknown Version>";       break;
    }
    
    HRESULT hr = StringCchCopyW(pwzVersion, cchVersion, pwzTemp);
    
    if (SUCCEEDED(hr))
    {
#if !defined(_WIN64)
        if (IsOS(OS_WOW6432))
#endif
        {
            StringCchCatW(pwzVersion, cchVersion, L" (64-Bit)");
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
    LVCOLUMNW columnInfo;
    LVITEMW itemInfo;
    int i = 0;
    wchar_t buffer[MAX_PATH];
    wchar_t text[32];
    
    int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    
    StringCchCopyW(text, _countof(text), L"Name");
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = width / 3 + width / 8;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 0;
    
    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    /* Using this odd size, keeps the columns aligned with
     * the other list views, and also gives the text a little
     * more room to keep from being truncated. */
    StringCchCopyW(text, _countof(text), L"Value");
    columnInfo.cx = (2 * width) / 3 - width / 8;
    columnInfo.pszText = text;
    columnInfo.iSubItem = 1;
    
    ListView_InsertColumn(hListView, 1, &columnInfo);
    
    // operating system and version
    StringCchCopyW(text, _countof(text), L"Operating System");
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
        GetModuleHandleW(L"KERNEL32.DLL"), "GlobalMemoryStatusEx");
    
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
    
    StringCchCopyW(text, _countof(text), L"Memory Load");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    StringCchPrintfW(buffer, MAX_PATH, L"%d%%", dwMemoryLoad);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    StringCchCopyW(text, _countof(text), L"Physical Memory Total");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwTotalPhys, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    StringCchCopyW(text, _countof(text), L"Physical Memory Available");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwAvailPhys, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    StringCchCopyW(text, _countof(text), L"Swap Space Total");
    itemInfo.iItem = i;
    itemInfo.pszText = text;
    
    ListView_InsertItem(hListView, &itemInfo);
    
    FormatBytes(dwTotalPageFile, buffer, 64);
    ListView_SetItemText(hListView, i++, 1, buffer);
    
    StringCchCopyW(text, _countof(text), L"Swap Space Available");
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
static HFONT CreateSimpleFont(LPCWSTR pwzName, int nSizeInPoints, bool bBold)
{
    ASSERT(nullptr != pwzName); ASSERT(nSizeInPoints > 0);
    
    // convert size from points to pixels
    HDC hDC = GetDC(nullptr);
    int sizeInPixels = -MulDiv(nSizeInPoints,
        GetDeviceCaps(hDC, LOGPIXELSY), 72);
    
    ReleaseDC(nullptr, hDC);
    
    // fill in LOGFONT structure
    LOGFONTW lf = { 0 };
    lf.lfHeight = sizeInPixels;
    lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
    StringCchCopyW(lf.lfFaceName, LF_FACESIZE, pwzName);
    
    // create it
    return CreateFontIndirectW(&lf);
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
static LPCWSTR units[] = { L"bytes", L"KB", L"MB", L"GB", L"TB", L"PB", L"EB" };

static void FormatBytes(DWORDLONG stBytes, LPWSTR pwzBuffer, size_t cchBuffer)
{
    long double dValue = (long double) stBytes;
    unsigned int uUnit = 0;
    
    while ((dValue >= 1024) && (uUnit < COUNTOF(units) - 1))
    {
        dValue /= 1024;
        ++uUnit;
    }
    
    if (uUnit >= 3)
    {
        StringCchPrintfW(
            pwzBuffer, cchBuffer,
            L"%.2Lf %ls",
            dValue, units[uUnit]);
    }
    else
    {
        StringCchPrintfW(
            pwzBuffer, cchBuffer,
            L"%d %ls",
            (int)floor(dValue + 0.5), units[uUnit]);
    }
}
