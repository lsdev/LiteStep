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
#include "install.h"
#include "installstart.h"
#include "installntuser.h"
#include "installconfirmchanges.h"
#include "installfinish.h"
#include "../utility/versioning.h"

HINSTANCE g_hInstance = NULL;

VOID WINAPI InstallShell(LPCSTR pszPathtoExe)
{
	// We need version 5.80 or greater of comctl32.dll to use the new
	// wizard pages
	if (GetDLLVersion("Comctl32.dll") >= PACKVERSION(5,80))
	{
		HBITMAP hbmWaterMark = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_WATERMARK));
		HBITMAP hbmHeader = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_HEADER));
		
		CWizardMain psWizard(g_hInstance, hbmWaterMark, hbmHeader);

		CInstallStartPage pspStart(IDD_INSTALL_START);
		psWizard.AddPage(&pspStart);

		CInstallNTUser pspNT(IDD_INSTALL_NT_USERS);
		pspNT.SetHeader(IDS_INSTALL_NT_USERS_TITLE, IDS_INSTALL_NT_USERS_SUBTITLE);
		psWizard.AddPage(&pspNT);

		CInstallFinish pspChanges(IDD_INSTALL_CONFIRM_CHANGES);
		psWizard.AddPage(&pspChanges);

		CInstallFinish pspFinish(IDD_INSTALL_FINISH);
		psWizard.AddPage(&pspFinish);

		psWizard.RunWizard();

		if (hbmWaterMark)
		{
			DeleteObject(hbmWaterMark);
		}
		if (hbmHeader)
		{
			DeleteObject(hbmHeader);
		}
	}
	else
	{
		// old message box way
	}
}


VOID WINAPI UninstallShell()
{
	// We need version 5.80 or greater of comctl32.dll to use the new
	// wizard pages
	if (GetDLLVersion("Comctl32.dll") >= PACKVERSION(5,80))
	{
	}
	else
	{
		// old message box way
	}
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fReason, LPVOID /*lpvReserved*/)
{
	switch (fReason)
	{
		case DLL_PROCESS_ATTACH:

		//DisableThreadLibraryCalls(hInstance);
		//
		// When we're attached to a process, we are NOT running in
		// app mode:
		//
		g_hInstance = hInstance;
		break;

		case DLL_PROCESS_DETACH:
		break;

		default:
		break;
	}

	return TRUE;
}
