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
#include "litestep.h"
#include "../utility/safestr.h" // Always include last in cpp file

// const char rcsRevision[] = "$Revision: 1.1 $"; // Our Version
const char rcsId[] = "$Id: litestep.cpp,v 1.1 2002/09/23 02:43:24 message Exp $"; // The Full RCS ID.
const char LSRev[] = "0.24.7 ";

// Parse the command line
void ParseCmdLine(LPCSTR pszCmdLine);
void ExecuteCmdLineBang(LPCSTR pszCommand, LPCSTR pszArgs);

CLiteStep gLiteStep;
BOOL bRunStartup = TRUE;
CHAR szAppPath[MAX_PATH];
CHAR szRcPath[MAX_PATH];
HINSTANCE hLSInstance = NULL;

struct CreationData
{
	SHORT cbExtra;
	CLiteStep* pLiteStep;
};

typedef UNALIGNED CreationData UACreationData;

//
// ExecuteCmdLineBang
//
void ExecuteCmdLineBang(LPCSTR pszCommand, LPCSTR pszArgs)
{
	LMBANGCOMMAND * bcBangCommand = NULL;

	if (IsValidStringPtr(pszCommand))
	{
		HWND hWnd = FindWindow("TApplication", "Litestep");
		if ((hWnd != NULL) && IsWindow(hWnd))
		{
			bcBangCommand = new LMBANGCOMMAND;
			if (bcBangCommand != NULL)
			{
				bcBangCommand->cbSize = sizeof(LMBANGCOMMAND);
				bcBangCommand->hWnd = NULL;

				StringCchCopy(bcBangCommand->szCommand, MAX_BANGCOMMAND, pszCommand);

				bcBangCommand->szArgs[0] = '\0';
				if (IsValidStringPtr(pszArgs))
				{
					StringCchCopy(bcBangCommand->szArgs, MAX_BANGARGS, pszArgs);
				}

				COPYDATASTRUCT cds = {LM_BANGCOMMAND, sizeof(LMBANGCOMMAND), (LPVOID)bcBangCommand};

				SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) & cds);

				delete bcBangCommand;
			}
		}
	}

	exit(0);
}


//
// ParseCmdLine(LPCSTR pszCmdLine)
//
void ParseCmdLine(LPCSTR pszCmdLine)
{
	CHAR szToken[MAX_PATH];

	if (IsValidStringPtr(pszCmdLine))
	{
		LPCSTR pszNextToken = pszCmdLine;
		while (GetToken(pszNextToken, szToken, &pszNextToken, false))
		{
			switch (szToken[0])
			{
				case '-':
				{
					if (!stricmp(szToken, "-nostartup"))
					{
						bRunStartup = FALSE;
					}
					else if (!stricmp(szToken, "-install"))
					{
						HMODULE hInstall = LoadLibrary("install.dll");
						if (hInstall)
						{
							FARPROC (__stdcall * InstallShell)(LPCSTR) = NULL;
							InstallShell = (FARPROC (__stdcall *)(LPCSTR))GetProcAddress(hInstall, "InstallShell");

							if (InstallShell)
							{
								InstallShell(szAppPath);
							}

							InstallShell = NULL;
							FreeLibrary(hInstall);
							hInstall = NULL;

							exit(0);
						}
					}
					else if (!stricmp(szToken, "-uninstall"))
					{
						HMODULE hInstall = LoadLibrary("install.dll");
						if (hInstall)
						{
							FARPROC (__stdcall * UninstallShell)(VOID) = NULL;
							UninstallShell = (FARPROC (__stdcall *)(VOID))GetProcAddress(hInstall, "UninstallShell");

							if (UninstallShell)
							{
								UninstallShell();
							}

							UninstallShell = NULL;
							FreeLibrary(hInstall);
							hInstall = NULL;

							exit(0);
						}
					}
				}
				break;

				case '!':
				{
					ExecuteCmdLineBang(szToken, pszNextToken);
				}
				break;

				default:
				{
					if (PathFileExists(szToken))
					{
						if (strchr(szToken, '\\'))
						{
							StringCchCopy(szRcPath, MAX_PATH, szToken);
						}
						else
						{
							StringCchPrintfEx(szRcPath, MAX_PATH, NULL, NULL, STRSAFE_NULL_ON_FAILURE, "%s\\%s", szAppPath, szToken);
						}
					}
				}
				break;
			}
		}
	}
}


//
//
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr = S_OK;
	hLSInstance = hInstance;

	// Determine our application's path
	if (GetModuleFileName (hInstance, szAppPath, sizeof (szAppPath)) > 0)
	{
		PathRemoveFileSpec(szAppPath);
		PathAddBackslash(szAppPath);
	}
	StringCchPrintfEx(szRcPath, MAX_PATH, NULL, NULL, STRSAFE_NULL_ON_FAILURE, "%sstep.rc", szAppPath);

	// Parse command line, setting appropriate variables
	ParseCmdLine(lpCmdLine);

	// If we can't find "step.rc", there's no point in proceeding
	if (!PathFileExists(szRcPath))
	{
		RESOURCE_STREX(hInstance, IDS_LITESTEP_ERROR2, resourceTextBuffer, MAX_LINE_LENGTH,
		               "Unable to find the file \"%s\".\nPlease verify the location of the file, and try again.", szRcPath);
		MessageBox(NULL, resourceTextBuffer, "LiteStep", MB_TOPMOST | MB_ICONEXCLAMATION);
		exit(0);
	}

	// Check for previous instance
	CreateMutex(NULL, FALSE, "LiteStep");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Prevent multiple instances of LiteStep
		RESOURCE_STR(hInstance, IDS_LITESTEP_ERROR1,
		             "A previous instance of LiteStep was detected.\nAre you sure you want to continue?");
		if (IDNO == MessageBox(NULL, resourceTextBuffer, "LiteStep", MB_TOPMOST | MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2))
		{
			exit(0);
		}
	}

	hr = gLiteStep.Start(szAppPath, szRcPath, hInstance, bRunStartup);

	return (int)hr;
}


//
// CLiteStep()
//
CLiteStep::CLiteStep()
{
	m_szAppPath[0] = '\0';
	m_szRcPath[0] = '\0';
	m_hInstance = NULL;
	m_bAppIsFullScreen = FALSE;
	m_hMainWindow = NULL;
	WM_ShellHook = 0;
	m_pModuleManager = NULL;
	//gBangManager = NULL;
	m_pDataStoreManager = NULL;
	m_pMessageManager = NULL;
	bHookManagerStarted = FALSE;
	hTrayManager = NULL;
	m_cxServiceItems = 0;
	m_pDDEService = NULL;
	m_pTrayService = NULL;
}


//
//
//
CLiteStep::~CLiteStep()
{
}


//
// Start(LPCSTR pszAppPath, LPCSTR pszRcPath, HINSTANCE hInstance, BOOL bRunStartup)
//
HRESULT CLiteStep::Start(LPCSTR pszAppPath, LPCSTR pszRcPath, HINSTANCE hInstance, BOOL bRunStartup)
{
	ATOM aClass;
	WNDCLASSEX wc;
	HRESULT hr;
	BOOL bDoRunStartup;
	BOOL bUnderExplorer = FALSE;

	StringCchCopy(m_szAppPath, MAX_PATH, pszAppPath);
	StringCchCopy(m_szRcPath, MAX_PATH, pszRcPath);
	m_hInstance = hInstance;

	// Initialize OLE/COM
	OleInitialize(NULL);

	// before anything else, start the recovery menu thread
	DWORD dwThreadID;
	CloseHandle(CreateThread(NULL, 0, RecoveryThreadProc, NULL, 0, &dwThreadID));

	// Get undocumented APIs

	// configure the Win32 window manager to hide windows when they are minimized
	MINIMIZEDMETRICS mm;
	mm.cbSize = sizeof(MINIMIZEDMETRICS);

	SystemParametersInfo(SPI_GETMINIMIZEDMETRICS, mm.cbSize, &mm, 0);

	if (!(mm.iArrange & ARW_HIDE))
	{
		mm.iArrange |= ARW_HIDE;
		SystemParametersInfo(SPI_SETMINIMIZEDMETRICS, mm.cbSize, &mm, 0);
	}

	SetupSettingsManager(szAppPath, szRcPath);

	bDoRunStartup = ((bRunStartup) ? GetRCBool("LSNoStartup", FALSE) : FALSE);

	// Check for explorer
	if (FindWindow("Shell_TrayWnd", NULL)) // Running under Exploder
	{
		if (!(GetRCBool("LSNoShellWarning", TRUE)))
		{
			RESOURCE_STR(hInstance, IDS_LITESTEP_ERROR3,
			             "You are currently running another shell, while Litestep b24 allows you\012to run under Explorer, we don't advise it for inexperienced users, and we\012will not support it, so do so at your own risk.\012\012If you continue, some of the advanced features of Litestep will be disabled\012such as the desktop. The wharf, hotkeys, and shortcuts will still work.\012\012To get rid of this message next time, put LSNoShellWarning in your step.rc\012\012Continue?")
			RESOURCE_TITLE(hInstance, IDS_LITESTEP_TITLE_WARNING, "Warning")
			if (MessageBox(0, resourceTextBuffer, resourceTitleBuffer, MB_YESNO | MB_ICONEXCLAMATION | MB_TOPMOST) == IDNO)
			{
				return 0;
			}
		}
		bUnderExplorer = TRUE;
	}

	// Register Window Class
	memset (&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = sizeof(CLiteStep*);
	wc.lpfnWndProc = CLiteStep::InternalWndProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = szMainWindowClass;
	aClass = RegisterClassEx(&wc);
	if (!aClass)
	{
		RESOURCE_MSGBOX_T(hInstance, IDS_LITESTEP_ERROR4,
		                  "Error registering main Litestep window class.",
		                  IDS_LITESTEP_TITLE_ERROR, "Error")

		exit(0);
	}

	UACreationData cdCreationData = {sizeof(UACreationData), this};

	// Create our main window
	m_hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
	                               szMainWindowClass, szMainWindowTitle, 0,
	                               0, 0, 0, 0, NULL, NULL, m_hInstance, (LPVOID) & cdCreationData);

	// Start up everything
	if (m_hMainWindow)
	{
		MSG message;
		FARPROC (__stdcall * RegisterShellHook)(HWND, DWORD) = NULL;

		// Set magic DWORD to prevent VWM from seeing main window
		SetWindowLong (m_hMainWindow, GWL_USERDATA, magicDWord);

		RegisterShellHook = (FARPROC (__stdcall *)(HWND, DWORD))GetProcAddress(GetModuleHandle("SHELL32.DLL"), (LPCSTR)((long)0xB5));
		WM_ShellHook = RegisterWindowMessage("SHELLHOOK");
		if (RegisterShellHook)
		{
			RegisterShellHook(NULL, RSH_REGISTER);
			RegisterShellHook(m_hMainWindow, RSH_TASKMAN);
		}

		// Set Shell Window
		if (!bUnderExplorer && (GetRCBool("LSSetAsShell", TRUE)))
		{
			FARPROC (__stdcall * SetShellWindow)(HWND) = NULL;
			SetShellWindow = (FARPROC (__stdcall *)(HWND))GetProcAddress(GetModuleHandle("USER32.DLL"), "SetShellWindow");
			if (SetShellWindow)
			{
				SetShellWindow(m_hMainWindow);
			}
		}

		hr = _InitServices();
		if (SUCCEEDED(hr))
		{
			hr = _StartServices();
			// Quietly swallow service errors... in the future.. do something
		}

		// Run startup items if the SHIFT key is not down
		if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) && bDoRunStartup)
		{
			DWORD dwThread;
			CloseHandle(CreateThread(NULL, 0, StartupRunner::Run, NULL, 0, &dwThread));
			bRunStartup = FALSE;
		}

		hr = _InitManagers();
		if (SUCCEEDED(hr))
		{
			hr = _StartManagers();
			// Quietly swallow manager errors... in the future.. do something
		}

		// Undocumented call: Shell Loading Finished
		SendMessage(GetDesktopWindow(), WM_USER, 0, 0);

		// Main message pump
		while (GetMessage(&message, 0, 0, 0) > 0)
		{
			__try
			{
			    if (message.hwnd == NULL)
		    {
			    if (message.message == NULL)
				    {
					    //something's wacked, break out of this
					    break;
				    }
				    // Thread message
				    switch (message.message)
				    {
					    case LM_THREAD_BANGCOMMAND:
					    {
						    ThreadedBangCommand * pInfo = (ThreadedBangCommand*)message.wParam;

						    if (pInfo != NULL)
						    {
							    pInfo->Execute();
							    pInfo->Release(); //check BangCommand.cpp for the reason
						    }
					    }
					    break;
				    }
			    }
			    else
			    {
				    TranslateMessage(&message);
				    DispatchMessage (&message);
			    }
			}
			__except(1)
			{}
		}

		if (RegisterShellHook)
		{
			RegisterShellHook(m_hMainWindow, RSH_UNREGISTER);
		}

		_StopManagers();
		_CleanupManagers();

		_StopServices();
		_CleanupServices();

		// Destroy _main window
		DestroyWindow(m_hMainWindow);
		m_hMainWindow = NULL;
	}
	else
	{
		RESOURCE_MSGBOX_T(hInstance, IDS_LITESTEP_ERROR5,
		                  "Error creating Litestep main application window.",
		                  IDS_LITESTEP_TITLE_ERROR, "Error")
	}

	// Unreg class
	UnregisterClass(szMainWindowClass, m_hInstance);

	// deinitialie stepsettings
	DeleteSettingsManager();

	// Uninitialize OLE/COM
	OleUninitialize();

	// close the recovery thread
	DestroyWindow(FindWindow(szRecoveryMenuWndClass, NULL));

	return S_OK;
}


//
//
//
LRESULT CALLBACK CLiteStep::InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CLiteStep * pLiteStep = NULL;

	if (uMsg == WM_CREATE)
	{
		LPVOID & lpCreateParams = LPCREATESTRUCT(lParam)->lpCreateParams;

		if (lpCreateParams)
		{
			pLiteStep = ((UACreationData*)(lpCreateParams))->pLiteStep;
			SetWindowLong(hWnd, GWL_CLASSPOINTER, (LONG)pLiteStep);
		}
	}
	else if (pLiteStep == NULL)
	{
		pLiteStep = (CLiteStep*)(GetWindowLong(hWnd, GWL_CLASSPOINTER));
	}

	if (pLiteStep)
	{
		/*if (uMsg == WM_CREATE)
		{
			const_cast<HWND>(pLiteStep->m_hMainWindow) = hWnd;
		}*/

		return pLiteStep->ExternalWndProc(hWnd, uMsg, wParam, lParam);

		/*if (uMsg == WM_NCDESTROY)
		{
			const_cast<HWND>(pLiteStep->hWnd) = NULL;
		}*/ 
		//return lReturn;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//
//
//
LRESULT CLiteStep::ExternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lReturn = FALSE;

	switch (uMsg)
	{
		case WM_KEYDOWN:
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case LM_SHUTDOWN:
				case SC_CLOSE:
				{
					ParseBangCommand(hWnd, "!ShutDown", NULL);
				}
				break;

				default:
				{
					lReturn = DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
				break;
			}
		}
		break;

		case WM_QUERYENDSESSION:
		case WM_ENDSESSION:
		{
			lReturn = TRUE;
		}
		break;

		case LM_SYSTRAYREADY:
		{
			if (hTrayManager)
			{
				SendMessage(hTrayManager, uMsg, 0, 0);
				lReturn = TRUE;
			}
		}
		break;

		case LM_SAVEDATA:
		{
			WORD wIdent = HIWORD(wParam);
			WORD wLength = LOWORD(wParam);
			void *pvData = (void *)lParam;
			if ((pvData != NULL) && (wLength > 0))
			{
				if (m_pDataStoreManager == NULL)
				{
					m_pDataStoreManager = new DataStore();
				}
				if (m_pDataStoreManager)
				{
					lReturn = m_pDataStoreManager->StoreData(wIdent, pvData, wLength);
				}
			}
		}
		break;

		case LM_RESTOREDATA:
		{
			WORD wIdent = HIWORD(wParam);
			WORD wLength = LOWORD(wParam);
			void *pvData = (void *)lParam;
			if ((pvData != NULL) && (wLength > 0))
			{
				if (m_pDataStoreManager)
				{
					lReturn = m_pDataStoreManager->ReleaseData(wIdent, pvData, wLength);
					if (m_pDataStoreManager->Count() == 0)
					{
						delete m_pDataStoreManager;
						m_pDataStoreManager = NULL;
					}
				}
			}
		}
		break;

		case LM_GETLSOBJECT:
		case LM_WINDOWLIST:
		case LM_MESSAGEMANAGER:
		case LM_DATASTORE:
		{
			; // Obsolete Message, return 0
		}
		break;

		case LM_RECYCLE:
		{
			switch (wParam)
			{
				case LR_RECYCLE:
				{
					_Recycle();
				}
				break;

				case LR_LOGOFF:
				{
					if (ExitWindowsEx(EWX_LOGOFF, 0))
					{
						PostQuitMessage(0);
					}
				}
				break;

				case LR_QUIT:
				{
					PostQuitMessage(0);
				}
				break;

				default:  // wParam == LR_MSSHUTDOWN
				{
					FARPROC (__stdcall * MSWinShutdown)(HWND) = NULL;
					MSWinShutdown = (FARPROC (__stdcall *)(HWND))GetProcAddress(GetModuleHandle("SHELL32.DLL"), (LPSTR)((long)0x3C));
					if (MSWinShutdown)
					{
						MSWinShutdown(m_hMainWindow);
					}
				}
				break;
			}
		}
		break;

		case LM_UNLOADMODULE:
		{ // Module Handler Message

			LPCSTR pszPath = (LPCSTR)wParam;

			if (IsValidStringPtr(pszPath))
			{
				if (m_pModuleManager)
				{
					m_pModuleManager->QuitModule(pszPath);
				}
			}
		}
		break;

		case LM_RELOADMODULE:
		{ // Module Handler Message

			LPCSTR pszPath = (LPCSTR)wParam;

			if (IsValidStringPtr(pszPath))
			{
				if (m_pModuleManager)
				{
					m_pModuleManager->QuitModule(pszPath);
					m_pModuleManager->LoadModule(pszPath, 0);
				}
			}
		}
		break;

		case LM_BANGCOMMAND:
		{
			PLMBANGCOMMAND plmbc = (PLMBANGCOMMAND)lParam;

			if (IsValidReadPtr(plmbc))
			{
				if (plmbc->cbSize == sizeof(LMBANGCOMMAND))
				{
					lReturn = ParseBangCommand(plmbc->hWnd, plmbc->szCommand, plmbc->szArgs);
				}
			}
		}
		break;

		case WM_COPYDATA:
		{
			PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

			switch (pcds->dwData)
			{
				case LM_BANGCOMMAND:
				{
					lReturn = SendMessage(hWnd, LM_BANGCOMMAND, 0, (LPARAM)pcds->lpData);
				}
				break;
			}
		}
		break;

		case LM_GETREVID:
		{
			SAFE_CHAR(szBuffer, 256);

			if (wParam == 1)
			{
				StringCchCopy(szBuffer, 256, &rcsId[1]);
				szBuffer[strlen(szBuffer) - 1] = '\0';
			}
			else
			{
				StringCchCopy(szBuffer, 256, , "litestep.exe: ");
				StringCchCat(szBuffer, 256, (LPCSTR) & LSRev);
				szBuffer[strlen(szBuffer) - 1] = '\0';
			}
			SendMessage((HWND)lParam, LM_GETREVID, 0, (long)szBuffer);
			m_pMessageManager->GetRevID(LM_GETREVID, wParam, lParam);
		}
		break;

		case LM_REGISTERHOOKMESSAGE:
		{
			if (!bHookManagerStarted)
			{
				bHookManagerStarted = startHookManager(m_hInstance);
			}
			if (bHookManagerStarted)
			{
				lReturn = RegisterHookMessage(hWnd, wParam, (HookCallback*)lParam);
			}
		}
		break;

		case LM_UNREGISTERHOOKMESSAGE:
		{
			if (bHookManagerStarted)
			{
				if (UnregisterHookMessage(hWnd, wParam, (HookCallback*)lParam) == 0)
				{
					stopHookManager();
					bHookManagerStarted = FALSE;
				}
			}
		}
		break;

		case LM_REGISTERMESSAGE:     // Message Handler Message
		{
			if (m_pMessageManager)
			{
				m_pMessageManager->AddMessages((HWND)wParam, (UINT *)lParam);
			}
		}
		break;

		case LM_UNREGISTERMESSAGE:     // Message Handler Message
		{
			if (m_pMessageManager)
			{
				m_pMessageManager->RemoveMessages((HWND)wParam, (UINT *)lParam);
			}
		}
		break;

		default:
		{
			if (m_pMessageManager)
			{
				if (uMsg == WM_ShellHook)
				{
					HWND hWndMessage = (HWND)lParam;
					uMsg = (LOWORD(wParam) & 0x00FF) + 9500;
					lParam = (LOWORD(wParam) & 0xFF00);
					wParam = (WPARAM)hWndMessage;

					if (uMsg == LM_WINDOWACTIVATED)
					{
						if ((lParam > 0) && (m_bAppIsFullScreen == FALSE))
						{
							m_bAppIsFullScreen = TRUE;
							ParseBangCommand(m_hMainWindow, "!HIDEMODULES", NULL);
						}
						else if ((lParam <= 0) && (m_bAppIsFullScreen == TRUE))
						{
							m_bAppIsFullScreen = FALSE;
							ParseBangCommand(m_hMainWindow, "!SHOWMODULES", NULL);
						}
					}
				}

				if (m_pMessageManager->HandlerExists(uMsg))
				{
					lReturn = m_pMessageManager->SendMessage(uMsg, wParam, lParam);
					break;
				}
			}
			lReturn = DefWindowProc (hWnd, uMsg, wParam, lParam);
		}
		break;
	}

	return lReturn;
}


//
// _InitServies()
//
HRESULT CLiteStep::_InitServices()
{
	HRESULT hr = S_OK;

	IID iidNULL = {0};

	m_ServiceItems[0] = ServiceItem(Service<DDEService>::CreateInstance, (IService**) & m_pDDEService);
	m_ServiceItems[1] = ServiceItem(Service<TrayService>::CreateInstance, (IService**) & m_pTrayService);
	m_cxServiceItems = 2;

	for (int i = 0; ((i < m_cxServiceItems) && SUCCEEDED(hr)); i++)
	{
		hr = m_ServiceItems[i].pfnFactory(iidNULL, (void**)m_ServiceItems[i].ppService);
	}

	return hr;
}


//
// _StartServices()
//
HRESULT CLiteStep::_StartServices()
{
	HRESULT hr = S_OK;

	for (int i = 0; ((i < m_cxServiceItems) && SUCCEEDED(hr)); i++)
	{
		if (*m_ServiceItems[i].ppService)
		{
			hr = (*m_ServiceItems[i].ppService)->Start();
		}
	}

	m_pTrayService->get_hWnd(&hTrayManager);

	return hr;
}


//
// _StopServices()
//
HRESULT CLiteStep::_StopServices()
{
	HRESULT hr = S_OK;

	for (int i = m_cxServiceItems - 1; i >= 0 ; i--)
	{
		if (*m_ServiceItems[i].ppService)
		{
			(*m_ServiceItems[i].ppService)->Stop();
		}
	}

	return hr;
}


//
// _CleanupServices()
//
void CLiteStep::_CleanupServices()
{
	for (int i = 0; i < m_cxServiceItems; i++)
	{
		if (*m_ServiceItems[i].ppService)
		{
			(*m_ServiceItems[i].ppService)->Release();
		}
	}
}


//
// _InitManagers()
//
HRESULT CLiteStep::_InitManagers()
{
	HRESULT hr = S_OK;

	//gDataStore = new DataStore();

	m_pMessageManager = new MessageManager();

	m_pModuleManager = new ModuleManager(m_hMainWindow, m_szAppPath);

	//gBangManager = new BangManager();

	return hr;
}


//
// _StartManagers
//
HRESULT CLiteStep::_StartManagers()
{
	HRESULT hr = S_OK;

	//SetBangManager(gBangManager);

	// Setup bang commands in lsapi
	setupBangs();

	// Load modules
	m_pModuleManager->LoadModules();

	return hr;
}


//
// _StopManagers()
//
HRESULT CLiteStep::_StopManagers()
{
	HRESULT hr = S_OK;

	if (bHookManagerStarted)
	{
		stopHookManager();
		bHookManagerStarted = FALSE;
	}

	m_pModuleManager->QuitModules();
	//gBangManager->ClearBangCommands();
	m_pMessageManager->ClearMessages();

	//ClearBangManager();

	return hr;
}


//
// _CleanupManagers
//
void CLiteStep::_CleanupManagers()
{
	if (m_pModuleManager)
	{
		delete m_pModuleManager;
		m_pModuleManager = NULL;
	}

	/*if (gBangManager)
	{
		delete gBangManager;
		gBangManager = NULL;
	}*/

	if (m_pMessageManager)
	{
		delete m_pMessageManager;
		m_pMessageManager = NULL;
	}

	if (m_pDataStoreManager)
	{
		m_pDataStoreManager->Clear();
		delete m_pDataStoreManager;
		m_pDataStoreManager = NULL;
	}
}


//
// _Recycle
//
void CLiteStep::_Recycle()
{
	_StopManagers();

	DeleteSettingsManager();

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		RESOURCE_MSGBOX(m_hInstance, IDS_LITESTEP_ERROR6,
		                "Recycle has been paused, click OK to continue.", "LiteStep")
	}

	SetupSettingsManager(szAppPath, szRcPath);

	_StartManagers();
}




















