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
#ifndef __INCLUDE_WIZARDPAGE_H
#define __INCLUDE_WIZARDPAGE_H

#include "common.h"
#include <prsht.h>
#include "wizardmain.h"

#define WZ_FIRSTPAGE 0x00000001
#define WZ_LASTPAGE  0x00000002

struct Message
{
	HWND hWnd;
	UINT uMsg;
	union
	{
		struct
		{
			WPARAM wParam;
			LPARAM lParam;
			LRESULT lResult;
		};
		struct
		{
			WORD wParamLo;
			WORD wParamHi;
			WORD lParamLo;
			WORD lParamHi;
			WORD lResultLo;
			WORD lResultHi;
		};
	};
};

class CWizardMain;

class CWizardPage
{
protected:
	HPROPSHEETPAGE	m_hPage;
	PROPSHEETPAGE m_pspPage;

	DWORD m_dwFlags;
	UINT m_nIndex;
	UINT m_nCaption;
	char m_szCaption[MAX_PATH + 1];
	HINSTANCE m_hInstance;

#ifdef IDC_TITLE

	HFONT m_hTitleFont;
#endif // IDC_TITLE

	CWizardMain* m_pwmWizard;

	void _Create(LPCSTR pszTemplate, UINT nCaption);
	void _HandleButtonClick(UINT nButton);

public:
	CWizardPage(UINT nTemplate, UINT nCaption = 0);
	CWizardPage(LPCSTR pszTemplate, UINT nCaption = 0);
	virtual ~CWizardPage();

	HPROPSHEETPAGE get_Handle()
	{
		return m_hPage;
	}
	DWORD set_Flag(DWORD dwFlag)
	{
		return (m_dwFlags |= dwFlag);
	};
	DWORD get_Flag()
	{
		return m_dwFlags;
	};

	DWORD AddFlag(DWORD dwFlag)
	{
		return (m_pspPage.dwFlags |= dwFlag);
	};
	DWORD RemoveFlag(DWORD dwFlag)
	{
		return (m_pspPage.dwFlags &= ~dwFlag);
	};

	void SetHeader(UINT nHeaderTitle, UINT nHeaderSubTitle);
	void SetHeader(LPCSTR pszHeaderTitle, LPCSTR pszHeaderSubTitle);

	HPROPSHEETPAGE Create(UINT nIndex, HINSTANCE hInstance, CWizardMain* pwmWizard);

	void SetStaticText(HWND hWnd, UINT nStaticTextID, UINT nResourceID, LPCSTR pszDefText, ...);

	static BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void dialogProc(Message& message);

	virtual void onInitDialog(Message& message);
	virtual void onCtlColorDlg(Message& message);
	virtual void onCtlColorStatic(Message& message);
	virtual void onCommand(Message& message);
	virtual void onNotify(Message& message);
	virtual void onWizardSetActive(Message& message);
	virtual void onWizardNext(Message& message);
	virtual void onWizardBack(Message& message);
	virtual void onWizardFinish(Message& message);
	virtual void onWizardReset(Message& message);
};

#endif // __INCLUDE_WIZARDPAGE_H
