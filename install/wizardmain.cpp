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
#include "wizardmain.h"

struct PROPERTYPAGE_ITEM
{
	PROPERTYPAGE_ITEM *pNext;
	CWizardPage* pPage;
	UINT nTemplate;

	PROPERTYPAGE_ITEM()
	{
		pPage = NULL;
		pNext = NULL;
	}
};

CWizardMain::CWizardMain(HINSTANCE hInstance)
{
	_Create(hInstance, NULL, NULL);
}

CWizardMain::CWizardMain(HINSTANCE hInstance, HBITMAP hbmWaterMark, HBITMAP hbmHeader)
{
	_Create(hInstance, hbmWaterMark, hbmHeader);
}

void CWizardMain::_Create(HINSTANCE hInstance, HBITMAP hbmWaterMark, HBITMAP hbmHeader)
{
	m_pPageTail = NULL;
	m_pPageHead = NULL;

	m_hInstance = hInstance;
	m_nPages = 0;
	m_nLastPage = 0;

	ZeroMemory(&m_pshHeader, sizeof(m_pshHeader));
    m_pshHeader.dwSize = sizeof(m_pshHeader);

	m_pshHeader.hInstance = hInstance;
	m_pshHeader.hwndParent = NULL;
	m_pshHeader.dwFlags = PSH_WIZARD97 | PSH_WATERMARK | PSH_HEADER; //|PSH_USECALLBACK;
	m_pshHeader.nStartPage = 0;

	if (hbmWaterMark)
	{
		m_pshHeader.hbmWatermark = hbmWaterMark;
		m_pshHeader.dwFlags |= PSH_USEHBMWATERMARK;
	}
#ifdef IDB_WATERMARK
	else
	{
		m_pshHeader.pszbmWatermark = MAKEINTRESOURCE(IDB_WATERMARK);
	}
#endif
	if (hbmHeader)
	{
		m_pshHeader.hbmHeader = hbmHeader;
		m_pshHeader.dwFlags |= PSH_USEHBMHEADER;
	}
#ifdef IDB_HEADER
	else
	{
		m_pshHeader.pszbmHeader = MAKEINTRESOURCE(IDB_HEADER);
	}
#endif
}

CWizardMain::~CWizardMain()
{
	while (m_pPageHead != NULL)
	{
		m_pPageTail = m_pPageHead->pNext;

		m_pPageHead->pPage = NULL;
		m_pPageHead->pNext = NULL;
		delete m_pPageHead;
		m_pPageHead = m_pPageTail;
	}
}

void CWizardMain::AddPage(CWizardPage* wpPage)
{
	PROPERTYPAGE_ITEM * pppi;

	if (wpPage != NULL)
	{
		pppi = new PROPERTYPAGE_ITEM;
		if (pppi)
		{
			m_nPages++;
			pppi->pPage = wpPage;
			if (m_pPageHead != NULL)
			{
				m_pPageTail->pNext = pppi;
			}
			else
			{
				m_pPageHead = pppi;
			}
			m_pPageTail = pppi;
		}
	}
}

int CWizardMain::RunWizard()
{
	PROPERTYPAGE_ITEM * pNextItem = m_pPageHead;
	HPROPSHEETPAGE *ahpsp;
	UINT nCount = 0;
	UINT nReturn = -1;

	if (m_nPages > 0)
	{
		ahpsp = new HPROPSHEETPAGE[m_nPages];

		if (ahpsp)
		{
			memset(ahpsp, 0, sizeof(ahpsp));
			while (pNextItem != NULL)
			{
				// First and last pages dont have the header
				if (nCount == 0)
				{
					pNextItem->pPage->RemoveFlag(PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE);
					pNextItem->pPage->AddFlag(PSP_HIDEHEADER);
					pNextItem->pPage->set_Flag(WZ_FIRSTPAGE);
				}
				else if (nCount == (m_nPages - 1))
				{
					pNextItem->pPage->RemoveFlag(PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE);
					pNextItem->pPage->AddFlag(PSP_HIDEHEADER);
					pNextItem->pPage->set_Flag(WZ_LASTPAGE);
				}

				ahpsp[nCount] = pNextItem->pPage->Create(nCount, m_hInstance, this);
				nCount++;
				pNextItem = pNextItem->pNext;
			}

			m_pshHeader.phpage = ahpsp;
			m_pshHeader.nPages = m_nPages;

			nReturn = PropertySheet(&m_pshHeader);

			delete [] ahpsp;

			return nReturn;
		}
	}

	return nReturn;
}
