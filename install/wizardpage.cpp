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
#include "wizardpage.h"
#include <windowsx.h>
#include <stdio.h>
#include "../utility/localization.h"
#include "../lsapi/lsapi.h"

CWizardPage::CWizardPage(UINT nTemplate,
                         UINT nCaption)
{
	_Create(MAKEINTRESOURCE(nTemplate), nCaption);
}

CWizardPage::CWizardPage(LPCSTR pszTemplate,
                         UINT nCaption)
{
	_Create(pszTemplate, nCaption);
}

CWizardPage::~CWizardPage()
{
#ifdef IDC_TITLE
	if (m_hTitleFont)
	{
		DeleteObject(&m_hTitleFont);
	}
#endif // IDC_TITLE
}
void CWizardPage::_Create(LPCSTR pszTemplate, UINT nCaption)
{
	ZeroMemory(&m_pspPage, sizeof(m_pspPage));
    m_pspPage.dwSize = sizeof(m_pspPage);

	m_pspPage.dwFlags = PSP_DEFAULT;
	m_pspPage.pszTemplate = pszTemplate;
	m_pspPage.pfnDlgProc = CWizardPage::DlgProc;
	m_pspPage.lParam = (LPARAM)this;

	m_nCaption = nCaption;
	m_hPage = NULL;
	m_pwmWizard = NULL;
	m_hInstance = NULL;
	m_nIndex = 0;
	m_dwFlags = 0;

#ifdef IDC_TITLE

	m_hTitleFont = NULL;
#endif // IDC_TITLE
}

void CWizardPage::SetHeader(UINT nHeaderTitle, UINT nHeaderSubTitle)
{
	SetHeader(MAKEINTRESOURCE(nHeaderTitle), MAKEINTRESOURCE(nHeaderSubTitle));
}

void CWizardPage::SetHeader(LPCSTR pszHeaderTitle, LPCSTR pszHeaderSubTitle)
{
	if (pszHeaderTitle)
	{
		m_pspPage.pszHeaderTitle = pszHeaderTitle;
		m_pspPage.dwFlags |= PSP_USEHEADERTITLE;
	}
	if (pszHeaderSubTitle)
	{
		m_pspPage.pszHeaderSubTitle = pszHeaderSubTitle;
		m_pspPage.dwFlags |= PSP_USEHEADERSUBTITLE;
	}
}

HPROPSHEETPAGE CWizardPage::Create(UINT nIndex, HINSTANCE hInstance, CWizardMain* pwmWizard)
{
	m_pspPage.hInstance = hInstance;
	m_hInstance = hInstance;
	m_pwmWizard = pwmWizard;
	m_nIndex = nIndex;

	if (m_nCaption != 0)
	{
		if (LoadString(hInstance, m_nCaption, m_szCaption, MAX_PATH) != 0)
		{
			m_pspPage.pszTitle = m_szCaption;
			m_pspPage.dwFlags |= PSP_USETITLE;
		}
	}

	m_hPage = CreatePropertySheetPage(&m_pspPage);
	return m_hPage;
}

void CWizardPage::SetStaticText(HWND hWnd, UINT nStaticTextID, UINT nResourceID, LPCSTR pszDefText, ...)
{
	char szText[MAX_LINE_LENGTH];
	char szFormat[MAX_LINE_LENGTH];
	va_list vargs;

	GetResStr(m_hInstance, nResourceID, szFormat, MAX_LINE_LENGTH, pszDefText);

	va_start(vargs, pszDefText);
	_vsnprintf(szText, MAX_LINE_LENGTH, szFormat, vargs);
	va_end(vargs);

	SetDlgItemText(hWnd, nStaticTextID, szText);
}

BOOL CALLBACK CWizardPage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWizardPage * pspPage = NULL;

	if (uMsg == WM_INITDIALOG)
	{
		pspPage = (CWizardPage*)((LPPROPSHEETPAGE)lParam)->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)pspPage);
	}
	else
	{
		pspPage = (CWizardPage*)GetWindowLong(hWnd, GWL_USERDATA);
	}
	if (pspPage)
	{
		Message message = {hWnd, uMsg, wParam, lParam, 0};

		pspPage->dialogProc(message);

		return message.lResult;
	}

	return 0; //DefDlgProc(hWnd, uMsg, wParam, lParam);
}

void CWizardPage::dialogProc(Message& message)
{
	switch (message.uMsg)
	{
		case WM_NOTIFY:
		{
			onNotify(message);
		}
		break;

		case WM_INITDIALOG:
		{
			onInitDialog(message);
		}
		break;

		case WM_CTLCOLORDLG:
		{
			onCtlColorDlg(message);
		}
		break;

		case WM_CTLCOLORSTATIC:
		{
			onCtlColorStatic(message);
		}
		break;

		case WM_COMMAND:
		{
			onCommand(message);
		}
		break;

		default:
		{
			message.lResult = 0;
		}
	}
}

void CWizardPage::onInitDialog(Message& message)
{
#ifdef IDC_TITLE
	if ((m_dwFlags & WZ_FIRSTPAGE) || (m_dwFlags & WZ_LASTPAGE))
	{
		if (m_hTitleFont == NULL)
		{
			LOGFONT lf;
			HDC hdc = GetDC(NULL);

			SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, &lf, 0);
			lf.lfWeight = FW_BOLD;
			lf.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * 12 / 72;

			m_hTitleFont = LocalCreateFontIndirect(&lf);

			ReleaseDC(NULL, hdc);
		}
		if (m_hTitleFont)
		{
			HWND hTitle = GetDlgItem(message.hWnd, IDC_TITLE);
			if (hTitle)
			{
				SetWindowFont(hTitle, m_hTitleFont, TRUE);
			}
		}
	}
#endif
}

void CWizardPage::onCtlColorDlg(Message& message)
{
	/*
	HDC hDC = (HDC)message.wParam;

	SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
	SetBkColor(hDC, GetSysColor(COLOR_WINDOW));

	message.lResult = (BOOL)GetSysColorBrush(COLOR_WINDOW);
	*/
}

void CWizardPage::onCtlColorStatic(Message& message)
{
	//message.lResult = (BOOL)GetSysColorBrush(COLOR_WINDOW);
}

void CWizardPage::onCommand(Message& message)
{}

void CWizardPage::onNotify(Message& message)
{
	LPNMHDR lpnm = (LPNMHDR) message.lParam;
	HWND hParent = GetParent(message.hWnd);

	switch (lpnm->code)
	{
		case PSN_SETACTIVE:   //Enable the correct buttons on for the active page
		{
			onWizardSetActive(message);
		}
		break;

		case PSN_WIZBACK:
		{
			onWizardBack(message);
			SetWindowLong(hParent, DWL_MSGRESULT, message.lResult);
			message.lResult = TRUE;
		}
		break;

		case PSN_WIZNEXT:
		{
			onWizardNext(message);
			SetWindowLong(hParent, DWL_MSGRESULT, message.lResult);
			message.lResult = TRUE;
		}
		break;

		case PSN_WIZFINISH:
		{
			onWizardFinish(message);
		}
		break;

		case PSN_RESET:
		{
			onWizardReset(message);
		}
		break;

		default:
		break;
	}
}

void CWizardPage::onWizardSetActive(Message& message)
{
	if ((m_dwFlags & WZ_FIRSTPAGE) && (m_dwFlags & WZ_LASTPAGE)) // Only one page, only enable finish
	{
		PropSheet_SetWizButtons(GetParent(message.hWnd), PSWIZB_FINISH);
	}
	else if (m_dwFlags & WZ_FIRSTPAGE) // First page, enable next
	{
		PropSheet_SetWizButtons(GetParent(message.hWnd), PSWIZB_NEXT);
	}
	else if (m_dwFlags & WZ_LASTPAGE) // Last page, enable back and finish
	{
		PropSheet_SetWizButtons(GetParent(message.hWnd), PSWIZB_BACK | PSWIZB_FINISH);
	}
	else // All others, allow next and back
	{
		PropSheet_SetWizButtons(GetParent(message.hWnd), PSWIZB_NEXT | PSWIZB_BACK);
	}
}

void CWizardPage::onWizardNext(Message& message)
{
	HWND hParent = GetParent(message.hWnd);
	int nCurrentPage = PropSheet_HwndToIndex(hParent, message.hWnd);
	m_pwmWizard->set_LastPage(nCurrentPage);
	message.lResult = PropSheet_IndexToId(hParent, nCurrentPage + 1);
}

void CWizardPage::onWizardBack(Message& message)
{
	HWND hParent = GetParent(message.hWnd);
	message.lResult = PropSheet_IndexToId(hParent, m_pwmWizard->get_LastPage());
}

void CWizardPage::onWizardFinish(Message& message)
{}

void CWizardPage::onWizardReset(Message& message)
{}
