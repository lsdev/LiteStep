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
#include "installntuser.h"

CInstallNTUser::CInstallNTUser(UINT nTemplate):
		CWizardPage(nTemplate)
{
}

CInstallNTUser::~CInstallNTUser()
{
}

void CInstallNTUser::onInitDialog(Message& message)
{
	CWizardPage::onInitDialog(message);

	SAFE_CHAR(szBuffer, 64);
	DWORD dwLength = 64;
	
	SetStaticText(message.hWnd, IDC_INSTALL_NT_USERS, IDS_INSTALL_NT_USERS,
	              "How would you like to install %s?", "LiteStep");

	SetStaticText(message.hWnd, IDC_INSTALL_NT_ALL_USERS, IDS_INSTALL_NT_ALL_USERS,
	              "Install for ALL users (%s will become the shell for all users on \
				  this computer)", "LiteStep");

	GetUserName(szBuffer, &dwLength);

	SetStaticText(message.hWnd, IDC_INSTALL_NT_CURRENT_USER, IDS_INSTALL_NT_CURRENT_USER,
	              "Install for CURRENT user (%s will only become the shell for %s)",
				  "LiteStep", szBuffer);
}
