/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

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
#include "../utility/common.h"
#include <commctrl.h>
#include <math.h>
#include "../utility/core.hpp"

#ifdef __GNUC__
  typedef void (__stdcall *STTWTYPE)(HWND, BOOL);

  STTWTYPE SwitchToThisWindow = (STTWTYPE)GetProcAddress(
      GetModuleHandle("USER32.DLL"), "SwitchToThisWindow");
#endif

extern const char rcsRevision[];

enum
{
    ABOUT_BANGS = 0,
    ABOUT_DEVTEAM = 1,
    ABOUT_MODULES = 2,
    ABOUT_REVIDS = 3,
    ABOUT_SYSINFO = 4,
};

typedef void (*AboutFunction)(HWND);

void AboutBangs(HWND hListView);
void AboutDevTeam(HWND hListView);
void AboutModules(HWND hListView);
void AboutRevIDs(HWND hListView);
void AboutSysInfo(HWND hListView);

//
// misc functions
//
BOOL WINAPI AboutBoxProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HFONT CreateSimpleFont(LPCSTR pszName, int nSizeInPoints, bool bBold);
int GetClientWidth(HWND hWnd);
void TrimLeft(char* pszToTrim);
void FormatBytes(size_t stBytes, LPSTR pszBuffer, size_t cchBuffer);

struct
{
	const char *option;
	AboutFunction function;
}
aboutOptions[] =
{
    "Bang Commands", AboutBangs,
    "Development Team", AboutDevTeam,
    "Loaded Modules", AboutModules,
    "Revision IDs", AboutRevIDs,
    "System Information", AboutSysInfo
};

struct
{
    const char *nick;
    const char *realName;
}
theDevTeam[] =
{
    "Message", "Bobby G. Vinyard",
    "ilmcuts", "Simon"
};

const unsigned int aboutOptionsCount = sizeof(aboutOptions) / sizeof(aboutOptions[0]);
const unsigned int theDevTeamCount = sizeof(theDevTeam) / sizeof(theDevTeam[0]);

struct CallbackInfo
{
    HWND hListView;
    int nItem;
};


//
// AboutBox Dialog Procedure
//
BOOL WINAPI AboutBoxProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_COMBOBOX &&
                HIWORD(wParam) == CBN_SELCHANGE)
			{
				// delete listview items
				SendDlgItemMessage(hWnd, IDC_LISTVIEW,
                    LVM_DELETEALLITEMS, 0, 0);

				int i;

                // delete listview columns
				for (i = 3; i >= 0; i--)
                {
                    SendDlgItemMessage(hWnd, IDC_LISTVIEW,
                        LVM_DELETECOLUMN, (WPARAM)i, 0);
                }

				// get new selection
				i = (int)SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_GETCURSEL, 0, 0);

				// fill the listview
				aboutOptions[i].function(GetDlgItem(hWnd, IDC_LISTVIEW));
			}
			else if (LOWORD(wParam) == IDOK || LOWORD(wParam == IDCANCEL))
			{
				// release title font
				HFONT titleFont = (HFONT)SendDlgItemMessage(hWnd, IDC_TITLE,
                    WM_GETFONT, 0, 0);

				DeleteObject(titleFont);

				// close the dialog box
				EndDialog(hWnd, IDOK);

                return TRUE;
			}
            
			return FALSE;
		}

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
		{
			HBRUSH hbReturn = FALSE; // special return value to tell the system
                                     // to perform default message processing

            // the header and title need a white (COLOR_WINDOW) background
			int id = GetDlgCtrlID((HWND)lParam);

			if (id == IDC_TITLE || id == IDC_THEME_INFO)
			{
				HDC hDC = (HDC)wParam;

				SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
				SetBkColor(hDC, GetSysColor(COLOR_WINDOW));

				hbReturn = GetSysColorBrush(COLOR_WINDOW);
			}
			else if (id == IDC_HEADER || id == IDC_LOGO)
			{
				hbReturn = GetSysColorBrush(COLOR_WINDOW);
			}

			return (BOOL)hbReturn;
		}

		case WM_INITDIALOG:
		{
			// set title font
			HFONT hTitleFont = CreateSimpleFont("Verdana", 14, false);
			SendDlgItemMessage(hWnd, IDC_TITLE, WM_SETFONT,
                (WPARAM)hTitleFont, FALSE);

			// set title with LS version
			SetDlgItemText(hWnd, IDC_TITLE, "LiteStep 0.24.7 RC 1");

			// set Theme info
            char themeAuthor[16] = { 0 };
            char themeName[21] = { 0 };
            char themeOut[MAX_LINE_LENGTH] = { 0 };

			GetRCString("ThemeAuthor", themeAuthor, "(unknown)", sizeof(themeAuthor));

			if (GetRCString("ThemeName", themeName, NULL, sizeof(themeName)))
            {
                StringCchPrintf(themeOut, MAX_LINE_LENGTH,
                    "Theme: %s by %s", themeName, themeAuthor);
            }
			else
            {
                StringCchPrintf(themeOut, MAX_LINE_LENGTH, 
                    "Theme by %s", themeAuthor);
            }

			SetDlgItemText(hWnd, IDC_THEME_INFO, themeOut);

			// set compile time
            char compileTime[64] = { 0 };
			StringCchPrintf(compileTime, 64,
                "Compiled on %s at %s", __DATE__, __TIME__);

			SetDlgItemText(hWnd, IDC_COMPILETIME, compileTime);

			// add options to combo box
			for (int i = 0; i < aboutOptionsCount; ++i)
            {
                SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_ADDSTRING,
                    0, (LPARAM)aboutOptions[i].option);
            }

			// default to revision IDs
			SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_SETCURSEL, ABOUT_REVIDS, 0);
			AboutRevIDs(GetDlgItem(hWnd, IDC_LISTVIEW));

			// center dialog on screen
			RECT rc;
			GetWindowRect(hWnd, &rc);

			SetWindowPos(hWnd, HWND_TOP,
                (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
                0, 0, SWP_NOSIZE);

			SwitchToThisWindow(hWnd, TRUE);

			return FALSE;
		}

        default:
        break;
	}

	return FALSE;
}


// AboutBox Thread Procedure
//

ULONG WINAPI AboutBoxThread(void *)
{
    return DialogBox(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, (DLGPROC)AboutBoxProcedure);
}


// Fill listview with bang command information
//

BOOL __stdcall BangCallback(LPCSTR pszName, LPARAM lParam)
{
    CallbackInfo* pCi = (CallbackInfo*)lParam;

    LVITEM itemInfo;
    itemInfo.mask = LVIF_TEXT;
    itemInfo.iItem = pCi->nItem++;
    itemInfo.pszText = (char*)pszName;
    itemInfo.iSubItem = 0;

    ListView_InsertItem(pCi->hListView, &itemInfo);

    return TRUE;
}

void AboutBangs(HWND hListView)
{
	LVCOLUMN columnInfo;

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
	columnInfo.pszText = "Bang Command";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

    CallbackInfo ci = { 0 };
    ci.hListView = hListView;

    EnumLSData(ELD_BANGS, (FARPROC)BangCallback, (LPARAM)&ci);
}


// Fill listview with development team information
//
void AboutDevTeam(HWND hListView)
{
	LVCOLUMN columnInfo;
	int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = width / 3;
	columnInfo.pszText = "Nick";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	columnInfo.cx = (2 * width) / 3;
	columnInfo.pszText = "Real Name";
	columnInfo.iSubItem = 1;

	ListView_InsertColumn(hListView, 1, &columnInfo);

	for (int i = 0; i < theDevTeamCount; i++)
	{
		LVITEM itemInfo;

		itemInfo.mask = LVIF_TEXT;
		itemInfo.iItem = i;
		itemInfo.pszText = (char *) theDevTeam[i].nick;
		itemInfo.iSubItem = 0;

		ListView_InsertItem(hListView, &itemInfo);
		ListView_SetItemText(hListView, i, 1, (char *) theDevTeam[i].realName);
	}
}


// Fill listview with module information
//

BOOL __stdcall ModulesCallback(LPCSTR pszPath, DWORD dwFlags, LPARAM lParam)
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


void AboutModules(HWND hListView)
{
    LVCOLUMN columnInfo;
    
    columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    columnInfo.fmt = LVCFMT_LEFT;
    columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
    columnInfo.pszText = "Module";
    columnInfo.iSubItem = 0;

    ListView_InsertColumn(hListView, 0, &columnInfo);
    
    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    
    EnumLSData(ELD_MODULES, (FARPROC)ModulesCallback, (LPARAM)&ci);
}


// Fill listview with revision ID (LM_GETREVID) information
//

BOOL __stdcall RevIDCallback(LPCSTR pszRevID, LPARAM lParam)
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


void AboutRevIDs(HWND hListView)
{
	LVCOLUMN columnInfo;
	LVITEM itemInfo;
	int i = 0;
	char buffer[MAX_PATH];

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
	columnInfo.pszText = "Revision ID";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	// special cases: litestep.exe and lsapi.dll
	itemInfo.mask = LVIF_TEXT;
	itemInfo.iItem = i++;
	itemInfo.pszText = buffer;
	itemInfo.iSubItem = 0;

	StringCchCopy(buffer, MAX_PATH, "lsapi.dll: ");
	StringCchCat(buffer, MAX_PATH, &rcsRevision[11]);
	buffer[strlen(buffer) - 1] = 0;

	ListView_InsertItem(hListView, &itemInfo);

    CallbackInfo ci = { 0 };
    ci.hListView = hListView;
    ci.nItem = i;
    
    EnumLSData(ELD_REVIDS, (FARPROC)RevIDCallback, (LPARAM)&ci);
}


// Fill listview with system information
//

void AboutSysInfo(HWND hListView)
{
	LVCOLUMN columnInfo;
	LVITEM itemInfo;
	int i = 0;
	char buffer[MAX_PATH];

	int width = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = width / 2;
	columnInfo.pszText = "Name";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	columnInfo.cx = width / 2;
	columnInfo.pszText = "Value";
	columnInfo.iSubItem = 1;

	ListView_InsertColumn(hListView, 1, &columnInfo);

	// operating system and version
	itemInfo.mask = LVIF_TEXT;
	itemInfo.iItem = i;
	itemInfo.pszText = "Operating System";
	itemInfo.iSubItem = 0;

	ListView_InsertItem(hListView, &itemInfo);

	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&versionInfo);

	if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if (versionInfo.dwMinorVersion >= 90)
        {
            StringCchCopy(buffer, MAX_PATH, "Windows ME");
        }
		else if (versionInfo.dwMinorVersion >= 10)
        {
            StringCchCopy(buffer, MAX_PATH, "Windows 98");
        }
		else
        {
            StringCchCopy(buffer, MAX_PATH, "Windows 95");
        }
	}
	else
	{
		if ((versionInfo.dwMajorVersion == 5) && (versionInfo.dwMinorVersion >= 1))
        {
            StringCchCopy(buffer, MAX_PATH, "Windows XP");
        }
		else if (versionInfo.dwMajorVersion == 5)
        {
            StringCchCopy(buffer, MAX_PATH, "Windows 2000");
        }
		else
        {
            StringCchPrintf(buffer, MAX_PATH, "Windows NT %d.%d",
                versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
        }
	}

	TrimLeft(versionInfo.szCSDVersion);

	if (versionInfo.szCSDVersion[0])
	{
        StringCchCat(buffer, MAX_PATH, " (");
		StringCchCat(buffer, MAX_PATH, versionInfo.szCSDVersion);
		StringCchCat(buffer, MAX_PATH, ")");
	}

	ListView_SetItemText(hListView, i++, 1, buffer);

	// memory information
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	itemInfo.iItem = i;
	itemInfo.pszText = "Memory Load";

	ListView_InsertItem(hListView, &itemInfo);

	StringCchPrintf(buffer, MAX_PATH, "%d%%", ms.dwMemoryLoad);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Physical Memory Total";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwTotalPhys, buffer, 64);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Physical Memory Available";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwAvailPhys, buffer, 64);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Swap Space Total";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwTotalPageFile, buffer, 64);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Swap Space Available";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwAvailPageFile, buffer, 64);
	ListView_SetItemText(hListView, i++, 1, buffer);

}


// Simplified version of CreateFont
//

HFONT CreateSimpleFont(LPCSTR pszName, int nSizeInPoints, bool bBold)
{
	ASSERT_ISSTRING(pszName); ASSERT(nSizeInPoints > 0);

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


// Return the width of the window's client area
//

int GetClientWidth(HWND hWnd)
{
	RECT r;
	GetClientRect(hWnd, &r);

	return r.right - r.left;
}


// Trims whitespace from the beginning of a string in-place
//
void TrimLeft(char* pszToTrim)
{
	ASSERT_ISSTRING(pszToTrim);
    
    // skip past spaces
	while (*pszToTrim && StrChr(WHITESPACE, *pszToTrim))
    {
        ++pszToTrim;
    }

    ASSERT_ISSTRING(pszToTrim);
}


// Formats a byte count into a string suitable for display to the user
//
LPCSTR units[] = { "bytes", "KB", "MB", "GB", "TB", "PB" };

void FormatBytes(size_t stBytes, LPSTR pszBuffer, size_t cchBuffer)
{
	double dValue = (double)stBytes;
	unsigned int uUnit = 0;

	while ((dValue >= 1024) && (uUnit < (sizeof(units)/sizeof(units[0]) - 1)))
	{
		dValue /= 1024;
		++uUnit;
	}

	StringCchPrintf(pszBuffer, cchBuffer,
        "%d %s", (int)floor(dValue + 0.5), units[uUnit]);
}
