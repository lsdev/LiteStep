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
#ifndef __LSWINBASE_H
#define __LSWINBASE_H

#include "common.h"

#define BEGIN_MESSAGEPROC switch (message.uMsg) {
#define MESSAGE(handler, msg) case msg: handler(message); break;
#define REJECT_MESSAGE(msg) case msg: break;
#define END_MESSAGEPROC default: message.lResult = DefWindowProc(hWnd, message.uMsg, message.wParam, message.lParam); }

struct Message
{
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


class Window
{
protected:
	static WNDCLASSEX windowClass;
	static LPCSTR className;
	static DWORD instanceCount;
	static HINSTANCE hInstance;
	const volatile HWND hWnd;
	const HWND hParent;

public:
	Window(LPCSTR className);
	virtual ~Window();
	const HWND handle() const;

	static void init(HINSTANCE hInst);
	static HINSTANCE instance();

protected:
	bool createWindow(DWORD dwExStyle, LPCSTR lpWindowName, DWORD dwStyle,
	                  int x, int y, int nWidth, int nHeight, HWND hWndParent);
	bool destroyWindow();

	static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void windowProc(Message& message);
};

#endif
