//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This is a part of the Litestep Shell source code.
//
// Copyright (C) 1997-2007  Litestep Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define STRSAFE_NO_DEPRECATE
#include "lswinbase.h"

const unsigned int GWL_CLASSPOINTER = 0;


struct CreationData
{
	SHORT cbExtra;
	Window* window;
};

typedef UNALIGNED CreationData UACreationData;


const char defaultClassName[] = "AutoWindowClass";


LPCSTR Window::className = defaultClassName;
WNDCLASSEX Window::windowClass;
DWORD Window::instanceCount = 0;
HINSTANCE Window::hInstance = 0;


//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
Window::Window(LPCSTR szClassName):
		hWnd(NULL),
		hParent(NULL)
{
	WNDCLASSEX & wc = windowClass;

	if (!hInstance)
		MessageBox(NULL, "ERROR: Please call Window::init()", szClassName,
		           MB_ICONINFORMATION | MB_TOPMOST);

	instanceCount++;
	if (instanceCount > 1)
		return ;

	if (className != defaultClassName)
	{
		delete[] const_cast<LPSTR>(className);
		className = defaultClassName;
	}

	if (szClassName)
	{
		className = new char[strlen(szClassName) + 1];
		strcpy((LPSTR)className, szClassName);
	}

	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = sizeof(Window*);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = Window::wndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;

	if (!RegisterClassEx(&wc))
	{
		// Class could not be registered, try to re-register
		UnregisterClass(className, hInstance);

		if (!RegisterClassEx(&wc))
		{
			// Still no luck, error out
			MessageBox(NULL, "Unable to register window class.", className,
			           MB_ICONEXCLAMATION | MB_TOPMOST);
			throw;
		}
	}
}


//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
Window::~Window()
{
	destroyWindow();
	if (instanceCount > 0)
		instanceCount--;
	if (instanceCount > 0)
		return ;

	UnregisterClass(className, hInstance);
	if (className != defaultClassName)
		delete[] const_cast<LPSTR>(className);
}


//---------------------------------------------------------
// Returns the handle to the class' window
//---------------------------------------------------------
const HWND Window::handle() const
{
	return hWnd;
}


//---------------------------------------------------------
// Creates a new window and links it with the class
//---------------------------------------------------------
bool Window::createWindow(DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
                          int x, int y, int nWidth, int nHeight, HWND hWndParent)
{
	UACreationData creationData = {sizeof(UACreationData), this};

	if (hWnd && !destroyWindow())
	{
		MessageBox(NULL, "Unable to destroy existing window.", className,
		           MB_ICONEXCLAMATION | MB_TOPMOST);
		throw;
	}

	hParent = hWndParent;

	hWnd = CreateWindowEx(dwExStyle, className, lpWindowName, dwStyle,
	                                        x, y, nWidth, nHeight, hParent, NULL, hInstance, &creationData);
	if (!hWnd)
		return false;

	return true;
}



//---------------------------------------------------------
// Destroys the window associated with the class
//---------------------------------------------------------
bool Window::destroyWindow()
{
	if (hWnd && DestroyWindow(hWnd))
		return true;
	return false;
}


//---------------------------------------------------------
// If the window is associated with a class, call the
// messsage handler of the class
//---------------------------------------------------------
LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Message message = { 0 };
	message.uMsg = uMsg;
	message.wParam = wParam;
	message.lParam = lParam;
	
	Window* window = NULL;

	if (uMsg == WM_CREATE)
	{
		LPVOID & lpCreateParams = LPCREATESTRUCT(lParam)->lpCreateParams;

		if (lpCreateParams)
		{
			window = ((UACreationData*)(lpCreateParams))->window;
			SetWindowLong(hWnd, GWL_CLASSPOINTER, (LONG)window);
		}
	}
	else
		window = (Window*)(GetWindowLong(hWnd, GWL_CLASSPOINTER));

	if (window)
	{
		if (uMsg == WM_CREATE)
			window->hWnd = hWnd;
		window->windowProc(message);
		if (uMsg == WM_NCDESTROY)
			window->hWnd = NULL;
		return message.lResult;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//---------------------------------------------------------
// Empty skeleton for messsage handling
//---------------------------------------------------------
void Window::windowProc(Message& message)
{
	message.lResult = DefWindowProc(hWnd, message.uMsg, message.wParam, message.lParam);
}


void Window::init(HINSTANCE hInst)
{
	hInstance = hInst;
}

HINSTANCE Window::instance()
{
	return hInstance;
}
