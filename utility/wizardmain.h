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
#ifndef __INCLUDE_WIZARDMAIN_H
#define __INCLUDE_WIZARDMAIN_H

#include "wizardpage.h"

class CWizardPage;

class CWizardMain
{
private:
	PROPSHEETHEADER m_pshHeader;

	struct PROPERTYPAGE_ITEM *m_pPageHead;
	struct PROPERTYPAGE_ITEM *m_pPageTail;

	UINT m_nPages;
	UINT m_nLastPage;
	HINSTANCE m_hInstance;

	void _Create(HINSTANCE hInstance, HBITMAP hbmWaterMark, HBITMAP hbmHeader);

public:
	CWizardMain(HINSTANCE hInstance);
	CWizardMain(HINSTANCE hInstance, HBITMAP hbmWaterMark, HBITMAP hbmHeader);
	virtual ~CWizardMain();

	void set_LastPage(UINT nLastPage)
	{
		m_nLastPage = nLastPage;
	};
	UINT get_LastPage()
	{
		return m_nLastPage;
	};

	void AddPage(CWizardPage* wpPage);

	int RunWizard();
};

#endif  // __INCLUDE_WIZARDMAIN_H
