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
#include "common.h"
#include <commctrl.h>
#include <objbase.h>
#include <oleauto.h>
#include <io.h>
#include <math.h>

extern const char rcsRevision[];

enum
{
    ABOUT_BANGS = 0,
    ABOUT_DEVTEAM = 1,
    ABOUT_MODULES = 2,
    ABOUT_REVIDS = 3,
    ABOUT_SYSINFO = 4,
    ABOUT_CHANGES = 5
};

typedef void (*AboutFunction)(HWND);

void AboutBangs(HWND);
void AboutDevTeam(HWND);
void AboutModules(HWND);
void AboutRevIDs(HWND);
void AboutSysInfo(HWND);
void AboutChanges(HWND);

HFONT CreateSimpleFont(LPCSTR, int, BOOL, BOOL);
int GetClientWidth(HWND);
void TrimLeft(char *);
void FormatBytes(DWORD, char *);


// AboutBox Dialog Procedure
//

struct
{
	const char *option;
	AboutFunction function;
}
aboutOptions[] = {
                     "Bang Commands", AboutBangs,
                     "Development Team", AboutDevTeam,
                     "Loaded Modules", AboutModules,
                     "Revision IDs", AboutRevIDs,
                     "System Information", AboutSysInfo,
                     "Changes.txt", AboutChanges
                 };

const int aboutOptionsCount = sizeof(aboutOptions) / sizeof(aboutOptions[0]);

BOOL WINAPI AboutBoxProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_COMBOBOX && HIWORD(wParam) == CBN_SELCHANGE)
			{
				int i;

				// delete listview items
				SendDlgItemMessage(hWnd, IDC_LISTVIEW, LVM_DELETEALLITEMS, 0, 0);

				// delete listview columns
				for (i = 3; i >= 0; i--)
					SendDlgItemMessage(hWnd, IDC_LISTVIEW, LVM_DELETECOLUMN, (WPARAM) i, 0);

				// get new selection
				i = SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_GETCURSEL, 0, 0);

				// fill the listview
				aboutOptions[i].function(GetDlgItem(hWnd, IDC_LISTVIEW));
			}
			else if (LOWORD(wParam) == IDOK)
			{
				// release title font
				HFONT titleFont = (HFONT) SendDlgItemMessage(hWnd, IDC_TITLE, WM_GETFONT, 0, 0);
				DeleteObject(titleFont);

				// close the dialog box
				EndDialog(hWnd, IDOK);
				return TRUE;
			}
			return FALSE;
		}

		case WM_CTLCOLORSTATIC:
		{
			// the header and title need a white (COLOR_WINDOW) background
			int id = GetDlgCtrlID((HWND) lParam);

			if (id == IDC_TITLE || id == IDC_THEME_INFO)
			{
				HDC hDC = (HDC) wParam;

				SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
				SetBkColor(hDC, GetSysColor(COLOR_WINDOW));

				return (BOOL) GetSysColorBrush(COLOR_WINDOW);
			}
			else if (id == IDC_HEADER || id == IDC_LOGO)
			{
				return (BOOL) GetSysColorBrush(COLOR_WINDOW);
			}

			return FALSE;
		}

		case WM_INITDIALOG:
		{
			// set title's font to Verdana 13pt Bold
			HFONT titleFont = CreateSimpleFont("Verdana", 14, FALSE, FALSE);
			SendDlgItemMessage(hWnd, IDC_TITLE, WM_SETFONT, (WPARAM) titleFont, FALSE);

			// set title with LS version
			SetDlgItemText(hWnd, IDC_TITLE, "LiteStep 0.24.7");

			//set Theme info
			char themeAuthor[16];
			char themeName[21];
			char themeOut[MAX_LINE_LENGTH];
			themeName[0] = themeAuthor[0] = '\0';

			GetRCString("ThemeAuthor", themeAuthor, "(unknown)", sizeof(themeAuthor));
			GetRCString("ThemeName", themeName, "", sizeof(themeName));

			if (themeName[0])
				wsprintf(themeOut, "Theme: %s by %s", themeName, themeAuthor);
			else
				wsprintf(themeOut, "Theme by %s", themeAuthor);

			SetDlgItemText(hWnd, IDC_THEME_INFO, themeOut);

			// set compile time
			char compileTime[64];
			wsprintf(compileTime, "Compiled on %s at %s", __DATE__, __TIME__);
			SetDlgItemText(hWnd, IDC_COMPILETIME, compileTime);

			// add options to combo box
			for (int i = 0; i < aboutOptionsCount - 1; i++)
				SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_ADDSTRING, 0, (LPARAM) aboutOptions[i].option);

			_finddata_t found;
			char szChangesTxtPath[MAX_PATH];
			long handle = 0;

			LSGetLitestepPath(szChangesTxtPath, MAX_PATH);
			strcat(szChangesTxtPath, "changes.txt");

			handle = _findfirst(szChangesTxtPath, &found);

			if (handle != -1L)
			{
				SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_ADDSTRING, 0, (LPARAM) aboutOptions[5].option);
			}

			// default to revision IDs
			SendDlgItemMessage(hWnd, IDC_COMBOBOX, CB_SETCURSEL, ABOUT_REVIDS, 0);
			AboutRevIDs(GetDlgItem(hWnd, IDC_LISTVIEW));

			// center dialog on screen
			RECT rc;
			GetWindowRect(hWnd, &rc);

			SetWindowPos(hWnd,
			             HWND_TOP,
			             (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
			             (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2,
			             0, 0,
			             SWP_NOSIZE);

			// bring us to the front
			BOOL (WINAPI * SwitchToThisWindow)(HWND, BOOL);

			SwitchToThisWindow = (BOOL (WINAPI *)(HWND, BOOL)) GetProcAddress(
			                         GetModuleHandle("USER32.DLL"), "SwitchToThisWindow");

			SwitchToThisWindow(hWnd, TRUE);

			return TRUE;
		}
	}

	return FALSE;
}


// AboutBox Thread Procedure
//

ULONG WINAPI AboutBoxThread(void *)
{
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), NULL,
	          (DLGPROC) AboutBoxProcedure);

	return 0;
}


// Fill listview with bang command information
//

void AboutBangs(HWND hListView)
{
	LVCOLUMN columnInfo;

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
	columnInfo.pszText = "Bang Command";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	// get bang command names from bang manager
	/*ILitestep *litestep;
	IBangManager *bangMgr;

	litestep = (ILitestep *) SendMessage( GetLitestepWnd(), LM_GETLSOBJECT, 0, 0 );
	litestep->GetBangManager( &bangMgr );

	if ( !bangMgr )
		return ;

	long count = bangMgr->GetBangCommandCount(), total = 0;
	BSTR *array = new BSTR[count];
	if ( !array )
		return ;

	total = bangMgr->GetBangCommandNames(count, array);

	if ( !total )
		return ;

	for ( int i = 0; i < count; i++ )
	{
		LVITEM itemInfo;
		char buffer[64];

		WideCharToMultiByte( CP_ACP,
		                     0,
		                     array[i],
		                     -1,
		                     buffer,
		                     64,
		                     NULL,
		                     NULL );

		SysFreeString(array[i]);

		itemInfo.mask = LVIF_TEXT;
		itemInfo.iItem = i;
		itemInfo.pszText = buffer;
		itemInfo.iSubItem = 0;

		ListView_InsertItem( hListView, &itemInfo );
	}

	// free up array damnit
	delete array;*/

}


// Fill listview with development team information
//

struct
{
	const char *nick;
	const char *realName;
}
theDevTeam[] =
    {
        "Headius", "Charles Oliver Nutter",
        "Message", "Bobby G. Vinyard",
        //        "NeXTer", "Joachim Calvert",
        "Maduin", "Kevin Schaffer",
        //        "murphy", "Torsten Stelling",
        //        "grd", "Gustav Munkby",
        //        "GeekMaster", "Lowell Heddings",
        //        "c0mrade", "Kirill Arushanov",
        //        "Noodge", "Dustin Williams",
        "Chaku", "Chao-Kuo Lin"
    };

const int theDevTeamCount = sizeof(theDevTeam) / sizeof(theDevTeam[0]);

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

void AboutModules(HWND hListView)
{
	LVCOLUMN columnInfo;

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
	columnInfo.pszText = "Module";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	// get module names from module manager
	/*ILitestep *litestep;
	IModuleManager *moduleMgr;

	litestep = (ILitestep *) SendMessage( GetLitestepWnd(), LM_GETLSOBJECT, 0, 0 );
	litestep->GetModuleManager( &moduleMgr );

	if ( !moduleMgr )
		return ;

	int count = 0;
	//moduleMgr->GetModuleCount( &count );

	if ( count == 0 )
		return ;

	SAFEARRAY *array = SafeArrayCreateVector( VT_BSTR, 0, count );
	//moduleMgr->GetModuleList( array, &count );

	BSTR *arrayData;
	SafeArrayAccessData( array, (void **) &arrayData );

	for ( int i = 0; i < count; i++ )
	{
		LVITEM itemInfo;
		char buffer[MAX_PATH];

		WideCharToMultiByte( CP_ACP,
		                     0,
		                     arrayData[i],
		                     -1,
		                     buffer,
		                     MAX_PATH,
		                     NULL,
		                     NULL );

		itemInfo.mask = LVIF_TEXT;
		itemInfo.iItem = i;
		itemInfo.pszText = buffer;
		itemInfo.iSubItem = 0;

		ListView_InsertItem( hListView, &itemInfo );
	}

	SafeArrayUnaccessData( array );
	SafeArrayDestroy( array );*/
}


// Fill listview with revision ID (LM_GETREVID) information
//

void AboutRevIDs(HWND hListView)
{
	LVCOLUMN columnInfo;
	LVITEM itemInfo;
	int i = 0;
	char buffer[256];

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

	strcpy(buffer, "litestep.exe: 0.24.7");
	ListView_InsertItem(hListView, &itemInfo);

	itemInfo.iItem = i++;

	strcpy(buffer, "lsapi.dll: ");
	strcat(buffer, &rcsRevision[11]);
	buffer[strlen(buffer) - 1] = 0;

	ListView_InsertItem(hListView, &itemInfo);

	// ask message manager which windows handle LM_GETREVID
	/*ILitestep *litestep;
	IMessageManager *msgMgr;

	litestep = (ILitestep *) SendMessage( GetLitestepWnd(), LM_GETLSOBJECT, 0, 0 );
	litestep->GetMessageManager( &msgMgr );

	if ( !msgMgr )
		return ;

	SAFEARRAY *array = NULL;
	msgMgr->GetWindowsForMessage( LM_GETREVID, &array );

	if ( !array )
		return ;

	long lbound, ubound;

	SafeArrayGetLBound( array, 1, &lbound );
	SafeArrayGetUBound( array, 1, &ubound );

	int count = ubound - lbound + 1;

	HWND *arrayData;
	SafeArrayAccessData( array, (void **) &arrayData );

	for ( int j = 0; j < count; j++ )
	{
		if ( !IsWindow( arrayData[j] ) )
			continue;

		buffer[0] = 0;
		SendMessage( arrayData[j], LM_GETREVID, 0, (LPARAM) buffer );

		itemInfo.mask = LVIF_TEXT;
		itemInfo.iItem = i++;
		itemInfo.pszText = buffer;
		itemInfo.iSubItem = 0;

		ListView_InsertItem( hListView, &itemInfo );
	}

	SafeArrayUnaccessData( array );
	SafeArrayDestroy( array );*/
}


// Fill listview with _tsystem information
//

void AboutSysInfo(HWND hListView)
{
	LVCOLUMN columnInfo;
	LVITEM itemInfo;
	int i = 0;
	char buffer[64];

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

	// operating _tsystem and version
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
			strcpy(buffer, "Windows ME");
		else if (versionInfo.dwMinorVersion >= 10)
			strcpy(buffer, "Windows 98");
		else
			strcpy(buffer, "Windows 95");
	}
	else
	{
		if ((versionInfo.dwMajorVersion == 5) && (versionInfo.dwMinorVersion >= 1))
			strcpy(buffer, "Windows XP");
		else if (versionInfo.dwMajorVersion == 5)
			strcpy(buffer, "Windows 2000");
		else
			sprintf(buffer, "Windows NT %d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
	}

	TrimLeft(versionInfo.szCSDVersion);

	if (versionInfo.szCSDVersion[0])
	{
		lstrcat(buffer, " (");
		lstrcat(buffer, versionInfo.szCSDVersion);
		lstrcat(buffer, ")");
	}

	ListView_SetItemText(hListView, i++, 1, buffer);

	// memory information
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	itemInfo.iItem = i;
	itemInfo.pszText = "Memory Load";

	ListView_InsertItem(hListView, &itemInfo);

	sprintf(buffer, "%d%%", ms.dwMemoryLoad);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Physical Memory Total";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwTotalPhys, buffer);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Physical Memory Available";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwAvailPhys, buffer);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Swap Space Total";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwTotalPageFile, buffer);
	ListView_SetItemText(hListView, i++, 1, buffer);

	itemInfo.iItem = i;
	itemInfo.pszText = "Swap Space Available";

	ListView_InsertItem(hListView, &itemInfo);

	FormatBytes(ms.dwAvailPageFile, buffer);
	ListView_SetItemText(hListView, i++, 1, buffer);

}

void AboutChanges(HWND hListView)
{
	LVCOLUMN columnInfo;

	columnInfo.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	columnInfo.fmt = LVCFMT_LEFT;
	columnInfo.cx = GetClientWidth(hListView) - GetSystemMetrics(SM_CXVSCROLL);
	columnInfo.pszText = "Changes";
	columnInfo.iSubItem = 0;

	ListView_InsertColumn(hListView, 0, &columnInfo);

	char szChangesTxtPath[MAX_PATH];

	LSGetLitestepPath(szChangesTxtPath, MAX_PATH);
	strcat(szChangesTxtPath, "changes.txt");

	FILE *f;

	f = fopen (szChangesTxtPath, "r");

	if (f)
	{
		fseek (f, 0, SEEK_SET);
	}

	char szTempBuffer[MAX_LINE_LENGTH];
	int i = 0;
	int j = 0;
	int width = columnInfo.cx;

	while (f && !feof (f))
	{

		if (!fgets (szTempBuffer, MAX_LINE_LENGTH, f))
			break;

		int tempWidth = ListView_GetStringWidth(hListView, szTempBuffer);
		if (tempWidth > width)
		{
			width = tempWidth;
			ListView_SetColumnWidth(hListView, 0, width);
		}

		LVITEM itemInfo;

		j = 0;
		while (szTempBuffer[j] != 0)
			j++;
		szTempBuffer[j - 1] = 0;

		itemInfo.mask = LVIF_TEXT;
		itemInfo.iItem = i++;
		itemInfo.pszText = szTempBuffer;
		itemInfo.iSubItem = 0;

		ListView_InsertItem(hListView, &itemInfo);
	}

	fclose(f);
}

// Simplified version of CreateFont
//

HFONT CreateSimpleFont(LPCSTR faceName, int sizeInPoints, BOOL bold, BOOL italic)
{
	// convert size from points to pixels
	HDC hDC = GetDC(NULL);
	int sizeInPixels = -MulDiv(sizeInPoints, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	ReleaseDC(NULL, hDC);

	// fill in LOGFONT structure
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight = sizeInPixels;
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = italic;
	strncpy(lf.lfFaceName, faceName, LF_FACESIZE);

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

void TrimLeft(char *toTrim)
{
	char * trimmed = toTrim;

	// skip past spaces
	while (*toTrim && isspace(*toTrim))
		toTrim++;

	// copy the rest of the string over
	while (*toTrim)
		* trimmed++ = *toTrim++;

	// null-terminate it
	*trimmed = 0;
}


// Formats a byte count into a string suitable for display to the user
//

LPCSTR units[] = {
                     "bytes", "KB", "MB", "GB", "TB"
                 };

void FormatBytes(DWORD bytes, LPSTR buffer)
{
	double value = (double) bytes;
	int unit = 0;

	while (value >= 1024)
	{
		value /= 1024;
		unit++;
	}

	wsprintf(buffer, "%d %s", (int) floor(value + 0.5), units[unit]);
}
