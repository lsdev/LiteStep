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
// NOTES // NOTES // NOTES // NOTES // NOTES // NOTES // NOTES // NOTES //
/****************************************************************************
Note 1:  The offline file sync is sent only with a valid uMessageCallback 
         (NIF_MESSAGE) (W2K)
 
Note 2:  The DUN/Networking icons sent using the NIS_HIDDEN dwState flags use 
         an invalid uID (usually 0xFFFFFFFF) (W2K/XP)
 
Note 3:  The window handling the DUN/Networking icons has a class and caption of
         "Connections Tray" (FindWindowEx(NULL, NULL, "Connections Tray, NULL))
		 (W2K/XP)
 
Note 4:  Tooltip (NIF_TIP) for the DUN/Networking icons isn't created until the tray 
         icon receives its first mouseover event (W2K/XP)
 
Note 5:  The DUN/Networking Icons appear to use NIS_SHAREDICON with two different
         uIDs. This will cause two icons to display with the same functionality. 
		 One icon will not show a tooltip. Therefore as a work around, find the 
		 "Connections Tray" window, and everytime a notification comes in matching
		 that hwnd and without a NIF_TIP flag, post a WM_MOUSEMOVE message with
		 the uID and uCallbackMessage to the "Connections Tray" window. This will
		 cause a tooltip to be added to the true icon next time it is sent. The other
		 icon will remain with out a tooltip and can be ignored. (XP)
 
Note 6:  RegisterWindowMessage("TaskbarCreated") should be sent with HWND_BROADCAST
         before the shell service objects are loaded (sending after wards would 
		 result in a Volume icon flicker or loss because the shell service object
		 would delete the icon) Also ensure the "TaskbarCreated" message is sent only
		 once. (W2K/XP)
 
Note 7:  Volume/DUN/Networking icons are controlled by shell service objects (SSO).
         They are COM servers implementing the IOleCommandTarget interface. 
		 The CLSIDs are listed in 
		 "Software\\Microsoft\\Windows\\CurrentVersion\\ShellServiceObjectDelayLoad" 
         They should be enumerated and created using IID_IOleCOmmandTarget. 
		 Exec each SSO with a group id of CGID_ShellServiceObject and command id 
		 of 2 to start. Exec each SSO with a group id of CGID_ShellServiceObject 
		 and a command id of 3 to stop.
 
****************************************************************************/
#define _WIN32_IE 0x0500 // Ensure we get the new defines.
#include "TrayService.h"
#include "../lsapi/lsapi.h"
#define STRSAFE_NO_DEPRECATE
#include "../utility/safestr.h"


//
// TrayService
//
TrayService::TrayService()
{
	m_hInstance = NULL;
	m_hTrayWnd = NULL;
	m_hLiteStep = NULL;
	m_hConnectionsTray = NULL;

	m_pFirst = NULL;
	m_pLast = NULL;

	m_ssoList = NULL;

	m_bWin2000 = FALSE;
}


//
// ~TrayService
//
TrayService::~TrayService()
{}


//
// Start()
//
HRESULT TrayService::Start()
{
	HRESULT hr = E_FAIL;

	m_hLiteStep = GetLitestepWnd();
	m_hInstance = GetModuleHandle(NULL);

	if (m_hLiteStep && m_hInstance)
	{
		OSVERSIONINFO OsVersionInfo;
		memset(&OsVersionInfo, 0, sizeof(OSVERSIONINFO));
		OsVersionInfo.dwOSVersionInfoSize = sizeof(OsVersionInfo);

		GetVersionEx(&OsVersionInfo);
		if ((OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) && (OsVersionInfo.dwMajorVersion == 5))
		{
			m_bWin2000 = true;
		}
		else
		{
			m_bWin2000 = false;
		}

		WNDCLASSEX wc;

		// Register tray notification window class
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = 4;
		wc.cbClsExtra = 0;

		wc.lpfnWndProc = TrayService::WndProcTray;	// our window procedure
		wc.hInstance = m_hInstance;					// hInstance of DLL
		wc.lpszClassName = TRAYSERVICE_CLASS;		// our window class name
		wc.style = CS_DBLCLKS;

		if (RegisterClassEx(&wc))
		{
			// Tray Manager Window
			m_hTrayWnd = CreateWindowEx(
			                 WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
			                 TRAYSERVICE_CLASS,
			                 TRAYSERVICE_TITLE,
			                 WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			                 0, 0,
			                 0, 0,
			                 NULL,
			                 NULL,
			                 m_hInstance,
			                 NULL);
			if (m_hTrayWnd)
			{
				SetWindowLong(m_hTrayWnd, GWL_USERDATA, magicDWord);
				SetWindowLong(m_hTrayWnd, 0, (LONG)this);

				// Win98/IE4 stuff
				PostMessage(HWND_BROADCAST, RegisterWindowMessage("TaskbarCreated"), 0, 0);

				if (m_bWin2000)
				{
					_LoadShellServiceObjects();

					m_hConnectionsTray = FindWindowEx(NULL, NULL, "Connections Tray", NULL);
				}
				hr = S_OK;
			}
			else
			{
				UnregisterClass(TRAYSERVICE_CLASS, m_hInstance);
				MessageBox(m_hLiteStep, "Error creating window", TRAYSERVICE_TITLE, MB_OK | MB_TOPMOST);
			}
		}
		else
		{
			MessageBox(m_hLiteStep, "Error registering window class", TRAYSERVICE_CLASS, MB_OK | MB_TOPMOST);
		}
	}

	return hr;
}


//
// Stop()
//
HRESULT TrayService::Stop()
{
	HRESULT hr = S_OK;
	PSYSTRAYICONDATA pnidFound, pnidNext = NULL;

	if (m_bWin2000)
	{
		_UnloadShellServiceObjects();
	}

	if (m_hTrayWnd)
	{
		DestroyWindow(m_hTrayWnd); // delete our window

		UnregisterClass(TRAYSERVICE_CLASS, m_hInstance); // unregister window class
	}

	pnidFound = m_pFirst;
	while (pnidFound)
	{
		pnidNext = pnidFound->pNext;
		if (pnidFound)
		{
			// if it's first or last, move those pointers
			if (m_pFirst == pnidFound)
				m_pFirst = pnidFound->pNext;
			else
				pnidFound->pPrev->pNext = pnidFound->pNext;

			if (m_pLast == pnidFound)
				m_pLast = pnidFound->pPrev;
			else
				pnidFound->pNext->pPrev = pnidFound->pPrev;

			if (pnidFound->nid.hIcon)
			{
				DestroyIcon(pnidFound->nid.hIcon);
			}
			delete pnidFound;
		}
		pnidFound = pnidNext;
	}

	return hr;
}


STDMETHODIMP TrayService::get_hWnd(HWND* phWnd)
{
	HRESULT hr;

	if (phWnd != NULL)
	{
		*phWnd = m_hTrayWnd;
		if (m_hTrayWnd)
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}


//
// WndProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//
LRESULT CALLBACK TrayService::WndProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TrayService * pTrayService;
	LRESULT lReturn = 0;

	if (!pTrayService)
	{
		pTrayService = (TrayService*)GetWindowLong(hWnd, 0);
	}

	if (pTrayService)
	{
		lReturn = pTrayService->WindowProcTray(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lReturn;
}


//
// WindowProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//
LRESULT TrayService::WindowProcTray(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COPYDATA:
		{
			PSYSTRAYICONDATA pnidFound = m_pFirst;
			PCOPYDATASTRUCT pcds;
			PSHELLTRAYDATA pstd;
			BOOL bNewIcon = TRUE;
			BOOL result = TRUE;

			// Get the WM_COPYDATA structure from the lParam
			pcds = (PCOPYDATASTRUCT) lParam;

			if (pcds->dwData != 1)
				return FALSE;

			// Get the shell tray data from the WM_COPYDATA structure
			pstd = (PSHELLTRAYDATA)pcds->lpData;

			// New actions for W2K and after, we don't handle these right now
			// so we will just bail out if we receive them
			if ((pstd->dwMessage == NIM_SETFOCUS) || (pstd->dwMessage == NIM_SETVERSION))
				return FALSE;

			// See if the NOTIFYICONDATA already exists in our list
			bNewIcon = !SUCCEEDED(_FindItem(pstd->nid.w9X.hWnd, pstd->nid.w9X.uID, &pnidFound));

			if (pstd->dwMessage != NIM_DELETE)
			{
				if (pnidFound == NULL)
				{
					pnidFound = new SYSTRAYICONDATA;
					if (pnidFound != NULL)
					{
						memset(pnidFound, 0, sizeof(SYSTRAYICONDATA));
						pnidFound->nid.cbSize = sizeof(LSNOTIFYICONDATA);
						pnidFound->bHidden = FALSE;
						pnidFound->nid.hWnd = pstd->nid.w9X.hWnd;
						pnidFound->nid.uID	= pstd->nid.w9X.uID;
					}
					else
					{
						return FALSE;
					}
				}

				// Setup version specific attributes
				switch (pstd->nid.w9X.cbSize)
				{
					case NOTIFYICONDATAV4A_cbSize:
					case NOTIFYICONDATAV4W_cbSize:
					break;

					case NOTIFYICONDATAV5A_cbSize:
					{
						if (pstd->nid.wME.uFlags & NIF_STATE)
						{
							if (pstd->nid.wME.dwStateMask & NIS_HIDDEN)
							{
								if (pstd->nid.wME.dwState & NIS_HIDDEN)
								{
									pnidFound->nid.dwState |= NIS_HIDDEN;
									pnidFound->nid.dwStateMask |= NIS_HIDDEN;
									pnidFound->bHidden = TRUE;
								}
								else
								{
									pnidFound->nid.dwState &= ~NIS_HIDDEN;
									pnidFound->nid.dwStateMask &= ~NIS_HIDDEN;
									pnidFound->bHidden = FALSE;
								}
							}
						}
					}
					break;

					case NOTIFYICONDATAV6W_cbSize:
					{
						if (pstd->nid.w2K.uFlags & NIF_STATE)
						{
							if (pstd->nid.w2K.dwStateMask & NIS_SHAREDICON)
							{
								if (pstd->nid.w2K.dwState & NIS_SHAREDICON)
								{
									if ((pstd->nid.w2K.uFlags & NIF_ICON) &&
									        (pstd->nid.w2K.hIcon) && (pstd->dwMessage == NIM_ADD))
									{
										pnidFound->bHidden = TRUE;
									}
								}
							}
						}
					}

					case NOTIFYICONDATAV5W_cbSize:
					{
						if (pstd->nid.w2K.uFlags & NIF_STATE)
						{
							if (pstd->nid.w2K.dwStateMask & NIS_HIDDEN)
							{
								if (pstd->nid.w2K.dwState & NIS_HIDDEN)
								{
									pnidFound->nid.dwState |= NIS_HIDDEN;
									pnidFound->nid.dwStateMask |= NIS_HIDDEN;
									pnidFound->bHidden = TRUE;
								}
								else
								{
									pnidFound->nid.dwState &= ~NIS_HIDDEN;
									pnidFound->nid.dwStateMask &= ~NIS_HIDDEN;
									pnidFound->bHidden = FALSE;
								}
							}
						}
					}
					break;
				}
			}

			switch (pstd->dwMessage)
			{
				case NIM_ADD:
				case NIM_MODIFY:
				{
					// Setup version specific szTip/szInfo
					switch (pstd->nid.w9X.cbSize)
					{
						case NOTIFYICONDATAV4A_cbSize:
						{
							if ((pstd->nid.w9X.uFlags & NIF_TIP) && (pstd->nid.w9X.szTip))
							{
								StringCchCopy(pnidFound->nid.szTip, 64, pstd->nid.w9X.szTip);
								pnidFound->nid.szTip[63] = '\0';
							}
						}
						break;

						case NOTIFYICONDATAV4W_cbSize:
						{
							if ((pstd->nid.wNT.uFlags & NIF_TIP) && (pstd->nid.wNT.szTip))
							{
								UINT nChars = min(UINT(wcslen(pstd->nid.wNT.szTip) + 1), UINT(64));
								::WideCharToMultiByte(CP_ACP, 0, pstd->nid.wNT.szTip, nChars, pnidFound->nid.szTip, nChars, NULL, NULL);

								// just to ensure last character is a NULL... :P
								pnidFound->nid.szTip[63] = '\0';
							}
						}
						break;

						case NOTIFYICONDATAV5A_cbSize:
						{
							if ((pstd->nid.wME.uFlags & NIF_INFO) && (pstd->nid.wME.szInfo))
							{
								StringCchCopy(pnidFound->nid.szTip, 256, pstd->nid.wME.szInfo);

								// just to ensure last character is a NULL... :P
								pnidFound->nid.szTip[255] = '\0';

								// Add the NIF_TIP flag so modules will know to use this
								pstd->nid.wME.uFlags |= NIF_TIP;
							}
							else if ((pstd->nid.wME.uFlags & NIF_TIP) && (pstd->nid.wME.szTip))
							{
								StringCchCopy(pnidFound->nid.szTip, 128, pstd->nid.wME.szTip);

								// just to ensure last character is a NULL... :P
								pnidFound->nid.szTip[127] = '\0';
							}
						}
						break;

						default:  // case NOTIFYICONDATAV5W_cbSize: & XP V6
						{
							if ((pstd->nid.w2K.uFlags & NIF_INFO) && (pstd->nid.w2K.szInfo))
							{
								UINT nChars = min(UINT(wcslen(pstd->nid.w2K.szInfo) + 1), UINT(256));
								::WideCharToMultiByte(CP_ACP, 0, pstd->nid.w2K.szInfo, nChars, pnidFound->nid.szTip, nChars, NULL, NULL);

								// just to ensure last character is a NULL... :P
								pnidFound->nid.szTip[255] = '\0';

								// Add the NIF_TIP flag so modules will know to use this
								pstd->nid.w2K.uFlags |= NIF_TIP;
							}
							else if ((pstd->nid.w2K.uFlags & NIF_TIP) && (pstd->nid.w2K.szTip))
							{
								UINT nChars = min(UINT(wcslen(pstd->nid.w2K.szTip) + 1), UINT(128));
								::WideCharToMultiByte(CP_ACP, 0, pstd->nid.w2K.szTip, nChars, pnidFound->nid.szTip, nChars, NULL, NULL);

								// just to ensure last character is a NULL... :P
								pnidFound->nid.szTip[127] = '\0';
							}
						}
						break;
					}

					DWORD dwFlags = pnidFound->nid.uFlags | pstd->nid.w9X.uFlags;
					pnidFound->nid.uFlags = pstd->nid.w9X.uFlags;

					if (pnidFound->nid.uFlags & NIF_MESSAGE)
					{
						pnidFound->nid.uCallbackMessage = pstd->nid.w9X.uCallbackMessage;
					}

					if (pnidFound->nid.uFlags & NIF_ICON)
					{
						if (pnidFound->nid.hIcon)
						{
							DestroyIcon(pnidFound->nid.hIcon);
						}
						pnidFound->nid.hIcon = CopyIcon(pstd->nid.w9X.hIcon); // Note 8
					}

					if (bNewIcon)
					{
						// add to last position in the list
						if (!m_pFirst)
						{
							m_pFirst = pnidFound;
							pnidFound->pPrev = NULL;
						}
						if (m_pLast)
						{
							pnidFound->pPrev = m_pLast;
							m_pLast->pNext = pnidFound;
						}
						m_pLast = pnidFound;
						pnidFound->pNext = NULL;

						if (pnidFound->bHidden == FALSE)
						{
							result = SendMessage(m_hLiteStep, LM_SYSTRAY, NIM_ADD, (LPARAM) & pnidFound->nid);
						}
					}
					else
					{
						if (pnidFound->bHidden == FALSE)
						{
							result = SendMessage(m_hLiteStep, LM_SYSTRAY, NIM_MODIFY, (LPARAM) & pnidFound->nid);
						}
					}

					// Update the flags, so that if we have to resend this, it's
					// up to date
					pnidFound->nid.uFlags = dwFlags;
				}
				break;

				case NIM_DELETE:
				{
					if ((pnidFound) && !(bNewIcon))
					{
						if (pnidFound->bHidden == FALSE)
						{
							// if it's first or last, move those pointers
							if (m_pFirst == pnidFound)
								m_pFirst = pnidFound->pNext;
							else
								pnidFound->pPrev->pNext = pnidFound->pNext;

							if (m_pLast == pnidFound)
								m_pLast = pnidFound->pPrev;
							else
								pnidFound->pNext->pPrev = pnidFound->pPrev;
						}

						// remove the icon class
						result = SendMessage(m_hLiteStep, LM_SYSTRAY, NIM_DELETE, (LPARAM) & pnidFound->nid);

						if (pnidFound->bHidden == FALSE)
						{
							if (pnidFound->nid.hIcon)
							{
								DestroyIcon(pnidFound->nid.hIcon);
							}
							delete pnidFound;
							pnidFound = NULL;
						}
					}
					break;
				}
				default:
				{
					result = FALSE;
					break;
				}
			}

			return result;

		}
		break;

		case LM_SYSTRAYREADY:
		{
			_SendSystemTray();
			return TRUE;
		}
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//
// SendSystemTray
//
void TrayService::_SendSystemTray()
{
	PSYSTRAYICONDATA pnidFound = m_pFirst;

	// See if the NOTIFYICONDATA already exists in our list
	while (pnidFound)
	{
		if (pnidFound->bHidden == FALSE)
		{
			SendMessage(m_hLiteStep, LM_SYSTRAY, NIM_ADD, (LPARAM) & pnidFound->nid);
		}
		pnidFound = pnidFound->pNext;
	}
}


//
// _LoadShellServiceObjects
//
void TrayService::_LoadShellServiceObjects()
{
	HKEY hkeyServices;
	LONG lErrorCode;
	int i = 0;

	lErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
	                          "Software\\Microsoft\\Windows\\CurrentVersion\\ShellServiceObjectDelayLoad",
	                          0, KEY_READ, &hkeyServices);


	while (lErrorCode == ERROR_SUCCESS)
	{
		char szValueName[32];
		char szData[40];
		DWORD cbValueName = 32;
		DWORD cbData = 40;
		DWORD dwDataType;

		lErrorCode = RegEnumValue(hkeyServices, i,
		                          szValueName, &cbValueName, 0,
		                          &dwDataType,
		                          (LPBYTE) szData, &cbData);

		if (lErrorCode == ERROR_SUCCESS)
		{
			WCHAR wszCLSID[40];
			CLSID clsid;
			IOleCommandTarget *pCmdTarget = NULL;

			MultiByteToWideChar(CP_ACP, 0, szData, cbData, wszCLSID, 40);

			CLSIDFromString(wszCLSID, &clsid);

			HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
			                              IID_IOleCommandTarget, (void **) & pCmdTarget);

			if (SUCCEEDED(hr))
			{
				pCmdTarget->Exec(&CGID_ShellServiceObject, 2, 0, NULL, NULL);

				ShellServiceObjectList *entry = new ShellServiceObjectList;
				entry->object = pCmdTarget;
				entry->next = m_ssoList;
				m_ssoList = entry;
			}
		}

		++i;
	}

	RegCloseKey(hkeyServices);
}


//
// UnloadShellServiceObjects
//
void TrayService::_UnloadShellServiceObjects()
{
	ShellServiceObjectList * ssoCurrent = m_ssoList;
	ShellServiceObjectList *ssoNext;

	while (ssoCurrent)
	{
		ssoNext = ssoCurrent->next;

		ssoCurrent->object->Exec(&CGID_ShellServiceObject, 3, 0, NULL, NULL);
		ssoCurrent->object->Release();
		delete ssoCurrent;

		ssoCurrent = ssoNext;
	}

	m_ssoList = NULL;
}



HRESULT TrayService::_FindItem(const HWND hWnd, const UINT uID, SYSTRAYICONDATA** ppnid)
{
	HRESULT hr = E_FAIL;

	if ((*ppnid != NULL) && hWnd)
	{
		*ppnid = m_pFirst;
		while (*ppnid != NULL)
		{
			if (((*ppnid)->nid.hWnd == hWnd) && ((*ppnid)->nid.uID == uID))
			{
				hr = S_OK;
				break;
			}
			*ppnid = (*ppnid)->pNext;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	return hr;
}
