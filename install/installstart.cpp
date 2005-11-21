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
#include "installstart.h"

CInstallStartPage::CInstallStartPage(UINT nTemplate):
		CWizardPage(nTemplate)
{
}

CInstallStartPage::~CInstallStartPage()
{
}

void CInstallStartPage::onInitDialog(Message& message)
{
	CWizardPage::onInitDialog(message);

	SetStaticText(message.hWnd, IDC_INSTALL_TITLE, IDS_INSTALL_INTRO_TITLE,
	              "LiteStep 0.24.7 Installation");

	SetStaticText(message.hWnd, IDC_INSTALL_INTRO_TEXT, IDS_INSTALL_INTRO_TEXT,
	              "This wizard will help you setup your computer to use an \
	              alternative shell. If you are unsure of anything at anytime, \
	              please cancel the process. This process can be undone by running \
	              the uninstall option of the alternative shell you are using.");
}

void CInstallStartPage::onWizardNext(Message& message)
{
	CWizardPage::onWizardNext(message);
	
	//
	// BUGBUG: This is causing an access violation in CWizardMain::set_LastPage()
	// not sure why... so for now, we call CWizardPage::onWizardNext, then set
	// the Message.lResult to the correct page we want to jump to next
	//
	// Set the back page
	//HWND hParent = GetParent(message.hWnd);
	//int nCurrentPage = PropSheet_HwndToIndex(hParent, message.hWnd);
	//m_pwmWizard->set_LastPage(nCurrentPage);


	// Get Windows version
	OSVERSIONINFO osInfo;
	ZeroMemory(&osInfo, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	GetVersionEx(&osInfo);

	// Set the next page
	if (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		message.lResult = IDD_INSTALL_NT_USERS;
	}
	else
	{
		message.lResult = IDD_INSTALL_CONFIRM_CHANGES;
	}
}
